#pragma once

extern "C"
{
#include "libavutil/samplefmt.h"
}

class AudioDeviceBufferProcessor
{
protected:
    AudioDeviceBufferProcessor* mpChildProcessor;

    virtual uint32_t process_buffer(uint8_t * buffer, long buffer_size, 
        int sample_rate, int sample_size, int channel_size, float speed, 
        int bytes_per_sample, enum AVSampleFormat out_format, int volume) = 0;

    virtual void clear_buffer() = 0;

    //当前BufferProcessor缓冲区的buffsize
    virtual uint32_t get_cache_buffer_size() = 0;
public:
    AudioDeviceBufferProcessor(/* args */);
    ~AudioDeviceBufferProcessor();

    void push_back_child_processor(AudioDeviceBufferProcessor* child_processor);

    long delivery_process_buffer(uint8_t * buffer, long buffer_size, 
        int sample_rate, int sample_size, int channel_size, float speed, 
        int bytes_per_sample, enum AVSampleFormat out_format, int volume);

    void delivery_clear_buffer();

    uint32_t get_delivery_cache_buffer_size();
};