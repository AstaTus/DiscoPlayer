#ifndef __DISCO_SDL_AUDIO_DEVICE_H__
#define __DISCO_SDL_AUDIO_DEVICE_H__


#include "SDL.h"

#include "../../render/audio/AudioDevice.h"
#include <atomic>
#include "../../common/thread/Semaphore.h"
class SDLAudioDevice : public AudioDevice
{
private:
    SDL_AudioSpec mAudioSpec;
    std::atomic<bool> mIsFlushing;
    Semaphore mFlushSemaphore;
    bool start_seek() override;
    bool end_seek() override;
public:
    SDLAudioDevice();
    virtual ~SDLAudioDevice();

    void fill_audio_buffer(Uint8 *stream, int len);

    bool start() override;
    bool pause() override;
    bool resume() override;

    bool stop() override;
};
#endif