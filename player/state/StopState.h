#pragma once
#include <thread>
#include "BaseState.h"
#include "../RenderSynchronizerFinishListener.h"

class MediaInputStream;
class MediaDecoder;
class VideoFrameTransformer;
class AudioFrameTransformer;
class AudioRender;
class RenderSynchronizer;
class StateManager;
class RenderSynchronizer;

class StopState : public BaseState, public RenderSynchronizerFinishListener
{
private:
    MediaInputStream *const mpMediaInputStream;
    MediaDecoder *const mpMediaDecoder;
    VideoFrameTransformer *const mpVideoFrameTransformer;
    AudioFrameTransformer *const mpAudioFrameTransformer;
    AudioRender *const mpAudioRender;

    std::thread *mpStopThread;
    StateManager *mpStateManager;
    RenderSynchronizer *mpRenderSynchronizer;

    void inner_stop_task();

    virtual void on_synchronizer_finish() override;

public:
    StopState(MediaInputStream *input_stream,
              MediaDecoder *media_decoder,
              VideoFrameTransformer *video_frame_transformer,
              AudioFrameTransformer *audio_frame_transformer,
              AudioRender *audio_render,
              StateManager *state_manager);
    ~StopState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};
