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

CorePlayer::CorePlayer()
: pMediaDecoder(nullptr),
pVideoRender(nullptr),
pRenderView(nullptr),
pInputStream(nullptr),
mVideoFrameTransformer(),
pCurrentVideoNode(nullptr),
pCurrentPlayItem(nullptr),
mSyncClockManager(SyncClockManager::SYNC_STRATEGY_VIDEO)
{
}

CorePlayer::~CorePlayer()
{
}

void CorePlayer::set_play_item(PlayItem * play_item)
{
    if (pCurrentPlayItem == nullptr && play_item->get_data_source().size() > 0)
    {
        pCurrentPlayItem = play_item;
    } else {

    }
}

void CorePlayer::replace_play_item(const PlayItem * const play_item)
{
    
}

void CorePlayer::set_render_surface(RenderView * render_view)
{
    pRenderView = render_view;
    pVideoRender = new VideoRender(pRenderView);
}

void CorePlayer::init_task()
{
    pInputStream = new MediaInputStream();
    pInputStream->open(pCurrentPlayItem->get_data_source());
    pMediaDecoder = new MediaDecoder(pInputStream->get_stream_iterator(), 
        pInputStream->get_packet_reader());
    pMediaDecoder->start();

    mVideoRenderFuture = std::async(std::launch::async, &CorePlayer::video_render_loop_task, this);
    mAudioRenderFuture = std::async(std::launch::async, &CorePlayer::audio_render_loop_task, this);
    mVideoTransformFuture = std::async(std::launch::async, &CorePlayer::video_frame_transform_loop_task, this);
    mAudioTransformFuture = std::async(std::launch::async, &CorePlayer::audio_frame_transform_loop_task, this);
}


void CorePlayer::video_frame_transform_loop_task()
{
    //TODO 受状态控制
    while (true)
    {
        //video 
        AVFrame * video_frame = pMediaDecoder->pop_frame(AVMEDIA_TYPE_VIDEO);
        mVideoFrameTransformer.push_frame_to_transform(video_frame);
    }
}

void CorePlayer::video_render_loop_task()
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

void CorePlayer::audio_render_loop_task()
{
    //TODO 受状态控制
    while (true)
    {
        //audio
        const AVFrame * const audio_frame = pMediaDecoder->pop_frame(AVMEDIA_TYPE_AUDIO);
    }
}

void CorePlayer::audio_frame_transform_loop_task()
{

}


void CorePlayer::start()
{
    if (pVideoRender == NULL || pCurrentPlayItem == NULL)
    {
        //TODO throw exception
    } else {
        mInitFuture = std::async(std::launch::async, &CorePlayer::init_task, this);
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