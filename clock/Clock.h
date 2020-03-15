#ifndef __CLOCK_H__
#define __CLOCK_H__
#include <stdint.h>
extern "C"
{
    #include "libavutil/rational.h"
}
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
    /* 帧的serial*/
    int mLastSerial;
    /* 暂停时的系统时刻 */
    double mPauseStartTime;
    /* 是否暂停 */ 
    bool mIsPause;
    /*PTS时间单位*/
    AVRational mRational;
    /*seek后的当前系统时间无法通过 AVRational 和 pts 给出，则用seek_position*/
    int64_t mSeekPosition;
public:
    Clock(/* args */);
    ~Clock();

    /* 上次更新帧的系统时刻 */ 
    double get_last_update_time();
    /* 上一帧需要展示的时长 */ 
    double get_last_duration();
    /* 上次更新的帧的PTS 即播放时间轴的时刻（非系统时刻） */ 
    double get_last_pts();
    /* 转换过后的上次更新的帧的PTS */
    double get_transformed_last_pts();
    /* 转换过后的上次更新的帧的显示时长 */
    double get_transformed_last_duration();
    /* 更新时钟 */ 
    void update(double time, double pts, AVRational & time_base, int serial);

    /* seek */
    void seek(int64_t time, int64_t position);

    void pasue();

    void resume();
};

#endif