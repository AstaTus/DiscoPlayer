#ifndef __CORE_PLAYER_H__
#define __CORE_PLAYER_H__

#include "PlayItem.h"
#include "../stream/MediaInputStream.h"
#include "../codec/MediaDecoder.h"

#include "../render/video/VideoRender.h"
#include "../render/video/RenderView.h"
#include "../render/video/VideoFrameTransformer.h"
#include "../render/audio/AudioDevice.h"
#include "../render/audio/AudioRender.h"

#include "../render/audio/AudioFrameTransformer.h"

#include "../clock/SyncClockManager.h"

#include "state/StateManager.h"

#include "ActivateNodeManager.h"
#include <future>

class StateChangedListener;
struct DebugInfo;

class CorePlayer
{
private:
    MediaDecoder * pMediaDecoder;
    VideoRender * pVideoRender;
    AudioRender * pAudioRender;

    RenderView * pRenderView;
    AudioDevice * pAudioDevice;

    MediaInputStream * pInputStream;

    PlayItem * pCurrentPlayItem;

    std::future<void> mInitFuture;
    std::future<void> mVideoRenderFuture;

    VideoFrameTransformer * mpVideoFrameTransformer;
    AudioFrameTransformer * mpAudioFrameTransformer;

    SyncClockManager mSyncClockManager;

    StateManager mStateManager;

    ActivateNodeManager * mpActivateNodeManager;

    DebugInfo * mpDebugInfo;

    void init_task();
public:
    CorePlayer();
    virtual ~CorePlayer();

    void set_render_surface(RenderView * render_view);

    void set_audio_device(AudioDevice * audio_device);

    //set 与 replace 系列函数的不同之处是会影响播放状态
    void set_play_item(PlayItem * play_item);

    void replace_play_item(const PlayItem * const play_item);

    PlayerStateEnum get_current_play_state();

    void set_player_state_change_listener(StateChangedListener * listener);

    void start();

    void pause();

    void resume();

    void stop();
    /**
     * @param position 单位毫秒
    */
    void seek(int64_t position);

    void init_states();

    /**
     * @return 返回目前视频长度，单位毫秒
    */
    int64_t get_duration();

    /**
     * @return 返回目前视频播放的进度，单位毫秒
    */
    int64_t get_current_position();

    /**
     * @return 返回debug信息
    */
    const DebugInfo * get_debug_info();
};
#endif