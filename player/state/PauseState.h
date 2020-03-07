#pragma once

#include "BaseState.h"

class AudioRender;
class PauseState: public BaseState
{
private:
    AudioRender * const mpAudioRender;
public:
    PauseState(AudioRender *audio_render);
    ~PauseState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};
