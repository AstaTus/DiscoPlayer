#include "PlayingState.h"
#include "../../render/audio/AudioDevice.h"

PlayingState::PlayingState(AudioDevice * audio_device)
:BaseState(),
mpAudioDevice(audio_device)
{

}

PlayingState::~PlayingState()
{

}

void PlayingState::on_state_enter()
{
    mpAudioDevice->resume();
}

void PlayingState::on_state_exit()
{
    mpAudioDevice->pause();
}
