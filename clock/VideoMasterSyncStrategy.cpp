#include "VideoMasterSyncStrategy.h"
#include "../common/log/Log.h"
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


SyncClockManager::SyncState VideoMasterSyncStrategy::get_current_video_sync_state(
    double next_pts, AVRational & time_base, int serial, long long * remaining_time, double current_time)
{
    SyncClockManager::SyncState state;
    //该帧还需继续显示
    double change_video_time = mVideoClock.get_last_update_time() + av_q2d(time_base) * mVideoClock.get_last_duration() / mSpeed;
    if (current_time < change_video_time)
    {
        *remaining_time = change_video_time - current_time;
        state = SyncClockManager::SyncState::SYNC_STATE_KEEP;
    } else {
        //更新时钟
        mVideoClock.update(current_time, next_pts, time_base, serial);
        if (current_time > mVideoClock.get_last_update_time())
        {
            state = SyncClockManager::SyncState::SYNC_STATE_DROP;
        } else {
            state = SyncClockManager::SyncState::SYNC_STATE_NEXT;
        }
    }

    Log::get_instance().log_debug("[Disco][VideoMasterSyncStrategy::get_current_video_sync_state] sync_state = %d\n", state);
    return state;
    
    
}

SyncClockManager::SyncState VideoMasterSyncStrategy::get_current_audio_sync_state(
    double next_pts, AVRational & time_base, int serial, long long * remaining_time, double current_time)
{
    return SyncClockManager::SyncState::SYNC_STATE_NEXT;
}

void VideoMasterSyncStrategy::resume()
{

}

void VideoMasterSyncStrategy::pause()
{

}

int64_t VideoMasterSyncStrategy::get_current_position()
{
    return 0;
}


void VideoMasterSyncStrategy::seek(uint64_t postion)
{
    int64_t current_time = av_gettime_relative() / 1000000.0;
    mAudioClock.seek(current_time, postion);
    mVideoClock.seek(current_time, postion);
}

double VideoMasterSyncStrategy::get_video_pts()
{
    return mVideoClock.get_last_pts();
}

double VideoMasterSyncStrategy::get_audio_pts()
{
    return mAudioClock.get_last_pts();
}

int64_t VideoMasterSyncStrategy::get_video_position()
{
    return mVideoClock.get_transformed_last_pts();
}

int64_t VideoMasterSyncStrategy::get_audio_position()
{
    return mAudioClock.get_transformed_last_pts();
}

