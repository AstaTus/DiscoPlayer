#ifndef __PLAYING_STATE_H__
#define __PLAYING_STATE_H__
#include "BaseState.h"

class AudioDevice;
class PlayingState: public BaseState
{
public:
    PlayingState(AudioDevice * audio_device);
    ~PlayingState();


    virtual void on_state_enter() override;
    virtual void on_state_exit() override;

private:
    AudioDevice * mpAudioDevice;
};
#endif