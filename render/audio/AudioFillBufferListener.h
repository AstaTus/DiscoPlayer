#pragma once
#include <stdint.h>
class AudioFillBufferListener
{
private:
    /* data */
public:
    AudioFillBufferListener(/* args */) = default;
    ~AudioFillBufferListener() = default;

    virtual void on_fill_audio_buffer(uint8_t * buffer, long len) = 0;
};