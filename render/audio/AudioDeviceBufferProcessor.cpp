#include "AudioDeviceBufferProcessor.h"

AudioDeviceBufferProcessor::AudioDeviceBufferProcessor(/* args */)
    : mpChildProcessor(nullptr)
{
}
AudioDeviceBufferProcessor::~AudioDeviceBufferProcessor()
{
}

void AudioDeviceBufferProcessor::push_back_child_processor(AudioDeviceBufferProcessor *child_processor)
{
    if (mpChildProcessor == nullptr)
    {
        mpChildProcessor = child_processor;
    } else {
        mpChildProcessor->push_back_child_processor(child_processor);
    }
}

void AudioDeviceBufferProcessor::delivery_clear_buffer()
{
    clear_buffer();
    if (mpChildProcessor != nullptr)
    {
        mpChildProcessor->clear_buffer();
    }
}

long AudioDeviceBufferProcessor::delivery_process_buffer(uint8_t * buffer, long buffer_size, 
        int sample_rate, int sample_size, int channel_size, float speed, 
        int bytes_per_sample, enum AVSampleFormat out_format, int volume)
{
    uint32_t size = buffer_size;
    uint8_t * out_buffer = buffer;
    if (is_need_process(buffer, buffer_size, sample_rate, sample_size, channel_size, 
        speed, bytes_per_sample, out_format, volume))
    {
        size = process_buffer(buffer, buffer_size, sample_rate, sample_size, channel_size, 
            speed, bytes_per_sample, out_format, volume, &out_buffer);
    }
    
    if (mpChildProcessor != nullptr)
    {
        return mpChildProcessor->delivery_process_buffer(out_buffer, size, 
            sample_rate, sample_size, channel_size, speed, bytes_per_sample, out_format, volume);
    }
    return size;
}

uint32_t AudioDeviceBufferProcessor::get_delivery_cache_buffer_size()
{
    if(mpChildProcessor == nullptr) {
        return get_cache_buffer_size();
    } else {
        return get_cache_buffer_size() + mpChildProcessor->get_delivery_cache_buffer_size();
    }
}