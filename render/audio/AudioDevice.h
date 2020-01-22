#ifndef __DISCO_AUDIO_DEVICE_H__
#define __DISCO_AUDIO_DEVICE_H__

#include "AudioClip.h"
#include "AudioDataRequestListener.h"
class AudioDevice 
{
protected:
    AudioDataRequestListener * pAudioDataRequestListener;
public:
    //TODO  增加提供format 接口 channel数 给子类调用
    AudioDevice();
    virtual ~AudioDevice();
    virtual bool open() = 0;
    virtual bool close() = 0;

    void set_audio_data_request_listener(AudioDataRequestListener * audio_data_request_listener);
};
#endif