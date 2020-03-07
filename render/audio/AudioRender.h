#ifndef __DISCO_AUDIO_RENDER_H__
#define __DISCO_AUDIO_RENDER_H__

#include "AudioFillBufferListener.h"
#include "AudioDeviceBuffer.h"
#include <stdint.h>

extern "C"
{
#include "libavformat/avformat.h"
};

class AudioDevice;
class AudioClip;
class AudioDeviceBuffer;
class AudioDeviceLastBufferProcessor;

class AudioRender: public AudioFillBufferListener 
{
private:
    static const int DEFULT_AUDIO_VOLUME;
    //声音设备
    AudioDevice * const mpAudioDevice;
    AudioDeviceBuffer mAudioDeviceBuffer;

    float mSpeed;
    int mAudioVolume;

    int mSampleRate;
    AVSampleFormat mOutFormat;
    int mChannelNum;
public:
    static const int MAX_AUDIO_VOLUME;
    static const float DEFULT_AUDIO_SPEED;
    AudioRender(AudioDevice * const audio_device);
    ~AudioRender();

    void on_fill_audio_buffer(uint8_t * buffer, long len) override;

    void set_audio_play_param(int sample_rate, AVSampleFormat sample_format, int channel_num);

    void set_audio_data_request_listener(AudioDataRequestListener * audio_data_request_listener);

    void set_last_buffer_processor(AudioDeviceLastBufferProcessor * audio_last_buffer_processor);

    void set_volume(int volume);

    int get_volume();

    void set_speed(float speed);

    float get_speed();

    bool stop();

    bool start();

    bool pause();

    bool resume();

    uint32_t get_cache_buffer_size();
};
#endif