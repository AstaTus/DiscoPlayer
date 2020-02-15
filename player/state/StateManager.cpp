#include "StateManager.h"


StateManager::StateManager(/* args */)
:mCurrentPlayState(PlayerStateEnum::INIT),
mIsPlayingByUser(false),
mStates(),
mLastPlayState(PlayerStateEnum::INIT)
{
}

StateManager::~StateManager()
{
    if (mpStateChangedListener != nullptr)
    {
        mpStateChangedListener = nullptr;
    }
    
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

bool StateManager::onSeekStart(int64_t position)
{
    //TODO 播放完成状态（COMPLETED）是否可以seek 
    if (mCurrentPlayState == PlayerStateEnum::PLAYING ||
        mCurrentPlayState == PlayerStateEnum::PAUSED ||
        mCurrentPlayState == PlayerStateEnum::BUFFERING)
    {
        update_play_state(PlayerStateEnum::SEEKING, position);
        return true;
    }

    return false;
}

bool StateManager::onSeekEnd()
{
    if (mCurrentPlayState == PlayerStateEnum::SEEKING)
    {
        update_play_state(mLastPlayState);
    }
}

void StateManager::onPauseByUser()
{
    mIsPlayingByUser = false;
    if (mCurrentPlayState == PlayerStateEnum::PLAYING)
    {
        update_play_state(PlayerStateEnum::PAUSED);
    }
}

void StateManager::onResumeByUser()
{
    mIsPlayingByUser = true;
    if (mCurrentPlayState == PlayerStateEnum::PAUSED)
    {
        update_play_state(PlayerStateEnum::PLAYING);
    }
}

void StateManager::on_prepare(bool is_start_pause, int64_t seek_position, const std::string& data_source)
{
    if (mCurrentPlayState == PlayerStateEnum::INIT)
    {
        update_play_state(PlayerStateEnum::PREPARING, is_start_pause, seek_position, data_source.c_str());
    }
}

bool StateManager::onFirstFramePrepared(bool is_pause)
{
    if (mCurrentPlayState == PlayerStateEnum::PREPARING)
    {
        if (!is_pause)
        {
            update_play_state(PlayerStateEnum::PLAYING);
        } else {
            update_play_state(PlayerStateEnum::PREPARED);
        }
    }
    return true;
}

PlayerStateEnum StateManager::get_play_state()
{
    return mCurrentPlayState;
}



void StateManager::add_state(PlayerStateEnum play_state, BaseState * state)
{
    mStates[play_state] = state;
}

void StateManager::set_state_changed_listener(StateChangedListener * listener)
{
    mpStateChangedListener = listener;
}
