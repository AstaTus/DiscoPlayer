#ifndef __STATE_MANAGER_H__
#define __STATE_MANAGER_H__

#include <condition_variable>
#include <map>
#include <queue>
#include <atomic>
#include "BaseState.h"
#include "../../common/log/Log.h"
#include "StateChangedListener.h"
 

class StateManager
{
public:
    StateManager(/* args */);
    ~StateManager();

    //player 相关
    void on_prepare(bool is_start_pause, int64_t seek_position, const std::string& data_source);

    void on_play();
    /**
     * 播放器起播时，如果用户是已暂停状态起播，那么拿到首帧后，播放器暂停在首帧的位置
     * @return 是否首帧已经展示过
    */
    bool on_prepared(bool is_pause);
    /**
     * 
    */
    bool on_seek_start(int64_t position);

    bool on_seek_end();

    void on_pause_by_user();
    void on_resume_by_user();
    void on_play_by_user();

    PlayerStateEnum get_play_state();

    void add_state(PlayerStateEnum play_state, BaseState * state);

    void set_state_changed_listener(StateChangedListener * listener);

private:
    /* data */
    std::atomic<PlayerStateEnum> mCurrentPlayState;

    std::atomic<PlayerStateEnum> mLastPlayState;
    //TODO 可能需要历史的播放状态改变
    // std::mutex mHistoryStatesMutex;
    // std::queue<PlayerStateEnum> mHistoryPlayStateQueue;

    StateChangedListener * mpStateChangedListener;

    bool mIsPlayingByUser;

    std::map<PlayerStateEnum, BaseState*> mStates;



    template <typename... Args>
    void update_play_state(PlayerStateEnum state, Args&&... args);
};

template <typename... Args>
void StateManager::update_play_state(PlayerStateEnum state, Args&&... args)
{
    BaseState * current_state = mStates[mCurrentPlayState];
    BaseState * next_state = mStates[state];
    if(current_state != nullptr) 
    {
        current_state->on_state_exit();
        Log::get_instance().log_debug("[Disco][StateManager::update_play_state state = %d exit\n", static_cast<int>(mCurrentPlayState.load()));
    }
    mLastPlayState.store(mCurrentPlayState.load());
    mCurrentPlayState = state;
    if(next_state != nullptr) 
    {
        Log::get_instance().log_debug("[Disco][StateManager::update_play_state state = %d enter\n", static_cast<int>(mCurrentPlayState.load()));
        next_state->on_state_enter(std::forward<Args>(args)...);
        //TODO maybe diffrent state in out
        if (mpStateChangedListener != nullptr)
        {
            mpStateChangedListener->on_state_changed(mCurrentPlayState);
        }
    }
}

#endif