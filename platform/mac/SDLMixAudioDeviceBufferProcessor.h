#pragma once
#include "../../render/audio/AudioDeviceLastBufferProcessor.h"

class SDLMixAudioDeviceBufferProcessor : public AudioDeviceLastBufferProcessor
{
private:
    virtual uint32_t process_buffer(uint8_t * buffer, long buffer_size, 
        int sample_rate, int sample_size, int channel_size, float speed, 
        int bytes_per_sample, enum AVSampleFormat out_format, int volume) override;
public:
    SDLMixAudioDeviceBufferProcessor(/* args */);
    ~SDLMixAudioDeviceBufferProcessor();

    virtual bool fill_buffer_by_cache(int volume) override;


};
