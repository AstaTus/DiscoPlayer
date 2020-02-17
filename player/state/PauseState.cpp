#include "PauseState.h"
#include "../../render/audio/AudioDevice.h"

PauseState::PauseState(AudioDevice *audio_device)
:mpAudioDevice(audio_device)
{
}

PauseState::~PauseState()
{
}

void PauseState::on_state_enter(...)
{   
    mpAudioDevice->pause();
}

void PauseState::on_state_exit()
{
    mpAudioDevice->resume();
}
