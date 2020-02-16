#include "AudioDevice.h"

AudioDevice::AudioDevice()
: pAudioDataRequestListener(nullptr),
mAudioVolume(64),
mSampleRate(0),
mSampleFormat(AVSampleFormat::AV_SAMPLE_FMT_NONE),
mChannelNum(0)
{
}

AudioDevice::~AudioDevice() {
    
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