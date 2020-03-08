#pragma once

#include "AudioDeviceBufferProcessor.h"

namespace soundtouch {
	class SoundTouch;
}

class WavOutFile;
class WavInFile;
class SoundTouchDeviceBufferProcessor : public AudioDeviceBufferProcessor
{
private:
    soundtouch::SoundTouch * mpSoundTouch;
    short *mpSoundBuffer;
    unsigned int mSoundBufferSize;

    uint8_t * mOutputBuffer;
    uint32_t mOutputArraySize;
    uint32_t mOutputWritePos; 

    uint32_t mSoundTouchPipeCacheSize;

    uint32_t mBytesPerSample;

    WavOutFile *mpWavFile;
    WavInFile * mpInWavFile;

    virtual uint32_t process_buffer(uint8_t * buffer, long buffer_size, 
        int sample_rate, int sample_size, int channel_size, float speed, 
        int bytes_per_sample, enum AVSampleFormat out_format, int volume, uint8_t ** out_buffer) override;

    virtual void clear_buffer() override;

    //当前BufferProcessor缓冲区的buffsize
    virtual uint32_t get_cache_buffer_size() override;

    void buffer_convert_to_sound_touch_buffer();

    virtual bool is_need_process(uint8_t * buffer, long buffer_size, 
        int sample_rate, int sample_size, int channel_size, float speed, 
        int bytes_per_sample, enum AVSampleFormat out_format, int volume);

public:
    SoundTouchDeviceBufferProcessor(/* args */);
    ~SoundTouchDeviceBufferProcessor();
};


