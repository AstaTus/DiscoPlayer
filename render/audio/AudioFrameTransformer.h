#ifndef __DISCO_AUDIO_FRAME_TRANSFORMER_H__
#define __DISCO_AUDIO_FRAME_TRANSFORMER_H__

#include "AudioTransformNode.h"
#include "AudioClip.h"
#include "AudioTransformProcessor.h"

#include "../../common/structure/ConcurrentQueue.h"
#include "../../common/cache/ConcurrentCachePool.h"
#include "../../common/thread/Semaphore.h"

#include <future>
#include <atomic>

class FrameReader;
using AudioTransformNodeCachePool = ConcurrentCachePool<AudioTransformNode>;
using AudioClipCachePool = ConcurrentCachePool<AudioClip>;
using AudioTransformNodeQueue = ConcurrentQueue<AudioTransformNode>;

class AudioFrameTransformer
{
private:
    static const int AUDIO_CLIP_CACHE_POOL_SIZE = 5;

    AudioTransformNodeQueue mTransformNodeQueue;
    AudioClipCachePool mClipCachePool;
    AudioTransformNodeCachePool mTransformNodeCachePool;
    AudioTransformProcessor mTransformProcessor;

    std::future<void> mAudioTransformFuture;
    Semaphore mTransformSemaphore;
    std::atomic_bool mIsTransformTaskPause;
    std::atomic_bool mIsTransformTaskStop;

    FrameReader * const mpFrameReader;

    void audio_frame_transform_loop_task();

    void push_frame_to_transform(FrameWrapper * frame);

public:
    AudioFrameTransformer(FrameReader * frame_reader);
    ~AudioFrameTransformer();

    AudioTransformNode * non_block_pop_transformed_node();
    AudioTransformNode * non_block_peek_transformed_node();
    void recycle(AudioTransformNode * transform_node);

    
    //TODO 宽高改变， 重建缓存中的images
    void retransform_cache_audio_clips();

    void start();

    void stop();

    void pause();

    void resume();

    void clear_buffer();
};
#endif


