#ifndef __PLAYING_STATE_H__
#define __PLAYING_STATE_H__
#include "BaseState.h"
#include <future>
#include <atomic>
#include "../../common/thread/Semaphore.h"
#include "../../render/audio/AudioDataRequestListener.h"

class MediaInputStream;
class MediaDecoder;
class VideoFrameTransformer;
class AudioFrameTransformer;
class AudioDevice;
class ActivateNodeManager;
class SyncClockManager;
class StateManager;
class AudioClip;
class VideoRender;
class AudioRender;
struct AudioTransformNode;
class PlayingState: public BaseState, public AudioDataRequestListener
{
private:
    VideoRender * mpVideoRender;
    AudioRender * mpAudioRender;
    AudioDevice * mpAudioDevice;
    ActivateNodeManager * mpActivateNodeManager;
    SyncClockManager * mpSyncClockManager;
    MediaInputStream * mpInputStream;
    StateManager * mpStateManager;

    std::future<void> mVideoRenderFuture;
    std::atomic<bool> mIsExit;
    std::atomic<bool> mIsRelease;

    std::atomic<bool> mIsVideoStreamEnd;
    std::atomic<bool> mIsAudioStreamEnd;

    Semaphore mRenderSemaphore;

    double mStartAudioDataRequestBeginTime;

    void on_inner_state_enter();
    void video_render_loop_task();
    void on_audio_data_request_begin() override;
    void on_audio_data_request_end() override;
    bool on_audio_data_request(AudioTransformNode ** node) override;

    void check_stream_end();

public:
    PlayingState(VideoRender *video_render,
                    AudioRender *audio_render,
                           AudioDevice *audio_device,
                           ActivateNodeManager *activate_node_manager,
                           SyncClockManager *sync_clock_manager,
                           MediaInputStream *media_input_stream,
                           StateManager *state_manager);
    ~PlayingState();

    virtual void on_state_enter(...) override;
    virtual void on_state_exit() override;
};
#endif