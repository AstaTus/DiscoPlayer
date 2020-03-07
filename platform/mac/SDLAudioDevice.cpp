#include "SDLAudioDevice.h"
#include "../../common/log/Log.h"

#define SDL_AUDIO_MIN_BUFFER_SIZE 512

void sdl_audio_callback(void *udata, Uint8 *stream, int len) {
	static_cast<SDLAudioDevice * >(udata)->fill_audio_buffer(stream, len);
}

SDLAudioDevice::SDLAudioDevice()
    :AudioDevice(),
	mIsFlushing(false),
	mFlushSemaphore(),
	mBytesPerSecond(0)
{
}

SDLAudioDevice::~SDLAudioDevice()
{
}

bool SDLAudioDevice::start()
{
	SDL_AudioStatus status = SDL_GetAudioStatus();
	if (status != SDL_AUDIO_STOPPED)
	{
		return true;
	}
	
    /*** 初始化初始化SDL_AudioSpec结构体 ***/
	
	// 音频数据的采样率。常用的有48000，44100等
	mAudioSpec.freq = mSampleRate; 
	
	// 音频数据的格式
	if (mSampleFormat == AV_SAMPLE_FMT_S16)
	{
		mAudioSpec.format = AUDIO_S16SYS;
	} else {
		//TODO 抛出异常
		//throw 
	}
	
	// 声道数。例如单声道取值为1，立体声取值为2
	mAudioSpec.channels = mChannelNum;
	
	// 设置静音的值
	mAudioSpec.silence = 0;

	// 音频缓冲区中的采样个数，要求必须是2的n次方
	// mAudioSpec.samples = std::max(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << (mAudioSpec.freq ));;
	mAudioSpec.samples = SDL_AUDIO_MIN_BUFFER_SIZE;
	// 填充音频缓冲区的回调函数
	mAudioSpec.callback = &sdl_audio_callback;
	mAudioSpec.userdata = this;
	/************************************/
	
	// 打开音频设备
	if (SDL_OpenAudio(&mAudioSpec, nullptr) < 0)
	{
		printf("Can not open audio!");
		return false;
	}

	mBytesPerSecond = av_samples_get_buffer_size(nullptr, mAudioSpec.channels, mAudioSpec.freq, mSampleFormat, 1);
	mLatencySeconds = ((double)(2 * mAudioSpec.size)) / mBytesPerSecond;

    return true;
}

int SDLAudioDevice::get_bytes_per_second()
{
	return mBytesPerSecond;
}

double SDLAudioDevice::get_latency_seconds()
{
	return mLatencySeconds;
}

bool SDLAudioDevice::pause()
{
	SDL_PauseAudio(1);
	return true;
}

bool SDLAudioDevice::resume()
{
	SDL_PauseAudio(0);
	return true;
}

bool SDLAudioDevice::stop()
{
    SDL_CloseAudio();
	return true;
}
