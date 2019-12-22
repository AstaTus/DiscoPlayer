#ifndef __VIDEO_FRAME_TRANSFORMER_H__
#define __VIDEO_FRAME_TRANSFORMER_H__

extern "C"
{
    #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

#include "TransformPorcessor.h"


#include "../../common/structure/ConcurrentQueue.h"
#include "../../common/cache/ConcurrentCachePool.h"
#include "TransformNode.h"
#include "Image.h"

using TransformNodeCachePool = ConcurrentCachePool<TransformNode>;
using ImageCachePool = ConcurrentCachePool<Image>;
using TransformNodeQueue = ConcurrentQueue<TransformNode>;

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

    TransformNode * non_block_pop_transformed_node();
    TransformNode * non_block_peek_transformed_node();
    void recycle(TransformNode * transform_node);


    /**调整 视频目标宽高
     * @param width 目标宽度
     * @param height 目标高度
    */
    void resize_target(int width, int height);
};
#endif