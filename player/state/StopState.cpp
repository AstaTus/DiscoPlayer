
#include "StopState.h"
#include "../../stream/MediaInputStream.h"
#include "../../codec/MediaDecoder.h"
#include "../../render/video/VideoFrameTransformer.h"
#include "../../render/audio/AudioFrameTransformer.h"
#include "../RenderSynchronizer.h"
#include "StateManager.h"
#include "../RenderSynchronizer.h"
#include "../../render/audio/AudioDevice.h"

StopState::StopState(MediaInputStream *input_stream,
              MediaDecoder *media_decoder,
              VideoFrameTransformer *video_frame_transformer,
              AudioFrameTransformer *audio_frame_transformer,
              AudioDevice *audio_device,
              StateManager *state_manager)
    : mpMediaInputStream(input_stream),
      mpMediaDecoder(media_decoder),
      mpVideoFrameTransformer(video_frame_transformer),
      mpAudioFrameTransformer(audio_frame_transformer),
      mpAudioDevice(audio_device),
      mpStateManager(state_manager)
{
    mpRenderSynchronizer = new RenderSynchronizer(
        mpVideoFrameTransformer, mpAudioFrameTransformer, this);
}

StopState::~StopState()
{
    delete mpRenderSynchronizer;
}

void StopState::on_state_enter(...)
{
    mpStopThread = new std::thread(&StopState::inner_stop_task, this);
}

void StopState::on_state_exit()
{
}

void StopState::inner_stop_task()
{
    mpMediaInputStream->close();
    mpRenderSynchronizer->start_by_end();
}

void StopState::on_synchronizer_finish()
{
    //如果是准备好就起播，则进入playing 状态
    //如果是准备好就首帧展示，则进入prepared 模式
    mpStateManager->on_release();
}