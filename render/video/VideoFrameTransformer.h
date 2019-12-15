#ifndef __VIDEO_FRAME_TRANSFORMER_H__
#define __VIDEO_FRAME_TRANSFORMER_H__

class TransformNodeQueue;
class TransformNode;
class AVFrame;
class Image;
class ImageCachePool;
class TransformNodeCachePool;

#include "TransformPorcessor.h"

class VideoFrameTransformer
{
private:
    static const int IMAGE_CACHE_POOL_SIZE = 5;

    TransformNodeQueue mTransformNodeQueue;
    ImageCachePool mImageCachePool;
    TransformNodeCachePool mTransformNodeCachePool;
    TransformPorcessor mTransformPorcessor;

public:
    VideoFrameTransformer(/* args */);
    ~VideoFrameTransformer();

    void push_frame_to_transform(AVFrame * frame);

    const TransformNode * const non_block_pop_transformed_node();
    const TransformNode * const non_block_peek_transformed_node();
    void recycle(TransformNode * transform_node);

    /**调整 视频目标宽高
     * @param width 目标宽度
     * @param height 目标高度
    */
    void resize_target(int width, int height);
};
#endif