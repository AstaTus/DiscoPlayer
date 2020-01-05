#include "VideoMasterSyncStrategy.h"

extern "C"
{
    #include "libavutil/time.h"
    #include "libavutil/log.h"
}

VideoMasterSyncStrategy::VideoMasterSyncStrategy(/* args */)
:SyncStrategy()
{
}

VideoMasterSyncStrategy::~VideoMasterSyncStrategy()
{
}


SyncClockManager::SyncState VideoMasterSyncStrategy::get_current_video_sync_state(double next_pts, AVRational & time_base, double * remaining_time)
{
    SyncClockManager::SyncState state;
    double current_time = av_gettime_relative() / 1000000.0;
    //该帧还需继续显示
    double change_video_time = mVideoClock.getLastUpdateTime() + av_q2d(time_base) * mVideoClock.getLastDuration() / mSpeed;
    if (current_time < change_video_time)
    {
        *remaining_time = change_video_time - current_time;
        state = SyncClockManager::SyncState::SYNC_STATE_KEEP;
    } else {
        //更新时钟
        mVideoClock.update(current_time, next_pts - mVideoClock.getLastPts(), next_pts);
        if (current_time > mVideoClock.getLastUpdateTime())
        {
            state = SyncClockManager::SyncState::SYNC_STATE_DROP;
        } else {
            state = SyncClockManager::SyncState::SYNC_STATE_NEXT;
        }
    }

    av_log(nullptr, AV_LOG_DEBUG, "[Disco][VideoMasterSyncStrategy::get_current_video_sync_state] sync_state = %d\n", state);
    return state;
    
    
}

SyncClockManager::SyncState VideoMasterSyncStrategy::get_current_audio_sync_state(double next_pts, AVRational & time_base, double * remaining_time)
{
    return SyncClockManager::SyncState::SYNC_STATE_NEXT;
}

void VideoMasterSyncStrategy::resume()
{

}

void VideoMasterSyncStrategy::pause()
{

}

