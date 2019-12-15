#include "CorePlayer.h"

#include "../render/video/VideoFrameTransformer.h"
#include "../render/video/TransformNode.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	#include "libavutil/time.h"
}

#define REFRESH_RATE 0.01

CorePlayer::CorePlayer(/* args */)
: pMediaDecoder(NULL),
pVideoRender(NULL),
pRenderView(NULL),
pInputStream(NULL),
mVideoFrameTransformer(),
pCurrentVideoNode(NULL),
mSyncClockManager(SyncClockManager::SYNC_STRATEGY_VIDEO)
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
    pVideoRender = new VideoRender(pRenderView);
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
        mVideoFrameTransformer->push_frame_to_transform(video_frame)
    }
}

void CorePlayer::video_render_thread_func(void *self)
{
    static_cast<CorePlayer *>(self)->video_render_loop();
}

void CorePlayer::video_render_loop()
{
    SyncClockManager::SyncState sync_state;
    double remaining_time = 0.0;
    //TODO 受状态控制
    while (true)
    {
        if(remaining_time > 0.0) {
            av_usleep((int)(int64_t)(remaining_time * 1000000.0));
        }
        
        remaining_time = REFRESH_RATE;
        //TODO sleep
        do {
            TransformNode * node = mVideoFrameTransformer.non_block_peek_transformed_node();
            if (node == NULL)
            {
                break;
            }
             
            sync_state = mSyncClockManager.get_current_video_sync_state(node->frame->pts, &remaining_time);
            if (sync_state == SyncClockManager::SYNC_STATE_KEEP)
            {
                //显示当前帧
                pVideoRender->refresh(pCurrentVideoNode->image);
            } else {

                mVideoFrameTransformer.non_block_pop_transformed_node();
                //回收上一帧
                if (pCurrentVideoNode != NULL)
                {
                    mVideoFrameTransformer.recycle(pCurrentVideoNode);
                    pMediaDecoder->recycle_frame(AVMEDIA_TYPE_VIDEO, pCurrentVideoNode->frame);
                }

                if (sync_state == SyncClockManager::SYNC_STATE_NEXT)
                {
                    //显示下一帧
                    pCurrentVideoNode = node;
                    pVideoRender->refresh(pCurrentVideoNode->image);
                }
            }
        } while(sync_state == SyncClockManager::SYNC_STATE_DROP);
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

void CorePlayer::audio_frame_transform_thread_fun(void *self)
{
    static_cast<CorePlayer *>(self)->audio_frame_transform_loop();
}

void CorePlayer::audio_frame_transform_loop()
{

}


void CorePlayer::start()
{
    if (pVideoRender == NULL || pCurrentPlayItem == NULL)
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