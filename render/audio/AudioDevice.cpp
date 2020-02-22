#include "AudioDevice.h"


const int AudioDevice::DEFULT_AUDIO_VOLUME = 64;
const int AudioDevice::MAX_AUDIO_VOLUME = 100;

AudioDevice::AudioDevice()
: pAudioDataRequestListener(nullptr),
mAudioVolume(DEFULT_AUDIO_VOLUME),
mSampleRate(0),
mSampleFormat(AVSampleFormat::AV_SAMPLE_FMT_NONE),
mChannelNum(0)
{
}

AudioDevice::~AudioDevice() 
{
    
}

void AudioDevice::set_volume(int volume) 
{
    if (volume >= 0 && volume <= MAX_AUDIO_VOLUME)
    {
        mAudioVolume = volume;
    }
}

int AudioDevice::get_volume() {
    return mAudioVolume;
}

void AudioDevice::set_audio_data_request_listener(AudioDataRequestListener * audio_data_request_listener)
{
    pAudioDataRequestListener = audio_data_request_listener;
}

void AudioDevice::set_audio_play_param(int sample_rate, AVSampleFormat sample_format, int channel_num)
{
    mSampleRate = sample_rate;
    mSampleFormat = sample_format;
    mChannelNum = channel_num;
}