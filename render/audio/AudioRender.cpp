#include "AudioRender.h"
#include "AudioClip.h"
#include "AudioDevice.h"

AudioRender::AudioRender(AudioDevice *const audio_device)
    : pAudioDevice(audio_device)
{
}

AudioRender::~AudioRender()
{
}