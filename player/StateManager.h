#ifndef __STATE_MANAGER_H__
#define __STATE_MANAGER_H__

#include <condition_variable>
#include <map>
class BaseState;
class StateManager
{
public:
    //外部状态
    enum class PlayState {
        IDLE,
        PREPARING, //播放器开始创建各种对象，拉视频数据解码变换等
        PREPARED, //首帧准备就绪
        PLAYING, //播放中
        PAUSED, //用户暂停
        COMPLETED, //播放完成
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
    void onInit();
    /**
     * 播放器起播时，如果用户是已暂停状态起播，那么拿到首帧后，播放器暂停在首帧的位置
     * @return 是否首帧已经展示过
    */
    bool onFirstFramePrepared();
    void onPauseByUser();
    void onResumeByUser();

    StateManager::PlayState get_play_state();

    void add_state(StateManager::PlayState play_state, BaseState * state);

private:
    /* data */
    std::mutex mStateMutex;

    StateManager::PlayState mCurrentPlayState;

    bool mIsPlayingByUser;

    bool mIsFirstFrameShown;

    std::map<PlayState, BaseState*> mStates;

    void update_play_state(PlayState state);
};


#endif