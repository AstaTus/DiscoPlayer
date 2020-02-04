#include "StateManager.h"


StateManager::StateManager(/* args */)
:mCurrentPlayState(StateManager::PlayState::INIT),
mIsPlayingByUser(false),
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

bool StateManager::onSeekStart(int64_t position)
{
    //TODO 播放完成状态（COMPLETED）是否可以seek 
    if (mCurrentPlayState == StateManager::PlayState::PLAYING ||
        mCurrentPlayState == StateManager::PlayState::PAUSED ||
        mCurrentPlayState == StateManager::PlayState::BUFFERING)
    {
        update_play_state(StateManager::PlayState::SEEKING, position);
        return true;
    }

    return false;
}

bool StateManager::onSeekEnd()
{
    if (mCurrentPlayState == StateManager::PlayState::SEEKING)
    {
        update_play_state(mLastPlayState);
    }
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

void StateManager::on_prepare(bool is_start_pause, int64_t seek_position, const std::string& data_source)
{
    if (mCurrentPlayState == StateManager::PlayState::INIT)
    {
        update_play_state(StateManager::PlayState::PREPARING, is_start_pause, seek_position, data_source.c_str());
    }
}

bool StateManager::onFirstFramePrepared(bool is_pause)
{
    if (mCurrentPlayState == StateManager::PlayState::PREPARING)
    {
        if (!is_pause)
        {
            update_play_state(StateManager::PlayState::PLAYING);
        } else {
            update_play_state(StateManager::PlayState::PREPARED);
        }
    }
    return true;
}

StateManager::PlayState StateManager::get_play_state()
{
    return mCurrentPlayState;
}



void StateManager::add_state(StateManager::PlayState play_state, BaseState * state)
{
    mStates[play_state] = state;
}
