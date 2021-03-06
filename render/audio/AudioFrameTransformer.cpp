#include "AudioFrameTransformer.h"
#include "../../codec/FrameWrapper.h"
#include "../../codec/FrameReader.h"
#include "../../common/log/Log.h"
#include "../../common/thread/ThreadUtils.h"

AudioFrameTransformer::AudioFrameTransformer(FrameReader * frame_reader)
:mpFrameReader(frame_reader),
mTransformNodeQueue(),
mClipCachePool(AUDIO_CLIP_CACHE_POOL_SIZE),
mTransformNodeCachePool(AUDIO_CLIP_CACHE_POOL_SIZE),
mTransformProcessor(),
mTransformSemaphore(),
mIsClearBufferAndPause(false),
mIsTransformTaskStop(false)
{
}

AudioFrameTransformer::~AudioFrameTransformer()
{
    mAudioTransformFuture.wait();

    AudioTransformNode * audio_transform_node = mTransformNodeQueue.non_block_pop_node();
    while (audio_transform_node != nullptr)
    {
        recycle(audio_transform_node);
        audio_transform_node = mTransformNodeQueue.non_block_pop_node();
    }

    // mpFrameReader = nullptr;
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
        Log::get_instance().log_debug("[Disco][AudioFrameTransformer] non_block_pop_transformed_node pop node\n");
    }
    
    return node;
}
AudioTransformNode * AudioFrameTransformer::non_block_peek_transformed_node()
{
    return mTransformNodeQueue.non_block_peek_node();
}

AudioTransformNode *AudioFrameTransformer::block_peek_transformed_node()
{
    return mTransformNodeQueue.block_peek_node();
}

AudioTransformNode *AudioFrameTransformer::block_pop_transformed_node()
{
    return mTransformNodeQueue.block_pop_node();
}

void AudioFrameTransformer::recycle(AudioTransformNode * transform_node)
{
    if (transform_node != nullptr)
    {
        Log::get_instance().log_debug("Disco AudioFrameTransformer::recycle node\n");
        mpFrameReader->recycle_frame(transform_node->frame_wrapper);
        mClipCachePool.recycle_node(transform_node->clip);
        mTransformNodeCachePool.recycle_node(transform_node);
    }
}

void AudioFrameTransformer::start()
{
    
    mAudioTransformFuture = std::async(std::launch::async, &AudioFrameTransformer::audio_frame_transform_loop_task, this);
}

void AudioFrameTransformer::audio_frame_transform_loop_task()
{
    ThreadUtils::set_thread_name("Disco Audio Frame Transformer");
    while (!mIsTransformTaskStop)
    {
        FrameWrapper *audio_frame_wrapper = mpFrameReader->pop_frame();
        Log::get_instance().log_debug("[Disco][AudioFrameTransformer] audio_frame_transform_loop_task pop frame wrapper\n");
        //stream is end
        if (audio_frame_wrapper->is_end)
        {
            AudioClip * clip = mClipCachePool.get_empty_node();
            AudioTransformNode * node = mTransformNodeCachePool.get_empty_node();
            node->is_end = true;
            node->clip = clip;
            node->frame_wrapper = audio_frame_wrapper;
            mTransformNodeQueue.push_node(node);
            break;
        }

        if (audio_frame_wrapper->serial != mpFrameReader->serial() ||
            audio_frame_wrapper->get_transformed_pts() < mpFrameReader->serial_start_time())
        {
            mpFrameReader->recycle_frame(audio_frame_wrapper);
            Log::get_instance().log_debug("[Disco][AudioFrameTransformer] audio_frame_transform_loop_task discard a frame wrapper\n");
        } else 
        {
            push_frame_to_transform(audio_frame_wrapper);
            Log::get_instance().log_debug("[Disco][AudioFrameTransformer] audio_frame_transform_loop_task add a frame wrapper to transform\n");
        }
    }

    Log::get_instance().log_debug("[Disco][AudioFrameTransformer] audio_frame_transform_loop_task thread over\n");
}

void AudioFrameTransformer::stop()
{
    resume();
    mIsTransformTaskStop = true;
}

bool AudioFrameTransformer::clear_buffer_and_pause()
{
    if (!mIsClearBufferAndPause)
    {
        mIsClearBufferAndPause = true;
        mClearBufferSemaphore.wait();
        return true;
    } else {
        return false;
    }
}

void AudioFrameTransformer::resume()
{
    mTransformSemaphore.signal();
}

void AudioFrameTransformer::retransform_cache_audio_clips()
{

}

void AudioFrameTransformer::clear_buffer()
{
    AudioTransformNode * node = mTransformNodeQueue.non_block_pop_node();
    while (node != nullptr)
    {
        recycle(node);
        node = mTransformNodeQueue.non_block_pop_node();
    }
}

int AudioFrameTransformer::get_output_sample_rate()
{
    return mTransformProcessor.get_output_sample_rate();
}

AVSampleFormat AudioFrameTransformer::get_output_sample_format()
{
    return mTransformProcessor.get_output_sample_format();
}

int AudioFrameTransformer::get_output_channel_num()
{
    return mTransformProcessor.get_output_channel_num();
}
