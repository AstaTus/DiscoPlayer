#include "StateManager.h"
#include "BaseState.h"

StateManager::StateManager(/* args */)
:mCurrentPlayState(StateManager::PlayState::INIT),
mIsPlayingByUser(false),
mIsFirstFrameShown(false),
mStates(),
mLastPlayState(StateManager::PlayState::INIT)
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

bool StateManager::onSeekStart()
{
    //TODO 播放完成状态（COMPLETED）是否可以seek 
    if (mCurrentPlayState == StateManager::PlayState::PLAYING ||
        mCurrentPlayState == StateManager::PlayState::PAUSED ||
        mCurrentPlayState == StateManager::PlayState::BUFFERING)
    {
        update_play_state(StateManager::PlayState::SEEKING);
    }
    
}

bool StateManager::onSeekEnd()
{
    // if ()
    // {
    //     /* code */
    // }
}

void StateManager::onPauseByUser()
{
    mIsPlayingByUser = false;
    if (mCurrentPlayState == StateManager::PlayState::PLAYING)
    {
        update_play_state(StateManager::PlayState::PAUSED);
    }
}

void StateManager::onResumeByUser()
{
    mIsPlayingByUser = true;
    if (mCurrentPlayState == StateManager::PlayState::PAUSED)
    {
        update_play_state(StateManager::PlayState::PLAYING);
    }
}

void StateManager::onInit()
{
    if (mCurrentPlayState == StateManager::PlayState::INIT)
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

    if (mCurrentPlayState == StateManager::PlayState::PREPARING)
    {
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
    // std::lock_guard<std::mutex> state_lock(mStateMutex);
    
    BaseState * current_state = mStates[mCurrentPlayState];
    BaseState * next_state = mStates[state];
    if(current_state != nullptr) 
    {
        current_state->on_state_exit();
    }
    mLastPlayState.store(mCurrentPlayState.load());
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
