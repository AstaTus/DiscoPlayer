#include "VideoMasterSyncStrategy.h"

extern "C"
{
    #include "libavutil/time.h"
}

VideoMasterSyncStrategy::VideoMasterSyncStrategy(/* args */)
:SyncStrategy()
{
}

VideoMasterSyncStrategy::~VideoMasterSyncStrategy()
{
}


SyncClockManager::SyncState VideoMasterSyncStrategy::get_current_video_sync_state(double next_pts, double * remaining_time)
{
    double current_time = av_gettime_relative() / 1000000.0;
    //该帧还需继续显示
    double change_video_time = mVideoClock.getLastUpdateTime() + mVideoClock.getLastDuration() / mSpeed;
    if (current_time < change_video_time)
    {
        *remaining_time = change_video_time - current_time;
        return SyncClockManager::SYNC_STATE_KEEP;
    } 
    
    //更新时钟
    mVideoClock.update(current_time, next_pts - mVideoClock.getLastPts(), next_pts);
    if (current_time > change_video_time)
    {
        return SyncClockManager::SYNC_STATE_DROP;
    }

    return SyncClockManager::SYNC_STATE_NEXT;
}

SyncClockManager::SyncState VideoMasterSyncStrategy::get_current_audio_sync_state(double next_pts, double * remaining_time)
{
    return SyncClockManager::SYNC_STATE_NEXT;
}

