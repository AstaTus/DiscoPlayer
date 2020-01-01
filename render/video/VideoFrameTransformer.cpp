#include "VideoFrameTransformer.h"

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

void VideoFrameTransformer::push_frame_to_transform(AVFrame * frame, int width, int height)
{
    Image * image = mImageCachePool.get_empty_node();
    mTransformPorcessor.transform_by_libyuv(frame, image, width, height);
    TransformNode * node = mTransformNodeCachePool.get_empty_node();
    node->image = image;
    node->frame = frame;
    mTransformNodeQueue.push_node(node);
}

TransformNode * VideoFrameTransformer::non_block_pop_transformed_node()
{
    return mTransformNodeQueue.non_block_pop_node();
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