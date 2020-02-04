#ifndef __INIT_STATE_H__
#define __INIT_STATE_H__
#include "BaseState.h"
#include <future>
#include "../RenderSynchronizerFinishListener.h"

class MediaInputStream;
class MediaDecoder;
class VideoFrameTransformer;
class AudioFrameTransformer;
class AudioDevice;
class RenderSynchronizer;
class StateManager;
class InitState : public BaseState, public RenderSynchronizerFinishListener
{
private:
    MediaInputStream * const mpMediaInputStream;
    MediaDecoder * const mpMediaDecoder;
    VideoFrameTransformer * const mpVideoFrameTransformer;
    AudioFrameTransformer * const mpAudioFrameTransformer;
    AudioDevice * const mpAudioDevice;

    RenderSynchronizer * mpRenderSynchronizer;
    StateManager * mpStateManager;
    std::future<void> mInitFuture;

    bool mIsStartPaused;
    bool mStartSeekPosition;
    virtual void on_synchronizer_finish() override;

    void inner_init_task(int64_t seek_position, const char * data_source);

    

public:
    InitState(MediaInputStream *input_stream,
                     MediaDecoder *media_decoder,
                     VideoFrameTransformer *video_frame_transformer,
                     AudioFrameTransformer *audio_frame_transformer,
                     AudioDevice *audio_device,
                     StateManager *state_manager);
    ~InitState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};
#endif
