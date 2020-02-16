#include "PlayingState.h"
#include "../../render/audio/AudioDevice.h"
#include "../../render/video/VideoFrameTransformer.h"
#include "../../render/video/VideoTransformNode.h"
#include "../../codec/FrameWrapper.h"
#include "../../common/log/Log.h"
#include "../PlayItem.h"
#include "../../stream/MediaInputStream.h"
#include "../../codec/MediaDecoder.h"

#include "../../render/video/VideoRender.h"
#include "../../render/video/RenderView.h"
#include "../../render/video/VideoFrameTransformer.h"

#include "../../render/audio/AudioRender.h"
#include "../../render/audio/AudioDevice.h"
#include "../../render/audio/AudioFrameTransformer.h"
#include "../../render/audio/AudioClip.h"
#include "../../clock/SyncClockManager.h"

#include "StateManager.h"

#include "../ActivateNodeManager.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/time.h"
}

#define REFRESH_RATE 0.01

PlayingState::PlayingState(VideoRender *video_render,
                           AudioDevice *audio_device,
                           ActivateNodeManager *activate_node_manager,
                           SyncClockManager *sync_clock_manager,
                           MediaInputStream *media_input_stream,
                           StateManager *state_manager)
    : BaseState(),
      mpVideoRender(video_render),
      mpAudioDevice(audio_device),
      mpActivateNodeManager(activate_node_manager),
      mpSyncClockManager(sync_clock_manager),
      mpInputStream(media_input_stream),
      mpStateManager(state_manager),
      mIsExit(true),
      mIsRelease(false),
      mRenderSemaphore(),
      mRenderAudioSemaphore(),
      mStartAudioDataRequestBeginTime(0.0)
{
    mpAudioDevice->set_audio_data_request_listener(this);
    mVideoRenderFuture = std::async(std::launch::async, &PlayingState::video_render_loop_task, this);
}

PlayingState::~PlayingState()
{
}
void PlayingState::on_state_enter(...)
{
    on_inner_state_enter();
}

void PlayingState::on_inner_state_enter()
{
    mpAudioDevice->start();
    mpAudioDevice->resume();
    mRenderSemaphore.signal();
}

void PlayingState::on_state_exit()
{
    mIsExit = true;
}

void PlayingState::video_render_loop_task()
{
    SyncClockManager::SyncState sync_state;
    double remaining_time = 0.0;
    while (!mIsRelease)
    {
        if (mIsExit)
        {
            mRenderSemaphore.wait();
            mIsExit = false;
        }

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
            if (node->frame_wrapper->serial != mpInputStream->get_serial() ||
                node->frame_wrapper->frame->pts * 1000 * av_q2d(node->frame_wrapper->time_base) < mpInputStream->get_serial_start_time())
            {
                mpActivateNodeManager->recyle_peek_video_node();
                break;
            }

            sync_state = mpSyncClockManager->get_current_video_sync_state(
                node->frame_wrapper->frame->pts,
                node->frame_wrapper->time_base,
                node->frame_wrapper->serial,
                &remaining_time);

            if (sync_state == SyncClockManager::SyncState::SYNC_STATE_KEEP)
            {
                //显示当前帧
                if (mpActivateNodeManager->get_current_video_node() != nullptr)
                {
                    mpVideoRender->refresh(mpActivateNodeManager->get_current_video_node()->image);
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
                        mpVideoRender->refresh(mpActivateNodeManager->get_current_video_node()->image);
                    }
                }
            }
        } while (sync_state == SyncClockManager::SyncState::SYNC_STATE_DROP);
    }
    Log::get_instance().log_debug("[Disco][CorePlayer] video_render_loop_task thread over\n");
}

void PlayingState::on_audio_data_request_begin()
{
    mStartAudioDataRequestBeginTime = av_gettime_relative() / 1000000.0;
}

void PlayingState::on_audio_data_request_end()
{
    double remaining_time = 0.0;
    if (mpActivateNodeManager->get_current_audio_node() != nullptr)
    {
        mpSyncClockManager->get_current_audio_sync_state(
                mpActivateNodeManager->get_current_audio_node()->frame_wrapper->frame->pts - 
                    mpActivateNodeManager->get_current_audio_node()->clip->size() * 1000 / mpAudioDevice->get_bytes_per_second() -
                    mpAudioDevice->get_latency_seconds() * 1000,
                mpActivateNodeManager->get_current_audio_node()->frame_wrapper->time_base,
                mpActivateNodeManager->get_current_audio_node()->frame_wrapper->serial,
                &remaining_time, mStartAudioDataRequestBeginTime);

        if (mpActivateNodeManager->get_current_audio_node()->clip->isFinish())
        {
            mpActivateNodeManager->recyle_current_audio_node();
        }
    }
}

bool PlayingState::on_audio_data_request(int len, AudioClip ** audio_clip)
{
    double remaining_time = 0.0;
    //audio
    if (mpActivateNodeManager->get_current_audio_node() == nullptr)
    {
        mpActivateNodeManager->obtain_current_audio_node();
    }

    AudioTransformNode *node = mpActivateNodeManager->get_current_audio_node();
    if (node == nullptr)
    {
        (*audio_clip) = nullptr;
        return true;
    }
    
    
    //播放完或者非当前serial的帧 回收
    if (node->clip->isFinish() ||
        (node->frame_wrapper->serial != mpInputStream->get_serial() ||
            node->frame_wrapper->frame->pts * av_q2d(node->frame_wrapper->time_base) * 1000 < mpInputStream->get_serial_start_time()))
    {
        mpActivateNodeManager->recyle_current_audio_node();
        (*audio_clip) = nullptr;
        return false;
    }
    else
    {
        Log::get_instance().log_debug("[Disco][PlayinState] on_audio_data_request add audio clip serial = %d\n", node->frame_wrapper->serial);
        (*audio_clip) = node->clip;
        return true;
    }
    //同步音频
    // audio_transform_node->clip
    //     pAudioRender->refresh();
}
