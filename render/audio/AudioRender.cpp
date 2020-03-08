#include "AudioRender.h"
#include "AudioClip.h"
#include "AudioDevice.h"
#include "AudioDataRequestListener.h"
#include "AudioDeviceBuffer.h"
#include "SoundTouchDeviceBufferProcessor.h"
const int AudioRender::DEFULT_AUDIO_VOLUME = 64;
const int AudioRender::MAX_AUDIO_VOLUME = 100;
const float AudioRender::DEFULT_AUDIO_SPEED = 1.0f;

AudioRender::AudioRender(AudioDevice *const audio_device)
    : mpAudioDevice(audio_device),
	mAudioVolume(DEFULT_AUDIO_VOLUME),
	mSpeed(DEFULT_AUDIO_SPEED)
{
	mpAudioDevice->set_audio_fill_buffer_listener(this);
    mAudioDeviceBuffer.push_back_porcessor(new SoundTouchDeviceBufferProcessor());
}

AudioRender::~AudioRender()
{
}

void AudioRender::set_audio_data_request_listener(AudioDataRequestListener * audio_data_request_listener)
{
	mAudioDeviceBuffer.set_audio_data_request_listener(audio_data_request_listener);
}

void AudioRender::on_fill_audio_buffer(uint8_t * buffer, long len)
{
    mAudioDeviceBuffer.fill_target_buffer(buffer, len, mSpeed, mAudioVolume, mOutFormat);
}

void AudioRender::set_volume(int volume) 
{
    if (volume >= 0 && volume <= MAX_AUDIO_VOLUME)
    {
        mAudioVolume = volume;
    }
}

int AudioRender::get_volume() {
    return mAudioVolume;
}

void AudioRender::set_speed(float speed)
{
	mSpeed = speed;
}

float AudioRender::get_speed()
{
	return mSpeed;
}

void AudioRender::set_audio_play_param(int sample_rate, AVSampleFormat sample_format, int channel_num)
{
    mSampleRate = sample_rate;
    mOutFormat = sample_format;
    mChannelNum = channel_num;

    if (mpAudioDevice != nullptr)
    {
        mpAudioDevice->set_audio_play_param(mSampleRate, mOutFormat, mChannelNum);
    }
}

bool AudioRender::stop()
{
    if (mpAudioDevice != nullptr)
    {
        return mpAudioDevice->stop();
    }

    return false;
}

bool AudioRender::start()
{
    if (mpAudioDevice != nullptr)
    {
        return mpAudioDevice->start();
    }

    return false;
}

bool AudioRender::pause()
{
    if (mpAudioDevice != nullptr)
    {
        return mpAudioDevice->pause();
    }

    return false;
}

void AudioRender::set_last_buffer_processor(AudioDeviceLastBufferProcessor * audio_last_buffer_processor)
{
    mAudioDeviceBuffer.push_back_last_porcessor(audio_last_buffer_processor);   
}


bool AudioRender::resume()
{
    if (mpAudioDevice != nullptr)
    {
        return mpAudioDevice->resume();
    }

    return false;
}

uint32_t AudioRender::get_cache_buffer_size()
{
    return mAudioDeviceBuffer.get_cache_buffer_size();
}