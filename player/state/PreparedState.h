#pragma once

#include "BaseState.h"
#include <future>
#include <thread> 
class AudioRender;
class StateManager;
class AudioFrameTransformer;

class PreparedState : public BaseState
{
private:
    AudioRender * const mpAudioRender;

    StateManager * mpStateManager;
    AudioFrameTransformer * const mpAudioFrameTransformer;
    std::thread * mpPreparedThread;

    bool mIsStartPaused;
    
    void inner_prepared_task();
public:
    PreparedState(AudioRender *audio_render,
                     StateManager *state_manager,
                     AudioFrameTransformer *audio_frame_transformer);
    ~PreparedState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};