#include "PlayingState.h"
#include "../../render/audio/AudioDevice.h"
#include "../../render/video/VideoFrameTransformer.h"
#include "../../render/video/VideoTransformNode.h"
#include "../../render/audio/AudioTransformNode.h"
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

#include "../../common/thread/ThreadUtils.h"

#include "StateManager.h"

#include "../ActivateNodeManager.h"

#include <thread>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/time.h"
}

#define REFRESH_RATE 10

PlayingState::PlayingState(VideoRender *video_render,
                           AudioRender * audio_render,
                           AudioDevice *audio_device,
                           ActivateNodeManager *activate_node_manager,
                           SyncClockManager *sync_clock_manager,
                           MediaInputStream *media_input_stream,
                           StateManager *state_manager)
    : BaseState(),
      mpVideoRender(video_render),
      mpAudioRender(audio_render),
      mpAudioDevice(audio_device),
      mpActivateNodeManager(activate_node_manager),
      mpSyncClockManager(sync_clock_manager),
      mpInputStream(media_input_stream),
      mpStateManager(state_manager),
      mIsExit(true),
      mIsRelease(false),
      mIsVideoStreamEnd(false),
      mIsAudioStreamEnd(false),
      mRenderSemaphore(),
      mStartAudioDataRequestBeginTime(0.0)
{
    mpAudioRender->set_audio_data_request_listener(this);
    mVideoRenderFuture = std::async(std::launch::async, &PlayingState::video_render_loop_task, this);
}

PlayingState::~PlayingState()
{
    mIsRelease = true;
    mRenderSemaphore.signal();
    mVideoRenderFuture.wait();

    mpVideoRender = nullptr;
    mpAudioDevice = nullptr;
    mpActivateNodeManager = nullptr;
    mpInputStream = nullptr;
    mpSyncClockManager = nullptr;
    mpStateManager = nullptr;
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
    Log::get_instance().log_debug("[Disco][PlayingState] on_inner_state_enter\n");
}

void PlayingState::on_state_exit()
{
    mIsExit = true;
}

void PlayingState::video_render_loop_task()
{
    ThreadUtils::set_thread_prority(99);
    SyncClockManager::SyncState sync_state;
    long long remaining_time = REFRESH_RATE;;
    while (!mIsRelease)
    {
        if (mIsExit)
        {
            mRenderSemaphore.wait();
            mIsExit = false;
            remaining_time = 0;
            Log::get_instance().log_debug("[Disco][PlayingState] video_render_loop_task resume\n");
            continue;
        }
        Log::get_instance().log_debug("[Disco][PlayingState] video_render_loop_task sleep milliseconds = %lld\n", remaining_time);
        std::this_thread::sleep_for(std::chrono::milliseconds(remaining_time));//毫秒
        do
        {
            VideoTransformNode *node = mpActivateNodeManager->peek_video_queue_node();
            if (node == nullptr)
            {
                Log::get_instance().log_debug("[Disco][PlayingState] peek_video_queue_node node is nullptr\n");
                break;
            }

            if (node->is_end)
            {
                mIsVideoStreamEnd = true;
                mIsRelease = true;
                break;
            }

            //非当前serial的帧 回收
            if (node->frame_wrapper->serial != mpInputStream->get_serial() ||
                node->frame_wrapper->get_transformed_pts() < mpInputStream->get_serial_start_time())
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
                    Log::get_instance().log_debug("[Disco][PlayingState] video_render_loop_task refresh keep frame\n");
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
                        Log::get_instance().log_debug("[Disco][PlayingState] video_render_loop_task refresh next frame\n");
                        mpVideoRender->refresh(mpActivateNodeManager->get_current_video_node()->image);
                    }
                }
            }
        } while (sync_state == SyncClockManager::SyncState::SYNC_STATE_DROP);
    }

    check_stream_end();
    Log::get_instance().log_debug("[Disco][CorePlayer] video_render_loop_task thread over\n");
}

void PlayingState::on_audio_data_request_begin()
{
    mStartAudioDataRequestBeginTime = av_gettime_relative() / 1000000.0;
}

bool PlayingState::on_audio_data_request(AudioTransformNode ** audio_node)
{
    double remaining_time = 0.0;
    //audio
    AudioTransformNode *node;

    node = mpActivateNodeManager->obtain_current_audio_node();

    if (node == nullptr)
    {
        mpActivateNodeManager->recyle_current_audio_node();
        (*audio_node) = nullptr;
        return true;
    }

    if (node->is_end)
    {
        mIsAudioStreamEnd = true;   
        check_stream_end();
        return false;
    }

    //非当前serial的帧 回收
    if ((node->frame_wrapper->serial != mpInputStream->get_serial() ||
            node->frame_wrapper->frame->pts * av_q2d(node->frame_wrapper->time_base) * 1000 < mpInputStream->get_serial_start_time()))
    {
        mpActivateNodeManager->recyle_current_audio_node();
        (*audio_node) = nullptr;
        return false;
    }
    else
    {
        Log::get_instance().log_debug("[Disco][PlayinState] on_audio_data_request add audio clip serial = %d\n", node->frame_wrapper->serial);
        (*audio_node) = node;
        return true;
    }
}

void PlayingState::on_audio_data_request_end()
{
    long long remaining_time = 0;
    if (mpActivateNodeManager->get_current_audio_node() != nullptr)
    {
        mpSyncClockManager->get_current_audio_sync_state(
                mpActivateNodeManager->get_current_audio_node()->frame_wrapper->frame->pts - 
                    mpAudioRender->get_cache_buffer_size() * 1000 / mpAudioDevice->get_bytes_per_second() -
                    mpAudioDevice->get_latency_seconds() * 1000,
                mpActivateNodeManager->get_current_audio_node()->frame_wrapper->time_base,
                mpActivateNodeManager->get_current_audio_node()->frame_wrapper->serial,
                &remaining_time, mStartAudioDataRequestBeginTime);
    }
}

void PlayingState::check_stream_end()
{
    if (mIsVideoStreamEnd && mIsAudioStreamEnd)
    {
        mpStateManager->on_play_completed();
    }
    
}
