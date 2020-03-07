#ifndef __DISCO_AUDIO_DEVICE_H__
#define __DISCO_AUDIO_DEVICE_H__

#include "AudioClip.h"


extern "C"
{
#include "libavformat/avformat.h"
};

class AudioFillBufferListener;
class AudioDevice 
{
protected:
    int mSampleRate;
    AVSampleFormat mSampleFormat;
    int mChannelNum;

    AudioFillBufferListener * mpAudioFillBufferListener;
public:
    //TODO  增加提供format 接口 channel数 给子类调用
    AudioDevice();
    virtual ~AudioDevice();
    virtual bool stop() = 0;

    virtual bool start() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;

    virtual int get_bytes_per_second() = 0;
    virtual double get_latency_seconds() = 0;

    void set_audio_play_param(int sample_rate, AVSampleFormat sample_format, int channel_num);

    void set_audio_fill_buffer_listener(AudioFillBufferListener * listener);

    void fill_audio_buffer(u_int8_t *stream, int len);    
};
#endif