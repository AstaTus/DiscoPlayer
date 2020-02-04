#include "InitState.h"
#include "../../stream/MediaInputStream.h"
#include "../../codec/MediaDecoder.h"
#include "../../render/video/VideoFrameTransformer.h"
#include "../../render/audio/AudioFrameTransformer.h"
#include "../RenderSynchronizer.h"
#include "StateManager.h"
#include "../RenderSynchronizer.h"
#include "../../render/audio/AudioDevice.h"

#define INIT_STATE_ARG_SIZE 3
InitState::InitState(MediaInputStream *input_stream,
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

InitState::~InitState()
{
}

void InitState::on_state_enter(...)
{
    va_list args;
    va_start(args, INIT_STATE_ARG_SIZE);
    mIsStartPaused = va_arg(args, bool);
    int64_t start_seek_postion = va_arg(args, int64_t);
    const char * data_source = va_arg(args, const char *);
    va_end(args);

    mInitFuture = std::async(std::launch::async, &InitState::inner_init_task, this, start_seek_postion, data_source);
}

void InitState::inner_init_task(int64_t seek_position, const char * data_source)
{
    mpMediaInputStream->open(data_source);
    mpMediaDecoder->start();
    mpVideoFrameTransformer->start();
    mpAudioFrameTransformer->start();
    mpAudioDevice->start();

    mpRenderSynchronizer->start(mpMediaInputStream->get_serial());
}

void InitState::on_state_exit()
{
}

void InitState::on_synchronizer_finish()
{
    //如果是准备好就起播，则进入playing 状态
    //如果是准备好就首帧展示，则进入prepared 模式
    mpStateManager->onFirstFramePrepared(mIsStartPaused);
}