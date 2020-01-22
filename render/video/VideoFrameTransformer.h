#ifndef __VIDEO_FRAME_TRANSFORMER_H__
#define __VIDEO_FRAME_TRANSFORMER_H__

#include "VideoTransformProcessor.h"
#include "../../common/structure/ConcurrentQueue.h"
#include "../../common/cache/ConcurrentCachePool.h"
#include "VideoTransformNode.h"
#include "Image.h"

using TransformNodeCachePool = ConcurrentCachePool<VideoTransformNode>;
using ImageCachePool = ConcurrentCachePool<Image>;
using TransformNodeQueue = ConcurrentQueue<VideoTransformNode>;

class VideoFrameTransformer
{
private:
    static const int IMAGE_CACHE_POOL_SIZE = 5;

    TransformNodeQueue mTransformNodeQueue;
    ImageCachePool mImageCachePool;
    TransformNodeCachePool mTransformNodeCachePool;
    VideoTransformProcessor mTransformPorcessor;

public:
    VideoFrameTransformer(/* args */);
    ~VideoFrameTransformer();

    void push_frame_to_transform(FrameWrapper * frame, int width, int height);

    VideoTransformNode * non_block_pop_transformed_node();
    VideoTransformNode * non_block_peek_transformed_node();
    void recycle(VideoTransformNode * transform_node);

    //TODO 宽高改变， 重建缓存中的images
    void retransform_cache_images();
};
#endif