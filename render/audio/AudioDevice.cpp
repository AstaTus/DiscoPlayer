#include "AudioDevice.h"

AudioDevice::AudioDevice()
: pAudioDataRequestListener(nullptr)
{
}

AudioDevice::~AudioDevice() {
    
}

void AudioDevice::set_audio_data_request_listener(AudioDataRequestListener * audio_data_request_listener)
{
    pAudioDataRequestListener = audio_data_request_listener;
}