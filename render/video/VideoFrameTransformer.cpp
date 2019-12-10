#include "VideoFrameTransformer.h"
#include "TransformNodeQueue.h"
#include "Image.h"
#include "ImageCachePool.h"
#include "TransformNode.h"

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

void VideoFrameTransformer::push_frame_to_transform(AVFrame * frame)
{
    Image * image = mImageCachePool.get_empty_node();
    mTransformPorcessor.transform_by_libyuv(frame, image);
    TransformNode * node = mTransformNodeCachePool.get_empty_node();
    node->image = image;
    node->frame = frame;
    mTransformNodeQueue.push_node(node);
}

TransformNode * VideoFrameTransformer::pop_transformed_node()
{
    return mTransformNodeQueue->pop();
}
void VideoFrameTransformer::recycle(Image * image, TransformNode * transform_node)
{
    mImageCachePool.recycle_node(image);
    mTransformNodeCachePool.recycle_node(transform_node);
}

void VideoFrameTransformer::resize_target(int width, int height)
{
    mTransformPorcessor.resize_target(width, height);
}