#include "CorePlayer.h"

#include "../render/video/VideoFrameTransformer.h"
#include "../render/video/TransformNode.h"
CorePlayer::CorePlayer(/* args */)
: pMediaDecoder(NULL),
pVideoRender(NULL),
pRenderView(NULL),
pInputStream(NULL),
mVideoFrameTransformer()
{
}

CorePlayer::~CorePlayer()
{
}

void CorePlayer::set_play_item(PlayItem * play_item)
{
    if (pCurrentPlayItem == NULL && play_item->get_data_source().size() > 0)
    {
        pCurrentPlayItem = play_item;
    } else {

    }
}

void CorePlayer::replace_play_item(const PlayItem * const play_item)
{
    
}

void CorePlayer::set_render_surface(IRenderView * render_view)
{
    pRenderView = render_view;
}

void CorePlayer::main_loop_thread_func(void *self)
{
    static_cast<CorePlayer *>(self)->main_loop();
}

void CorePlayer::main_loop()
{
    pInputStream = new MediaInputStream();
    pInputStream->open(pCurrentPlayItem->get_data_source());
    pMediaDecoder = new MediaDecoder(pInputStream->get_stream_iterator(), 
        pInputStream->get_packet_reader());
    pMediaDecoder->start();

    pthread_create(&mVideoRenderThreadId, NULL, video_render_thread_func, (void*)this);

    pthread_create(&mAudioRenderThreadId, NULL, audio_render_thread_func, (void*)this);

    pthread_create(&mVideoTransformThreadId, NULL, video_frame_transform_thread_fun, (void*)this);

    pthread_create(&mAudioTransformThreadId, NULL, audio_frame_transform_thread_fun, (void*)this);
}

void CorePlayer::video_render_thread_func(void *self)
{
    static_cast<CorePlayer *>(self)->video_render_loop();
}

void CorePlayer::video_render_loop()
{
    //TODO 受状态控制
    while (true)
    {
        //video 
        TransformNode * node = mVideoFrameTransformer.pop_transformed_node();
        
    }
}

void CorePlayer::audio_render_thread_func(void *self)
{
    static_cast<CorePlayer *>(self)->audio_render_loop();
}

void CorePlayer::audio_render_loop()
{
    //TODO 受状态控制
    while (true)
    {
        //audio
        const AVFrame * const audio_frame = pMediaDecoder->pop_frame(AVMEDIA_TYPE_AUDIO);
    }
}

void CorePlayer::video_frame_transform_thread_fun(void *self)
{
    static_cast<CorePlayer *>(self)->video_frame_transform_loop();
}
void CorePlayer::video_frame_transform_loop()
{
    //TODO 受状态控制
    while (true)
    {
        //video 
        const AVFrame * const video_frame = pMediaDecoder->pop_frame(AVMEDIA_TYPE_VIDEO);
        

    }
}

void CorePlayer::audio_frame_transform_thread_fun(void *self)
{
    static_cast<CorePlayer *>(self)->audio_frame_transform_loop();
}

void CorePlayer::audio_frame_transform_loop()
{

}


void CorePlayer::start()
{
    if (pRenderView == NULL || pCurrentPlayItem == NULL)
    {
        //TODO throw exception
    } else {

        pthread_create(&mInitThreadId, NULL, main_loop_thread_func, (void*)this);
    }
}

void CorePlayer::pause()
{

}

void CorePlayer::resume()
{

}

void CorePlayer::stop()
{

}