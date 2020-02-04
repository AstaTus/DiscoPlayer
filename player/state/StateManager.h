#ifndef __STATE_MANAGER_H__
#define __STATE_MANAGER_H__

#include <condition_variable>
#include <map>
#include <queue>
#include <atomic>
#include "BaseState.h"
#include "../../common/log/Log.h"
class StateManager
{
public:
    //外部状态
    enum class PlayState {
        INIT,
        PREPARING, //播放器开始创建各种对象，拉视频数据解码变换等
        PREPARED, //首帧暂停模式
        PLAYING, //播放中
        PAUSED, //用户暂停
        COMPLETED, //播放完成
        SEEKING, //SEEK
        //TODO
        BUFFERING, //当前无帧可以播放-音频和视频帧差的较大时也走这个状态
        STOPPED, //播放器停止？？？
        ERROR, //播放出错
        RELEASING, //播放器开始释放各种对象
        END //播放器释放各种对象完成
    };
    //内部状态
    StateManager(/* args */);
    ~StateManager();

    //stream 相关
    void onStreamOpen();
    //codec 相关
    void onDecodeStart();
    //render 相关
    void onRenderStart();
    //player 相关
    void on_prepare(bool is_start_pause, int64_t seek_position, const std::string& data_source);
    /**
     * 播放器起播时，如果用户是已暂停状态起播，那么拿到首帧后，播放器暂停在首帧的位置
     * @return 是否首帧已经展示过
    */
    bool onFirstFramePrepared(bool is_pause);
    /**
     * 
    */
    bool onSeekStart(int64_t position);

    bool onSeekEnd();

    void onPauseByUser();
    void onResumeByUser();

    StateManager::PlayState get_play_state();

    void add_state(StateManager::PlayState play_state, BaseState * state);

private:
    /* data */
    std::atomic<StateManager::PlayState> mCurrentPlayState;

    std::atomic<StateManager::PlayState> mLastPlayState;
    //TODO 可能需要历史的播放状态改变
    // std::mutex mHistoryStatesMutex;
    // std::queue<StateManager::PlayState> mHistoryPlayStateQueue;

    bool mIsPlayingByUser;

    std::map<PlayState, BaseState*> mStates;

    template <typename... Args>
    void update_play_state(PlayState state, Args&&... args);
};

template <typename... Args>
void StateManager::update_play_state(PlayState state, Args&&... args)
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
    }
}

#endif