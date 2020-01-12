#ifndef __DISCO_AUDIO_FRAME_TRANSFORMER_H__
#define __DISCO_AUDIO_FRAME_TRANSFORMER_H__

#include "AudioTransformNode.h"
#include "AudioClip.h"
#include "AudioTransformProcessor.h"

#include "../../common/structure/ConcurrentQueue.h"
#include "../../common/cache/ConcurrentCachePool.h"

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

public:
    AudioFrameTransformer(/* args */);
    ~AudioFrameTransformer();

    void push_frame_to_transform(FrameWrapper * frame);

    AudioTransformNode * non_block_pop_transformed_node();
    AudioTransformNode * non_block_peek_transformed_node();
    void recycle(AudioTransformNode * transform_node);

    //TODO 宽高改变， 重建缓存中的images
    void retransform_cache_audio_clips();
};
#endif


