#ifndef __CLOCK_H__
#define __CLOCK_H__
class Clock
{
private:
    // double pts;           /* clock base */
    // double pts_drift;     /* clock base minus time at which we updated the clock */
    // double last_updated;
    // double speed;
    // int serial;           /* clock is based on a packet with this serial */
    // int paused;
    // int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
    
    /* 展示视频帧或者音频帧的展示时长 */
    double mLastDuration;
    /* 展示视频帧或者音频帧的更新时间 系统时间轴*/
    double mLastUpdateTime;
    /* 展示视频帧或者音频帧的PTS */
    double mLastPts;
    /* 展示视频帧或者音频帧期间暂停的时长 */
    double mLastPauseDuration;

    /* 暂停时的系统时刻 */
    double mPauseStartTime;
    /* 是否暂停 */ 
    bool mIsPause;

public:
    Clock(/* args */);
    ~Clock();

    /* 上次更新帧的系统时刻 */ 
    double getLastUpdateTime();
    /* 上一帧需要展示的时长 */ 
    double getLastDuration();
    /* 上次更新的帧的PTS 即播放时间轴的时刻（非系统时刻） */ 
    double getLastPts();

    /* 更新时钟 */ 
    void update(double time, double duration, double pts);

    void pasue();

    void resume();
};

#endif