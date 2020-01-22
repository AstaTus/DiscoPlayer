#ifndef __DISCO_SDL_AUDIO_DEVICE_H__
#define __DISCO_SDL_AUDIO_DEVICE_H__


#include "SDL.h"

#include "../../render/audio/AudioDevice.h"

class SDLAudioDevice : public AudioDevice
{
private:
    SDL_AudioSpec mAudioSpec;

    
public:
    SDLAudioDevice();
    virtual ~SDLAudioDevice();

    void fill_audio_buffer(Uint8 *stream, int len);

    bool open() override;
    bool close() override;
};
#endif