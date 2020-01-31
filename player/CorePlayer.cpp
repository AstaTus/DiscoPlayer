#include "CorePlayer.h"

#include "../render/video/VideoFrameTransformer.h"
#include "../render/video/VideoTransformNode.h"
#include "../codec/FrameWrapper.h"
#include "../common/log/Log.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/time.h"
}

#include "./state/PlayingState.h"

#define REFRESH_RATE 0.01

CorePlayer::CorePlayer()
    : pMediaDecoder(nullptr),
      pVideoRender(nullptr),
      pRenderView(nullptr),
      pInputStream(nullptr),
      pCurrentPlayItem(nullptr),
      mIsStop(false),
      mSyncClockManager(SyncClockManager::SYNC_STRATEGY_AUDIO),
      mStateManager()
{
}

CorePlayer::~CorePlayer()
{
    mIsStop.store(true);
    mVideoRenderFuture.wait();
    mAudioRenderFuture.wait();
    mpVideoFrameTransformer->stop();
    mpAudioFrameTransformer->stop();

    delete mpVideoFrameTransformer;
    delete mpAudioFrameTransformer;
    delete mpActivateNodeManager;

    pRenderView = nullptr;
    pAudioDevice = nullptr;
    delete pVideoRender;
    delete pAudioRender;
}

void CorePlayer::set_play_item(PlayItem *play_item)
{
    if (pCurrentPlayItem == nullptr && play_item->get_data_source().size() > 0)
    {
        pCurrentPlayItem = play_item;
    }
    else
    {
    }
}

void CorePlayer::replace_play_item(const PlayItem *const play_item)
{
}

void CorePlayer::set_render_surface(RenderView *render_view)
{
    pRenderView = render_view;
    pVideoRender = new VideoRender(pRenderView);
}

void CorePlayer::set_audio_device(AudioDevice *audio_device)
{
    pAudioDevice = audio_device;
    pAudioDevice->set_audio_data_request_listener(this);
    pAudioRender = new AudioRender(pAudioDevice);
}

void CorePlayer::init_task()
{
    init_states();
    mStateManager.onInit();
    pInputStream = new MediaInputStream();
    pInputStream->open(pCurrentPlayItem->get_data_source());
    pMediaDecoder = new MediaDecoder(pInputStream->get_stream_iterator(),
                                     pInputStream->get_packet_reader());
    mpVideoFrameTransformer = new VideoFrameTransformer(pMediaDecoder->get_video_frame_reader());
    mpAudioFrameTransformer = new AudioFrameTransformer(pMediaDecoder->get_audio_frame_reader());
    mpActivateNodeManager = new ActivateNodeManager(mpVideoFrameTransformer, mpAudioFrameTransformer);

    mpVideoFrameTransformer->on_resize_render_view(
        pRenderView->get_width(), pRenderView->get_height());
    pMediaDecoder->start();
    mpAudioFrameTransformer->start();
    mpVideoFrameTransformer->start();
    pAudioDevice->start();

    mVideoRenderFuture = std::async(std::launch::async, &CorePlayer::video_render_loop_task, this);    
}



void CorePlayer::video_render_loop_task()
{
    SyncClockManager::SyncState sync_state;
    double remaining_time = 0.0;
    while (mStateManager.get_play_state() != StateManager::PlayState::RELEASING)
    {
        
        remaining_time = REFRESH_RATE;
        //TODO sleep
        do
        {
            VideoTransformNode *node = mpActivateNodeManager->peek_video_queue_node();
            if (node == nullptr)
            {
                break;
            }

            //非当前serial的帧 回收
            if (node->frame_wrapper->serial != pInputStream->get_serial() || 
                node->frame_wrapper->frame->pts * 1000 * av_q2d(node->frame_wrapper->time_base) < pInputStream->get_serial_start_time())
            {
                mpActivateNodeManager->recyle_peek_video_node();
                break;
            }
            
            if(!mStateManager.onFirstFramePrepared() && mStateManager.get_play_state() != StateManager::PlayState::PLAYING) {
                break;
            }

            sync_state = mSyncClockManager.get_current_video_sync_state(
                node->frame_wrapper->frame->pts,
                node->frame_wrapper->time_base,
                node->frame_wrapper->serial,
                &remaining_time);

            if (sync_state == SyncClockManager::SyncState::SYNC_STATE_KEEP)
            {
                //显示当前帧
                if (mpActivateNodeManager->get_current_video_node() != nullptr)
                {
                    pVideoRender->refresh(mpActivateNodeManager->get_current_video_node()->image);
                }
            }
            else
            {
                mpActivateNodeManager->obtain_current_video_node();

                if (sync_state == SyncClockManager::SyncState::SYNC_STATE_NEXT)
                {
                    //显示下一帧
                    if (mpActivateNodeManager->get_current_video_node())
                    {
                        pVideoRender->refresh(mpActivateNodeManager->get_current_video_node()->image);
                    }
                }
            }
        } while (sync_state == SyncClockManager::SyncState::SYNC_STATE_DROP);
        
    }
    Log::get_instance().log_debug("[Disco][CorePlayer] video_render_loop_task thread over\n");
}

void CorePlayer::on_audio_data_request_begin()
{
}

void CorePlayer::on_audio_data_request_end()
{
    if (mpActivateNodeManager->get_current_audio_node() != nullptr && 
        mpActivateNodeManager->get_current_audio_node()->clip->isFinish())
    {
        mpActivateNodeManager->recyle_current_audio_node();
    }
}

AudioClip *const CorePlayer::on_audio_data_request(int len)
{
    double remaining_time = 0.0;
    //audio
    if (mpActivateNodeManager->get_current_audio_node() == nullptr)
    {
        mpActivateNodeManager->obtain_current_audio_node();
    }

    AudioTransformNode * node = mpActivateNodeManager->get_current_audio_node();
    if (node != nullptr)
    {
        //播放完或者非当前serial的帧 回收
        if (node->clip->isFinish() || 
            (node->frame_wrapper->serial != pInputStream->get_serial() || 
            node->frame_wrapper->frame->pts * av_q2d(node->frame_wrapper->time_base) * 1000 < pInputStream->get_serial_start_time()))
        {
            mpActivateNodeManager->recyle_current_audio_node();
            return nullptr;
        }
        else 
        {
            mSyncClockManager.get_current_audio_sync_state(
                node->frame_wrapper->frame->pts,
                node->frame_wrapper->time_base,
                node->frame_wrapper->serial,
                &remaining_time);
            
            return node->clip;
        }
    }

    return nullptr;
    //同步音频
    // audio_transform_node->clip
    //     pAudioRender->refresh();
}

void CorePlayer::start()
{
    if (pVideoRender == NULL || pCurrentPlayItem == NULL)
    {
        //TODO throw exception
    }
    else
    {
        mInitFuture = std::async(std::launch::async, &CorePlayer::init_task, this);
    }
}

void CorePlayer::pause()
{
    mStateManager.onPauseByUser();
}

void CorePlayer::resume()
{
    mStateManager.onResumeByUser();
}

void CorePlayer::stop()
{
    mStateManager.onPauseByUser();
}

void CorePlayer::init_states()
{
    PlayingState * playing_state = new PlayingState(pAudioDevice);
    mStateManager.add_state(StateManager::PlayState::PLAYING, playing_state);
}

StateManager::PlayState CorePlayer::get_current_play_state()
{
    return mStateManager.get_play_state();
}

int64_t CorePlayer::get_duration()
{
    if (pInputStream != nullptr)
    {
        return pInputStream->get_duration();
    }

    return 0;
}

int64_t CorePlayer::get_current_position()
{
    return mSyncClockManager.get_current_position();
}

void CorePlayer::seek(int64_t position)
{
    std::async(std::launch::async, &CorePlayer::seek_task, this, position);    
}

void CorePlayer::seek_task(int64_t position)
{

}