#ifndef __CORE_PLAYER_H__
#define __CORE_PLAYER_H__

#include <pthread.h>

#include "PlayItem.h"
#include "../stream/MediaInputStream.h"
#include "../codec/MediaDecoder.h"
#include "../render/video/VideoRender.h"
#include "../render/video/RenderView.h"
#include "../render/video/VideoFrameTransformer.h"
#include "../clock/SyncClockManager.h"
class CorePlayer
{
private:
    MediaDecoder * pMediaDecoder;
    VideoRender * pVideoRender;

    RenderView * pRenderView;
    MediaInputStream * pInputStream;

    PlayItem * pCurrentPlayItem;

    pthread_t mInitThreadId;
    pthread_t mVideoRenderThreadId;
    pthread_t mAudioRenderThreadId;
    pthread_t mVideoTransformThreadId;
    pthread_t mAudioTransformThreadId;

    VideoFrameTransformer mVideoFrameTransformer;
    TransformNode * pCurrentVideoNode;

    SyncClockManager mSyncClockManager;

    static void main_loop_thread_func(void *self);
    
    
    static void video_render_thread_func(void *self);
    void video_render_loop();

    static void audio_render_thread_func(void *self);
    void audio_render_loop();

    static void video_frame_transform_thread_fun(void *self);
    void video_frame_transform_loop();

    static void audio_frame_transform_thread_fun(void *self);
    void audio_frame_transform_loop();

public:
    CorePlayer(/* args */);
    ~CorePlayer();

    void set_render_surface(IRenderView * render_view);

    //set 与 replace 系列函数的不同之处是会影响播放状态
    void set_play_item(PlayItem * play_item);

    void replace_play_item(const PlayItem * const play_item);

    void start();

    void pause();

    void resume();

    void stop();
};
#endif