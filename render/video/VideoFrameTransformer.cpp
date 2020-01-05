#include "VideoFrameTransformer.h"
#include "../../codec/FrameWrapper.h"

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
    TransformNode * node = mTransformNodeCachePool.get_empty_node();
    node->image = image;
    node->frame_wrapper = frame_wrapper;
    mTransformNodeQueue.push_node(node);
}

TransformNode * VideoFrameTransformer::non_block_pop_transformed_node()
{
    TransformNode * node = mTransformNodeQueue.non_block_pop_node();
    if (node != nullptr)
    {
        av_log(nullptr, AV_LOG_DEBUG, "[Disco][VideoFrameTransformer] non_block_pop_transformed_node pop node\n");
    }
    
    return node;
}

TransformNode * VideoFrameTransformer::non_block_peek_transformed_node()
{
    return mTransformNodeQueue.non_block_peek_node();
}

void VideoFrameTransformer::recycle(TransformNode * transform_node)
{
    mImageCachePool.recycle_node(transform_node->image);
    mTransformNodeCachePool.recycle_node(transform_node);
}

void VideoFrameTransformer::retransform_cache_images()
{
    
}