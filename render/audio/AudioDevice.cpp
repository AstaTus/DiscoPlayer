#include "AudioDevice.h"
#include "AudioFillBufferListener.h"

AudioDevice::AudioDevice()
: mSampleRate(0),
mSampleFormat(AVSampleFormat::AV_SAMPLE_FMT_NONE),
mChannelNum(0),
mpAudioFillBufferListener(nullptr)
{
}

AudioDevice::~AudioDevice() 
{
    
}

void AudioDevice::set_audio_fill_buffer_listener(AudioFillBufferListener * listener)
{
    mpAudioFillBufferListener = listener;
}

void AudioDevice::fill_audio_buffer(u_int8_t *stream, int len)
{	
	if (mpAudioFillBufferListener != nullptr)
	{
		mpAudioFillBufferListener->on_fill_audio_buffer(stream, len);
	}
}

void AudioDevice::set_audio_play_param(int sample_rate, AVSampleFormat sample_format, int channel_num)
{
    mSampleRate = sample_rate;
    mSampleFormat = sample_format;
    mChannelNum = channel_num;
}