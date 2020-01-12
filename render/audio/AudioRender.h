#ifndef __DISCO_AUDIO_RENDER_H__
#define __DISCO_AUDIO_RENDER_H__

class AudioDevice;
class AudioClip;

class AudioRender {

private:
    //声音设备
    AudioDevice * const pAudioDevice;

public:
    AudioRender(AudioDevice * const audio_device);
    ~AudioRender();
};
#endif