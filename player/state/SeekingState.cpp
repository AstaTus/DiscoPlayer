#include "SeekingState.h"
#include "../../render/audio/AudioDevice.h"
#include "../../codec/MediaDecoder.h"
#include "../../stream/MediaInputStream.h"
#include "../../render/video/VideoFrameTransformer.h"
#include "../../render/audio/AudioFrameTransformer.h"
#include "../ActivateNodeManager.h"
#include "../../common/log/Log.h"
#include "../RenderSynchronizer.h"
#include "StateManager.h"
#include "../../clock/SyncClockManager.h"

#define SEEK_STATE_ARG_SIZE 1

SeekingState::SeekingState(StateManager * state_manager,
                           AudioDevice * audio_device,
                           MediaInputStream * input_stream,
                           MediaDecoder * media_decoder,
                           VideoFrameTransformer * video_frame_transformer,
                           AudioFrameTransformer * audio_frame_transformer,
                           ActivateNodeManager * activate_node_manager,
                           SyncClockManager *  sync_clock_manager)
    : BaseState(),
      mpStateManager(state_manager),
      mpAudioDevice(audio_device),
      mpInputStream(input_stream),
      mpMediaDecoder(media_decoder),
      mpVideoFrameTransformer(video_frame_transformer),
      mpAudioFrameTransformer(audio_frame_transformer),
      mpActivateNodeManager(activate_node_manager),
      mpSyncClockManager(sync_clock_manager)
{
    mpRenderSynchronizer = new RenderSynchronizer(
        mpVideoFrameTransformer, mpAudioFrameTransformer, this);
}

SeekingState::~SeekingState()
{
}

void SeekingState::on_state_enter(...)
{
    va_list args;
    va_start(args,  SEEK_STATE_ARG_SIZE);
    int64_t postion = va_arg(args, int64_t);
    va_end(args);
    mSeekingFuture = std::async(std::launch::async, &SeekingState::inner_seek, this, postion);
}

void SeekingState::inner_seek(int64_t position)
{
    //TODO 此处有多线程时序问题
    Log::get_instance().log_debug("[Disco][SeekingState::inner_seek seek start]\n");
    //暂停音频播放
    //TODO 清除音频设备缓存
    
    //暂停视频播放，停留在最后一帧，在CorePlayer中暂停

    //暂停音视频Transform,且清除缓存
    // mpVideoFrameTransformer->clear_buffer_and_pause();
    // mpAudioFrameTransformer->clear_buffer_and_pause();
    //暂停音视频解码
    // mpMediaDecoder->clear_buffer_and_pause();
    //清除音视频当前播放展示的node缓存
    // mpActivateNodeManager->recyle_current_audio_node();
    // mpActivateNodeManager->recyle_current_video_node();
    //seek
    mpInputStream->seek(position);
    mpSyncClockManager->seek(position);
    //恢复音视频解码
    // mpMediaDecoder->resume();
    //恢复transform
    // mpVideoFrameTransformer->resume();
    // mpAudioFrameTransformer->resume();

    mpRenderSynchronizer->start(mpInputStream->get_serial());
}

void SeekingState::on_synchronizer_finish()
{
    mpStateManager->onSeekEnd();
}

void SeekingState::on_state_exit()
{
    //TODO  非内部切换state 可能导致 mpRenderSynchronizer 泄漏
    // 主要看外部哪些状态能从seeking 状态 切换到其他状态
    Log::get_instance().log_debug("[Disco][CorePlayer::seek end]");
}