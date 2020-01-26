#include "StateManager.h"
#include "BaseState.h"

StateManager::StateManager(/* args */)
:mCurrentPlayState(StateManager::PlayState::IDLE),
mIsPlayingByUser(false),
mIsFirstFrameShown(false),
mStates()
{
}

StateManager::~StateManager()
{
}

void StateManager::onStreamOpen()
{

}

void StateManager::onDecodeStart()
{

}

void StateManager::onRenderStart()
{

}

void StateManager::onPauseByUser()
{
    std::lock_guard<std::mutex> state_lock(mStateMutex);

    mIsPlayingByUser = false;
    if (mCurrentPlayState == StateManager::PlayState::PLAYING)
    {
        update_play_state(StateManager::PlayState::PAUSED);
    }
}

void StateManager::onResumeByUser()
{
    std::lock_guard<std::mutex> state_lock(mStateMutex);
    mIsPlayingByUser = true;
    if (mCurrentPlayState == StateManager::PlayState::PREPARED ||
        mCurrentPlayState == StateManager::PlayState::PAUSED)
    {
        update_play_state(StateManager::PlayState::PLAYING);
    }
}

void StateManager::onInit()
{
    std::lock_guard<std::mutex> state_lock(mStateMutex);

    if (mCurrentPlayState == StateManager::PlayState::IDLE)
    {
        update_play_state(StateManager::PlayState::PREPARING);
    }
}

bool StateManager::onFirstFramePrepared()
{
    if (mIsFirstFrameShown)
    {
        return false;
    }

    std::lock_guard<std::mutex> state_lock(mStateMutex);

    if (mCurrentPlayState == StateManager::PlayState::PREPARING)
    {
        update_play_state(StateManager::PlayState::PREPARED);
        if (mIsPlayingByUser)
        {
            update_play_state(StateManager::PlayState::PLAYING);
        } else {
            update_play_state(StateManager::PlayState::PAUSED);
        }
    }
    mIsFirstFrameShown = true;
    return true;
}

StateManager::PlayState StateManager::get_play_state()
{
    return mCurrentPlayState;
}

void StateManager::update_play_state(PlayState state)
{
    BaseState * current_state = mStates[mCurrentPlayState];
    BaseState * next_state = mStates[state];
    if(current_state != nullptr) 
    {
        current_state->on_state_exit();
    }
    mCurrentPlayState = state;
    if(next_state != nullptr) 
    {
        next_state->on_state_enter();
    }
}

void StateManager::add_state(StateManager::PlayState play_state, BaseState * state)
{
    mStates[play_state] = state;
}
