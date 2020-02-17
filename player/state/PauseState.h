#pragma once

#include "BaseState.h"

class AudioDevice;
class PauseState: public BaseState
{
private:
    AudioDevice * const mpAudioDevice;
public:
    PauseState(AudioDevice *audio_device);
    ~PauseState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};
