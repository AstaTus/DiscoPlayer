#include "AudioFrameTransformer.h"
#include "../../codec/FrameWrapper.h"

AudioFrameTransformer::AudioFrameTransformer(/* args */)
:mTransformNodeQueue(),
mClipCachePool(AUDIO_CLIP_CACHE_POOL_SIZE),
mTransformNodeCachePool(AUDIO_CLIP_CACHE_POOL_SIZE),
mTransformProcessor()
{
}

AudioFrameTransformer::~AudioFrameTransformer()
{
}

void AudioFrameTransformer::push_frame_to_transform(FrameWrapper * frame_wrapper)
{
    // Log::get_instance().log_debug("[Dsico][VideoFrameTransformer] add transform frame");
    AudioClip * clip = mClipCachePool.get_empty_node();
    mTransformProcessor.transform_by_ffmpeg(frame_wrapper->frame, clip);
    AudioTransformNode * node = mTransformNodeCachePool.get_empty_node();
    node->clip = clip;
    node->frame_wrapper = frame_wrapper;
    mTransformNodeQueue.push_node(node);
}

AudioTransformNode * AudioFrameTransformer::non_block_pop_transformed_node()
{
    AudioTransformNode * node = mTransformNodeQueue.non_block_pop_node();
    if (node != nullptr)
    {
        av_log(nullptr, AV_LOG_DEBUG, "[Disco][AudioFrameTransformer] non_block_pop_transformed_node pop node\n");
    }
    
    return node;
}
AudioTransformNode * AudioFrameTransformer::non_block_peek_transformed_node()
{
    return mTransformNodeQueue.non_block_peek_node();
}
void AudioFrameTransformer::recycle(AudioTransformNode * transform_node)
{
    mClipCachePool.recycle_node(transform_node->clip);
    mTransformNodeCachePool.recycle_node(transform_node);
}

void AudioFrameTransformer::retransform_cache_audio_clips()
{

}

