#ifndef __CORE_PLAYER_H__
#define __CORE_PLAYER_H__

#include "PlayItem.h"
#include "../stream/MediaInputStream.h"
#include "../codec/MediaDecoder.h"

#include "../render/video/VideoRender.h"
#include "../render/video/RenderView.h"
#include "../render/video/VideoFrameTransformer.h"

#include "../render/audio/AudioRender.h"
#include "../render/audio/AudioDevice.h"
#include "../render/audio/AudioFrameTransformer.h"

#include "../clock/SyncClockManager.h"

#include <future>
class CorePlayer : public AudioDataRequestListener
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
    std::future<void> mAudioRenderFuture;
    std::future<void> mVideoTransformFuture;
    std::future<void> mAudioTransformFuture;

    std::atomic<bool> mIsStop;

    VideoFrameTransformer mVideoFrameTransformer;
    VideoTransformNode * pCurrentVideoNode;

    AudioFrameTransformer mAudioFrameTransformer;
    AudioTransformNode * pCurrentAudioNode;

    SyncClockManager mSyncClockManager;

    void init_task();
    void video_render_loop_task();
    void video_frame_transform_loop_task();
    void audio_frame_transform_loop_task();

    void on_audio_data_request_begin() override;
    void on_audio_data_request_end() override;
    AudioClip * const on_audio_data_request(int len) override;

public:
    CorePlayer();
    virtual ~CorePlayer();

    void set_render_surface(RenderView * render_view);

    void set_audio_device(AudioDevice * audio_device);

    //set 与 replace 系列函数的不同之处是会影响播放状态
    void set_play_item(PlayItem * play_item);

    void replace_play_item(const PlayItem * const play_item);

    void start();

    void pause();

    void resume();

    void stop();
};
#endif