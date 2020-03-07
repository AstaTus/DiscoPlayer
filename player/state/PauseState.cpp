#include "PauseState.h"
#include "../../render/audio/AudioRender.h"

PauseState::PauseState(AudioRender *audio_render)
:mpAudioRender(audio_render)
{
}

PauseState::~PauseState()
{
}

void PauseState::on_state_enter(...)
{   
    mpAudioRender->pause();
}

void PauseState::on_state_exit()
{
    mpAudioRender->resume();
}
