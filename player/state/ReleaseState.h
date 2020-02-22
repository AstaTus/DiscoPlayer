#pragma once
#include "BaseState.h"

#include <thread> 

class MediaInputStream;
class MediaDecoder;
class VideoFrameTransformer;
class AudioFrameTransformer;
class AudioDevice;
class RenderSynchronizer;
class StateManager;
class SyncClockManager;
class ActivateNodeManager;

class ReleaseState : public BaseState
{
private:
    MediaInputStream *const mpMediaInputStream;
    MediaDecoder *const mpMediaDecoder;
    VideoFrameTransformer *const mpVideoFrameTransformer;
    AudioFrameTransformer *const mpAudioFrameTransformer;
    AudioDevice *const mpAudioDevice;
    StateManager *mpStateManager;
    SyncClockManager *mpSyncClockManager;
    ActivateNodeManager * mpActivateNodeManager;

    std::thread * mpReleaseThread;

    void inner_release_task();

public:
    ReleaseState(MediaInputStream *input_stream,
                 MediaDecoder *media_decoder,
                 VideoFrameTransformer *video_frame_transformer,
                 AudioFrameTransformer *audio_frame_transformer,
                 ActivateNodeManager *activate_node_manager,
                 AudioDevice *audio_device,
                 StateManager *state_manager,
                 SyncClockManager *sync_clock_manager);
    ~ReleaseState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};
