#ifndef __SEEKING_STATE_H__
#define __SEEKING_STATE_H__

#include <stdint.h>
#include "../RenderSynchronizerFinishListener.h"
#include <future>
#include "BaseState.h"

class AudioDevice;
class MediaInputStream;
class MediaDecoder;
class VideoFrameTransformer;
class AudioFrameTransformer;
class ActivateNodeManager;
class RenderSynchronizer;
class StateManager;
class SeekingState : public BaseState, public RenderSynchronizerFinishListener
{
private:
    StateManager * const mpStateManager;
    AudioDevice * const mpAudioDevice;
    MediaInputStream * const mpInputStream;
    MediaDecoder * const mpMediaDecoder;
    VideoFrameTransformer * const mpVideoFrameTransformer;
    AudioFrameTransformer * const mpAudioFrameTransformer;
    ActivateNodeManager * const mpActivateNodeManager;

    RenderSynchronizer * mpRenderSynchronizer;

    std::future<void> mSeekingFuture;

    void inner_seek(int64_t position);

    virtual void on_synchronizer_finish() override;
public:
    SeekingState(StateManager * state_manager,
                AudioDevice * audio_device, 
                MediaInputStream * input_stream,
                MediaDecoder * media_decoder,
                VideoFrameTransformer * video_frame_transformer,
                AudioFrameTransformer * audio_frame_transformer,
                ActivateNodeManager * activate_node_manager);
    ~SeekingState();

    virtual void on_state_enter(...) override;
    void on_state_exit() override;
};

#endif
