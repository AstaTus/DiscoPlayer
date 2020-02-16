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

bool StateManager::on_seek_start(int64_t position)
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

bool StateManager::on_seek_end()
{
    if (mCurrentPlayState == PlayerStateEnum::SEEKING)
    {
        update_play_state(mLastPlayState);
    }
}

void StateManager::on_play()
{
    if (mCurrentPlayState == PlayerStateEnum::PREPARED)
    {
        update_play_state(PlayerStateEnum::PLAYING);
    }
}

void StateManager::on_pause_by_user()
{
    mIsPlayingByUser = false;
    if (mCurrentPlayState == PlayerStateEnum::PLAYING)
    {
        update_play_state(PlayerStateEnum::PAUSED);
    }
}

void StateManager::on_resume_by_user()
{
    mIsPlayingByUser = true;
    if (mCurrentPlayState == PlayerStateEnum::PAUSED)
    {
        update_play_state(PlayerStateEnum::PLAYING);
    }
}

void StateManager::on_play_by_user()
{
    if (mCurrentPlayState == PlayerStateEnum::PREPARED)
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

bool StateManager::on_prepared(bool is_pause)
{
    if (mCurrentPlayState == PlayerStateEnum::PREPARING)
    {
        update_play_state(PlayerStateEnum::PREPARED, is_pause);
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
