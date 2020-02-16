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
    int mBytesPerSecond;
    double mLatencySeconds;
public:
    SDLAudioDevice();
    virtual ~SDLAudioDevice();

    void fill_audio_buffer(Uint8 *stream, int len);

    bool start() override;
    bool pause() override;
    bool resume() override;

    bool stop() override;

    int get_bytes_per_second() override;
    double get_latency_seconds() override;
};
#endif