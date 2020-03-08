#include "SDLMixAudioDeviceBufferProcessor.h"
#include "SDL.h"

SDLMixAudioDeviceBufferProcessor::SDLMixAudioDeviceBufferProcessor(/* args */)
{
}

SDLMixAudioDeviceBufferProcessor::~SDLMixAudioDeviceBufferProcessor()
{
}

uint32_t SDLMixAudioDeviceBufferProcessor::process_buffer(uint8_t * buffer, 
        long buffer_size, int sample_rate, int sample_size, int channel_size, 
        float speed, int bytes_per_sample, enum AVSampleFormat out_format, int volume, uint8_t ** out_buffer)
{
    if (buffer_size == 0)
    {
        return 0;
    }
    
    int rest_need_buffer_size = mTargetBufferSize - mTargetBufferValidSize;

    if( rest_need_buffer_size < buffer_size) {
        SDL_MixAudio(mpTargetBuffer + mTargetBufferValidSize, buffer, rest_need_buffer_size, volume);
        mTargetBufferValidSize += rest_need_buffer_size;

        set_cache_buffer(buffer + rest_need_buffer_size, buffer_size - rest_need_buffer_size);
    } else { 
        SDL_MixAudio(mpTargetBuffer + mTargetBufferValidSize, buffer, buffer_size, volume);
        mTargetBufferValidSize += buffer_size;
    }
    return rest_need_buffer_size;
}

bool SDLMixAudioDeviceBufferProcessor::fill_buffer_by_cache(int volume)
{
    if (get_cache_buffer_size() == 0)
    {
        return false;
    }
    
    int rest_need_buffer_size = mTargetBufferSize - mTargetBufferValidSize;
    if( rest_need_buffer_size < get_cache_buffer_size()) {
        SDL_MixAudio(mpTargetBuffer + mTargetBufferValidSize, get_cache_buffer(), rest_need_buffer_size, volume);
        mTargetBufferValidSize += rest_need_buffer_size;

        add_cache_read_size(rest_need_buffer_size);
        
    } else {
        SDL_MixAudio(mpTargetBuffer + mTargetBufferValidSize, get_cache_buffer(), get_cache_buffer_size(), volume);
        mTargetBufferValidSize += get_cache_buffer_size();

        add_cache_read_size(get_cache_buffer_size());
    }

    return true;
}