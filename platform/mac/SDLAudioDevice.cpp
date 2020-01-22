#include "SDLAudioDevice.h"

void sdl_audio_callback(void *udata, Uint8 *stream, int len) {
	static_cast<SDLAudioDevice * >(udata)->fill_audio_buffer(stream, len);
}

SDLAudioDevice::SDLAudioDevice()
    :AudioDevice()
{
}

SDLAudioDevice::~SDLAudioDevice()
{
}

bool SDLAudioDevice::open()
{
    /*** 初始化初始化SDL_AudioSpec结构体 ***/
	
	// 音频数据的采样率。常用的有48000，44100等
	mAudioSpec.freq = 48000; 
	
	// 音频数据的格式
	mAudioSpec.format = AUDIO_S16SYS;
	
	// 声道数。例如单声道取值为1，立体声取值为2
	mAudioSpec.channels = 2;
	
	// 设置静音的值
	mAudioSpec.silence = 0;

	// 音频缓冲区中的采样个数，要求必须是2的n次方
	mAudioSpec.samples = 1024;

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

	SDL_PauseAudio(0);

    return true;
}
bool SDLAudioDevice::close()
{
    SDL_CloseAudio();
}

void SDLAudioDevice::fill_audio_buffer(Uint8 *stream, int len)
{
    pAudioDataRequestListener->on_audio_data_request_begin();
	SDL_memset(stream, 0, len);//初始化缓存区
	int current_pos = 0;
	while (len > 0)
	{
		AudioClip * const audio_clip = pAudioDataRequestListener->on_audio_data_request(len);
		if (audio_clip == nullptr)
		{
			continue;
		}
		
		//TODO 混音
		//SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
		int read_size = audio_clip->size();
		if (len < audio_clip->size())
		{
			read_size = len;
		}

		memcpy(stream + current_pos, audio_clip->data(), read_size);
		len -= read_size;
		current_pos += read_size;
		audio_clip->add_read_size(read_size);
	}

	pAudioDataRequestListener->on_audio_data_request_end();
}