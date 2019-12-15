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

    double getLastUpdateTime();
    double getLastDuration();
    double getLastPts();

    void setLastUpdateTime(double time);
    void setLastDuration(double duration);
    void setLastPts(double pts);

    void update(double time, double duration, double pts);

    void pasue();

    void resume();

    /* 获得该条时间轴的时刻 */
    double getTime(double speed);

    /* 获得该条时间轴映射到系统时间轴的时刻*/
    double getMapingSystemTime(double speed);
};

#endif