#ifndef __SYNC_STRATEGY_H__
#define __SYNC_STRATEGY_H__

#include "SyncClockManager.h"
#include "Clock.h"
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
    double mSpeed;
public:
    SyncStrategy(/* args */);
    ~SyncStrategy();

    virtual SyncClockManager::SyncState get_current_video_sync_state(double next_pts, double * remaining_time) = 0;

    virtual SyncClockManager::SyncState get_current_audio_sync_state(double next_pts, double * remaining_time) = 0;




};
#endif