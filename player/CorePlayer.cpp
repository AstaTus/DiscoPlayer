#include "CorePlayer.h"

#include "../render/video/VideoFrameTransformer.h"
#include "../render/video/VideoTransformNode.h"
#include "../codec/FrameWrapper.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/time.h"
}

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
    pInputStream = new MediaInputStream();
    pInputStream->open(pCurrentPlayItem->get_data_source());
    pMediaDecoder = new MediaDecoder(pInputStream->get_stream_iterator(),
                                     pInputStream->get_packet_reader());
    pMediaDecoder->start();

    pAudioDevice->open();

    mVideoRenderFuture = std::async(std::launch::async, &CorePlayer::video_render_loop_task, this);
    // mAudioRenderFuture = std::async(std::launch::async, &CorePlayer::audio_render_loop_task, this);
    mVideoTransformFuture = std::async(std::launch::async, &CorePlayer::video_frame_transform_loop_task, this);
    mAudioTransformFuture = std::async(std::launch::async, &CorePlayer::audio_frame_transform_loop_task, this);
}

void CorePlayer::video_frame_transform_loop_task()
{
    //TODO 受状态控制
    while (!mIsStop)
    {
        //video
        FrameWrapper *video_frame_wrapper = pMediaDecoder->pop_frame(AVMEDIA_TYPE_VIDEO);
        mVideoFrameTransformer.push_frame_to_transform(video_frame_wrapper, pRenderView->get_width(), pRenderView->get_height());
        av_log(nullptr, AV_LOG_DEBUG, "[Disco][CorePlayer] video_frame_transform_loop_task add frame to transform\n");
    }

    av_log(nullptr, AV_LOG_DEBUG, "[Disco][CorePlayer] video_frame_transform_loop_task thread over\n");
}

void CorePlayer::video_render_loop_task()
{
    SyncClockManager::SyncState sync_state;
    double remaining_time = 0.0;
    //TODO 受状态控制
    while (true)
    {
        // if(remaining_time > 0.0) {
        //     av_usleep((int)(int64_t)(remaining_time * 1000000.0));
        // }

        remaining_time = REFRESH_RATE;
        //TODO sleep
        do
        {
            VideoTransformNode *node = mVideoFrameTransformer.non_block_peek_transformed_node();
            if (node == NULL)
            {
                break;
            }

            sync_state = mSyncClockManager.get_current_video_sync_state(
                node->frame_wrapper->frame->pts,
                node->frame_wrapper->time_base,
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
                if (pCurrentVideoNode != NULL)
                {
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

    av_log(nullptr, AV_LOG_DEBUG, "[Disco][CorePlayer] video_render_loop_task thread over\n");
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

AudioClip * const CorePlayer::on_audio_data_request(int len)
{
    double remaining_time = 0.0;
    //audio
    if (pCurrentAudioNode != nullptr)
    {
        if (pCurrentAudioNode->clip->isFinish())
        {
            pMediaDecoder->recycle_frame(AVMEDIA_TYPE_AUDIO, pCurrentAudioNode->frame_wrapper);
            mAudioFrameTransformer.recycle(pCurrentAudioNode);
            pCurrentAudioNode = nullptr;
        } else {
            return pCurrentAudioNode->clip;
        }
    }
    
    pCurrentAudioNode = mAudioFrameTransformer.non_block_pop_transformed_node();
    if (pCurrentAudioNode != nullptr)
    {
        mSyncClockManager.get_current_audio_sync_state(
            pCurrentAudioNode->frame_wrapper->frame->pts,
                pCurrentAudioNode->frame_wrapper->time_base,
                &remaining_time);
                
        return pCurrentAudioNode->clip;
    }
    
    return nullptr;
    //同步音频
    // audio_transform_node->clip
    //     pAudioRender->refresh();

    
    
}

void CorePlayer::audio_frame_transform_loop_task()
{
    //TODO 受状态控制
    while (!mIsStop)
    {
        //video
        FrameWrapper *audio_frame_wrapper = pMediaDecoder->pop_frame(AVMEDIA_TYPE_AUDIO);
        mAudioFrameTransformer.push_frame_to_transform(audio_frame_wrapper);
        av_log(nullptr, AV_LOG_DEBUG, "[Disco][CorePlayer] audio_frame_transform_loop_task add frame to transform\n");
    }

    av_log(nullptr, AV_LOG_DEBUG, "[Disco][CorePlayer] audio_frame_transform_loop_task thread over\n");
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
}

void CorePlayer::resume()
{
}

void CorePlayer::stop()
{
}