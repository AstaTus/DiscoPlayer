#include "AudioDeviceLastBufferProcessor.h"

const int AudioDeviceLastBufferProcessor::CACHE_INCREACE_SIZE = 1024;

AudioDeviceLastBufferProcessor::AudioDeviceLastBufferProcessor()
:mpTargetBuffer(nullptr),
mTargetBufferSize(0),
mTargetBufferValidSize(0),
mpCacheBuffer(nullptr),
mCacheBufferSize(0),
mCacheReadedSize(0),
mCacheArrayLen(0)
{

}

void AudioDeviceLastBufferProcessor::add_cache_read_size(uint32_t size)
{
    if (mCacheBufferSize < mCacheReadedSize + size)
    {
        //TODO exception
        return;
    }

    mCacheReadedSize += size;
}

uint32_t AudioDeviceLastBufferProcessor::get_cache_buffer_size()
{
    return mCacheBufferSize - mCacheReadedSize;
}

uint8_t * AudioDeviceLastBufferProcessor::get_cache_buffer()
{
    return (mpCacheBuffer + mCacheReadedSize);
}

void AudioDeviceLastBufferProcessor::clear_buffer()
{
    mCacheReadedSize = 0;
    mCacheBufferSize = 0;
}

void AudioDeviceLastBufferProcessor::set_cache_buffer(uint8_t * buff, uint32_t size)
{
    if (!is_cache_buffer_empty()){
        //TODO exception
        return;
    }

    if (mCacheArrayLen < size)
    {
        av_fast_malloc(&mpCacheBuffer, &mCacheArrayLen, size + CACHE_INCREACE_SIZE);
    }

    mCacheReadedSize = 0;
    mCacheBufferSize = size;
    
    memcpy(mpCacheBuffer, buff, size * sizeof(uint8_t));
}

bool AudioDeviceLastBufferProcessor::is_cache_buffer_empty()
{
    if (mCacheBufferSize == mCacheReadedSize)
    {
        return true;
    }

    return false;
}

void AudioDeviceLastBufferProcessor::set_target_buffer(uint8_t * buffer, uint32_t size)
{
    mpTargetBuffer = buffer;
    mTargetBufferSize = size;
    mTargetBufferValidSize = 0;
    memset(mpTargetBuffer, 0, mTargetBufferSize);
}

bool AudioDeviceLastBufferProcessor::is_target_buffer_full()
{
    return mTargetBufferSize == mTargetBufferValidSize;
}

bool AudioDeviceLastBufferProcessor::is_need_process(uint8_t * buffer, long buffer_size, 
    int sample_rate, int sample_size, int channel_size, float speed, 
    int bytes_per_sample, enum AVSampleFormat out_format, int volume)
{
    return true;
}