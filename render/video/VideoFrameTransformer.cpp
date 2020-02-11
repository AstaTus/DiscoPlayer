#include "VideoFrameTransformer.h"
#include "../../codec/FrameWrapper.h"
#include "../../codec/FrameReader.h"
#include "../../common/log/Log.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
#include "../../common/thread/ThreadUtils.h"

VideoFrameTransformer::VideoFrameTransformer(FrameReader *frame_reader)
    : mpFrameReader(frame_reader),
      mTransformNodeQueue(),
      mImageCachePool(IMAGE_CACHE_POOL_SIZE),
      mTransformNodeCachePool(IMAGE_CACHE_POOL_SIZE),
      mTransformPorcessor(),
      mTransformSemaphore(),
      mIsClearBufferAndPause(false),
      mIsTransformTaskStop(false)
{
}

VideoFrameTransformer::~VideoFrameTransformer()
{
}

void VideoFrameTransformer::push_frame_to_transform(FrameWrapper *frame_wrapper, int width, int height)
{
    // Log::get_instance().log_debug("[Dsico][VideoFrameTransformer] add transform frame");
    Image *image = mImageCachePool.get_empty_node();
    mTransformPorcessor.transform_by_libyuv(frame_wrapper->frame, image, width, height);
    VideoTransformNode *node = mTransformNodeCachePool.get_empty_node();
    node->image = image;
    node->frame_wrapper = frame_wrapper;
    mTransformNodeQueue.push_node(node);
}

VideoTransformNode *VideoFrameTransformer::non_block_pop_transformed_node()
{
    VideoTransformNode *node = mTransformNodeQueue.non_block_pop_node();
    if (node != nullptr)
    {
        Log::get_instance().log_debug("[Disco][VideoFrameTransformer] non_block_pop_transformed_node pop node\n");
    }

    return node;
}

VideoTransformNode *VideoFrameTransformer::non_block_peek_transformed_node()
{
    return mTransformNodeQueue.non_block_peek_node();
}

VideoTransformNode *VideoFrameTransformer::block_peek_transformed_node()
{
    return mTransformNodeQueue.block_peek_node();
}

VideoTransformNode *VideoFrameTransformer::block_pop_transformed_node()
{
    return mTransformNodeQueue.block_pop_node();
}

void VideoFrameTransformer::recycle(VideoTransformNode *transform_node)
{
    mpFrameReader->recycle_frame(transform_node->frame_wrapper);
    mImageCachePool.recycle_node(transform_node->image);
    mTransformNodeCachePool.recycle_node(transform_node);
}

void VideoFrameTransformer::retransform_cache_images()
{
}

void VideoFrameTransformer::video_frame_transform_loop_task()
{
    ThreadUtils::set_thread_name("Disco Video Frame Transformer");
    //只要播放器不销毁，就一直循环
    while (!mIsTransformTaskStop)
    {
        // if (mIsClearBufferAndPause)
        // {
        //     clear_buffer();
        //     mClearBufferSemaphore.signal();
        //     mTransformSemaphore.wait();
        //     mIsClearBufferAndPause = true;
        // }

        //video
        FrameWrapper *video_frame_wrapper = mpFrameReader->pop_frame();
        if (video_frame_wrapper->serial != mpFrameReader->serial() ||
            video_frame_wrapper->frame->pts * 1000 * av_q2d(video_frame_wrapper->time_base) < mpFrameReader->serial_start_time())
        {
            mpFrameReader->recycle_frame(video_frame_wrapper);
        } else {
            push_frame_to_transform(video_frame_wrapper, mRenderViewWidth, mRenderViewHeight);
            Log::get_instance().log_debug("[Disco][CorePlayer] video_frame_transform_loop_task add frame to transform\n");
        }
        
    }

    Log::get_instance().log_debug("[Disco][CorePlayer] video_frame_transform_loop_task thread over\n");
}

void VideoFrameTransformer::start()
{
    mVideoTransformFuture = std::async(std::launch::async, &VideoFrameTransformer::video_frame_transform_loop_task, this);
}

void VideoFrameTransformer::stop()
{
    resume();
    mIsTransformTaskStop = true;
}

bool VideoFrameTransformer::clear_buffer_and_pause()
{
    if (!mIsClearBufferAndPause)
    {
        mIsClearBufferAndPause = true;
        mClearBufferSemaphore.wait();
        return true;
    } else {
        return false;
    }
}

void VideoFrameTransformer::resume()
{
    mTransformSemaphore.signal();
}

void VideoFrameTransformer::on_resize_render_view(int width, int height)
{
    mRenderViewWidth = width;
    mRenderViewHeight = height;
}

void VideoFrameTransformer::clear_frame_buffer()
{

}

void VideoFrameTransformer::clear_buffer()
{
    VideoTransformNode * node = mTransformNodeQueue.non_block_pop_node();
    while (node != nullptr)
    {
        recycle(node);
        node = mTransformNodeQueue.non_block_pop_node();
    }
}
