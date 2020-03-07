#include "PreparedState.h"


#include "../../render/audio/AudioFrameTransformer.h"
#include "StateManager.h"
#include "../../render/audio/AudioRender.h"

#define PREPARED_STATE_ARG_SIZE 1

PreparedState::PreparedState(AudioRender *audio_render,
                     StateManager *state_manager,
                     AudioFrameTransformer *audio_frame_transformer)
    : mpAudioRender(audio_render),
      mpStateManager(state_manager),
      mpAudioFrameTransformer(audio_frame_transformer),
      mpPreparedThread(nullptr),
      mIsStartPaused(false)
{
}

PreparedState::~PreparedState()
{
}

void PreparedState::inner_prepared_task()
{
    mpAudioRender->set_audio_play_param(
        mpAudioFrameTransformer->get_output_sample_rate(),
        mpAudioFrameTransformer->get_output_sample_format(),
        mpAudioFrameTransformer->get_output_channel_num());

    if (mIsStartPaused == false)
    {
        mpStateManager->on_play();
    }
}

void PreparedState::on_state_enter(...)
{   va_list args;
    va_start(args, PREPARED_STATE_ARG_SIZE);
    mIsStartPaused = va_arg(args, bool);
    va_end(args);
    mpPreparedThread = new std::thread(&PreparedState::inner_prepared_task, this);
}

void PreparedState::on_state_exit()
{

}
