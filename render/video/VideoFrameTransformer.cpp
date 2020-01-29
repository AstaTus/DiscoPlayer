#include "VideoFrameTransformer.h"
#include "../../codec/FrameWrapper.h"
#include "../../common/log/Log.h"
extern "C"
{
    #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}
VideoFrameTransformer::VideoFrameTransformer()
: mTransformNodeQueue(),
mTransformPorcessor(),
mImageCachePool(IMAGE_CACHE_POOL_SIZE),
mTransformNodeCachePool(IMAGE_CACHE_POOL_SIZE)
{
}

VideoFrameTransformer::~VideoFrameTransformer()
{
}

void VideoFrameTransformer::push_frame_to_transform(FrameWrapper * frame_wrapper, int width, int height)
{
    // Log::get_instance().log_debug("[Dsico][VideoFrameTransformer] add transform frame");
    Image * image = mImageCachePool.get_empty_node();
    mTransformPorcessor.transform_by_libyuv(frame_wrapper->frame, image, width, height);
    VideoTransformNode * node = mTransformNodeCachePool.get_empty_node();
    node->image = image;
    node->frame_wrapper = frame_wrapper;
    mTransformNodeQueue.push_node(node);
}

VideoTransformNode * VideoFrameTransformer::non_block_pop_transformed_node()
{
    VideoTransformNode * node = mTransformNodeQueue.non_block_pop_node();
    if (node != nullptr)
    {
        Log::get_instance().log_debug("[Disco][VideoFrameTransformer] non_block_pop_transformed_node pop node\n");
    }
    
    return node;
}

VideoTransformNode * VideoFrameTransformer::non_block_peek_transformed_node()
{
    return mTransformNodeQueue.non_block_peek_node();
}

void VideoFrameTransformer::recycle(VideoTransformNode * transform_node)
{
    mImageCachePool.recycle_node(transform_node->image);
    mTransformNodeCachePool.recycle_node(transform_node);
}

void VideoFrameTransformer::retransform_cache_images()
{
    
}