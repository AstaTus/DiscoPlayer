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

#include "PlayingState.h"

#define REFRESH_RATE 0.01

CorePlayer::CorePlayer()
    : pMediaDecoder(nullptr),
      pVideoRender(nullptr),
      pRenderView(nullptr),
      pInputStream(nullptr),
      mVideoFrameTransformer(),
      pCurrentVideoNode(nullptr),
      pCurrentAudioNode(nullptr),
      pCurrentPlayItem(nullptr),
      mIsStop(false),
      mStateManager(),
      mSyncClockManager(SyncClockManager::SYNC_STRATEGY_AUDIO)
{

}

CorePlayer::~CorePlayer()
{
    mIsStop.store(true);
    mVideoRenderFuture.wait();
    mAudioRenderFuture.wait();
    mVideoTransformFuture.wait();
    mAudioTransformFuture.wait();

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
    pMediaDecoder->start();
    pAudioDevice->start();

    mVideoRenderFuture = std::async(std::launch::async, &CorePlayer::video_render_loop_task, this);
    // mAudioRenderFuture = std::async(std::launch::async, &CorePlayer::audio_render_loop_task, this);
    mVideoTransformFuture = std::async(std::launch::async, &CorePlayer::video_frame_transform_loop_task, this);
    mAudioTransformFuture = std::async(std::launch::async, &CorePlayer::audio_frame_transform_loop_task, this);
}

void CorePlayer::video_frame_transform_loop_task()
{
    //只要播放器不销毁，就一直循环
    while (mStateManager.get_play_state() != StateManager::PlayState::RELEASING)
    {
        //video
        FrameWrapper *video_frame_wrapper = pMediaDecoder->pop_frame(AVMEDIA_TYPE_VIDEO);
        mVideoFrameTransformer.push_frame_to_transform(video_frame_wrapper, pRenderView->get_width(), pRenderView->get_height());
        Log::get_instance().log_debug("[Disco][CorePlayer] video_frame_transform_loop_task add frame to transform\n");
    }

    Log::get_instance().log_debug("[Disco][CorePlayer] video_frame_transform_loop_task thread over\n");
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
            VideoTransformNode *node = mVideoFrameTransformer.non_block_peek_transformed_node();
            if (node == nullptr)
            {
                break;
            }

            //非当前serial的帧 回收
            if (node->frame_wrapper->serial != pInputStream->get_serial() || 
                node->frame_wrapper->frame->pts * 1000 * av_q2d(node->frame_wrapper->time_base) < pInputStream->get_serial_start_time())
            {
                /* code */
                mVideoFrameTransformer.non_block_pop_transformed_node();
                mVideoFrameTransformer.recycle(node);
                pMediaDecoder->recycle_frame(AVMEDIA_TYPE_VIDEO, node->frame_wrapper);
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
                if (pCurrentVideoNode != nullptr)
                {
                    pVideoRender->refresh(pCurrentVideoNode->image);
                }
            }
            else
            {
                mVideoFrameTransformer.non_block_pop_transformed_node();
                //回收上一帧
                if (pCurrentVideoNode != nullptr)
                {
                    pVideoRender->invalid_image();
                    mVideoFrameTransformer.recycle(pCurrentVideoNode);
                    pMediaDecoder->recycle_frame(AVMEDIA_TYPE_VIDEO, pCurrentVideoNode->frame_wrapper);
                    pCurrentVideoNode = nullptr;
                }

                if (sync_state == SyncClockManager::SyncState::SYNC_STATE_NEXT)
                {
                    //显示下一帧
                    pCurrentVideoNode = node;
                    pVideoRender->refresh(pCurrentVideoNode->image);
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
    if (pCurrentAudioNode != nullptr)
    {
        if (pCurrentAudioNode->clip->isFinish())
        {
            pMediaDecoder->recycle_frame(AVMEDIA_TYPE_AUDIO, pCurrentAudioNode->frame_wrapper);
            mAudioFrameTransformer.recycle(pCurrentAudioNode);
            pCurrentAudioNode = nullptr;
        }
    }
}

AudioClip *const CorePlayer::on_audio_data_request(int len)
{
    double remaining_time = 0.0;
    //audio
    if (pCurrentAudioNode == nullptr)
    {
        pCurrentAudioNode = mAudioFrameTransformer.non_block_pop_transformed_node();
    }

    if (pCurrentAudioNode != nullptr)
    {
        //播放完或者非当前serial的帧 回收
        if (pCurrentAudioNode->clip->isFinish() || 
            (pCurrentAudioNode->frame_wrapper->serial != pInputStream->get_serial() || 
            pCurrentAudioNode->frame_wrapper->frame->pts * av_q2d(pCurrentAudioNode->frame_wrapper->time_base) * 1000 < pInputStream->get_serial_start_time()))
        {
            pMediaDecoder->recycle_frame(AVMEDIA_TYPE_AUDIO, pCurrentAudioNode->frame_wrapper);
            mAudioFrameTransformer.recycle(pCurrentAudioNode);
            pCurrentAudioNode = nullptr;
            return nullptr;
        } 
        else 
        {
            mSyncClockManager.get_current_audio_sync_state(
                pCurrentAudioNode->frame_wrapper->frame->pts,
                pCurrentAudioNode->frame_wrapper->time_base,
                pCurrentAudioNode->frame_wrapper->serial,
                &remaining_time);
            
            return pCurrentAudioNode->clip;
        }
    }

    return nullptr;
    //同步音频
    // audio_transform_node->clip
    //     pAudioRender->refresh();
}

void CorePlayer::audio_frame_transform_loop_task()
{
    while (mStateManager.get_play_state() != StateManager::PlayState::RELEASING)
    {
        //video
        FrameWrapper *audio_frame_wrapper = pMediaDecoder->pop_frame(AVMEDIA_TYPE_AUDIO);
        mAudioFrameTransformer.push_frame_to_transform(audio_frame_wrapper);
        Log::get_instance().log_debug("[Disco][CorePlayer] audio_frame_transform_loop_task add frame to transform\n");
    }

    Log::get_instance().log_debug("[Disco][CorePlayer] audio_frame_transform_loop_task thread over\n");
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
    if (!pMediaDecoder->is_seeking())
    {
        pAudioDevice->pause();
        pMediaDecoder->pause();
        pInputStream->seek(position);
        pMediaDecoder->resume();
        pAudioDevice->resume();
        Log::get_instance().log_debug("[Disco][CorePlayer::seek]");
    }
}