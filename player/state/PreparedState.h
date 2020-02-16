#pragma once

#include "BaseState.h"
#include <future>
#include <thread> 
class AudioDevice;
class StateManager;
class AudioFrameTransformer;

class PreparedState : public BaseState
{
private:
    AudioDevice * const mpAudioDevice;
    StateManager * mpStateManager;
    AudioFrameTransformer * const mpAudioFrameTransformer;
    std::thread * mpPreparedThread;

    bool mIsStartPaused;
    
    void inner_prepared_task();
public:
    PreparedState(AudioDevice *audio_device,
                     StateManager *state_manager,
                     AudioFrameTransformer *audio_frame_transformer);
    ~PreparedState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};