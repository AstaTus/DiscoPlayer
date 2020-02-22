#ifndef __DISCO_AUDIO_DEVICE_H__
#define __DISCO_AUDIO_DEVICE_H__

#include "AudioClip.h"
#include "AudioDataRequestListener.h"

extern "C"
{
#include "libavformat/avformat.h"
};


class AudioDevice 
{
protected:
    static const int DEFULT_AUDIO_VOLUME;

    AudioDataRequestListener * pAudioDataRequestListener;
    int mAudioVolume;
    int mSampleRate;
    AVSampleFormat mSampleFormat;
    int mChannelNum;
public:
    static const int MAX_AUDIO_VOLUME;

    //TODO  增加提供format 接口 channel数 给子类调用
    AudioDevice();
    virtual ~AudioDevice();
    virtual bool stop() = 0;

    virtual bool start() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;

    virtual int get_bytes_per_second() = 0;
    virtual double get_latency_seconds() = 0;

    void set_volume(int volume);

    int get_volume();

    void set_audio_play_param(int sample_rate, AVSampleFormat sample_format, int channel_num);

    void set_audio_data_request_listener(AudioDataRequestListener * audio_data_request_listener);
};
#endif