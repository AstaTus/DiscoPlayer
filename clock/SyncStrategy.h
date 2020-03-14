#ifndef __SYNC_STRATEGY_H__
#define __SYNC_STRATEGY_H__

#include "SyncClockManager.h"
#include "Clock.h"
#include <stdint.h>
class SyncStrategy
{
protected:
    /* 视频时钟 */
    Clock mVideoClock;
    /* 音频时钟 */
    Clock mAudioClock;
    /* 自然时钟 */
    Clock mNaturalClock;

    /*时钟更新的系统时刻*/
    double mSystemTime;

    /*倍速*/
    float mSpeed;
public:
    SyncStrategy(/* args */);
    virtual ~SyncStrategy();

    virtual SyncClockManager::SyncState get_current_video_sync_state(
        double next_pts, AVRational & time_base, int serial, long long * remaining_time, double current_time) = 0;

    virtual SyncClockManager::SyncState get_current_audio_sync_state(
        double next_pts, AVRational & time_base, int serial, long long * remaining_time, double current_time) = 0;

    virtual void seek(uint64_t postion) = 0;

    virtual void resume() = 0;

    virtual void pause() = 0;

    virtual int64_t get_current_position() = 0;

    virtual double get_video_pts() = 0;

    virtual double get_audio_pts() = 0;

    virtual int64_t get_video_position() = 0;

    virtual int64_t get_audio_position() = 0;

    float get_speed();

    void set_speed(float speed);
};
#endif