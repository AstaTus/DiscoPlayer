#ifndef __VIDEO_FRAME_TRANSFORMER_H__
#define __VIDEO_FRAME_TRANSFORMER_H__

#include "VideoTransformProcessor.h"
#include "../../common/structure/ConcurrentQueue.h"
#include "../../common/cache/ConcurrentCachePool.h"
#include "VideoTransformNode.h"
#include "Image.h"

#include "../../common/thread/Semaphore.h"

#include <future>
#include <atomic>

class FrameReader;
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

    std::future<void> mVideoTransformFuture;
    Semaphore mTransformSemaphore;
    Semaphore mClearBufferSemaphore;
    std::atomic_bool mIsClearBufferAndPause;
    std::atomic_bool mIsTransformTaskStop;

    FrameReader *const mpFrameReader;
    int mRenderViewHeight;
    int mRenderViewWidth;

    void video_frame_transform_loop_task();
    void push_frame_to_transform(FrameWrapper *frame, int width, int height);

    void clear_frame_buffer();

    void clear_buffer();
public:
    VideoFrameTransformer(FrameReader *frame_reader);
    ~VideoFrameTransformer();

    
    VideoTransformNode *non_block_pop_transformed_node();
    VideoTransformNode *non_block_peek_transformed_node();

    VideoTransformNode *block_peek_transformed_node();
    VideoTransformNode *block_pop_transformed_node();
    void recycle(VideoTransformNode *transform_node);

    //TODO 宽高改变， 重建缓存中的images
    void retransform_cache_images();

    void start();

    void stop();

    bool clear_buffer_and_pause();

    void resume();

    void on_resize_render_view(int width, int height);
};
#endif