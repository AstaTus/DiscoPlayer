#include "AudioMasterSyncStrategy.h"

#include <math.h>
#include <algorithm>
#include "../common/log/Log.h"
extern "C"
{
    #include "libavutil/time.h"
    #include "libavutil/rational.h"
}


/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 40.0
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 100.0
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 150.0
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 100000.0



AudioMasterSyncStrategy::AudioMasterSyncStrategy(/* args */)
:SyncStrategy()
{

}

AudioMasterSyncStrategy::~AudioMasterSyncStrategy()
{
    
}

SyncClockManager::SyncState AudioMasterSyncStrategy::get_current_audio_sync_state(double next_pts, 
            AVRational & time_base, int serial, long long * remaining_time, double current_time)
{
    SyncClockManager::SyncState state;
    
    //更新时钟
    mAudioClock.update(current_time, next_pts, time_base, serial);
    Log::get_instance().log_debug("[DISCO]AudioMasterSyncStrategy::get_current_audio_sync_state time = %f serial = %d\n", av_q2d(time_base) * next_pts, serial);
    return SyncClockManager::SyncState::SYNC_STATE_NEXT;
}

SyncClockManager::SyncState AudioMasterSyncStrategy::get_current_video_sync_state(double next_pts, 
            AVRational & time_base, int serial, long long * remaining_time, double current_time)
{
    SyncClockManager::SyncState state;
    //该帧还需继续显示
    double change_video_time = mVideoClock.get_last_update_time() + compute_real_video_last_duration() / mSpeed;
    if (current_time < change_video_time)
    {
        *remaining_time = change_video_time - current_time;
        state = SyncClockManager::SyncState::SYNC_STATE_KEEP;
    } else {
        //更新时钟
        mVideoClock.update(current_time, next_pts, time_base, serial);
        Log::get_instance().log_debug("[DISCO]AudioMasterSyncStrategy::get_current_video_sync_state time = %f serial = %d\n", av_q2d(time_base) * next_pts, serial);
        if (current_time > mVideoClock.get_last_update_time())
        {
            *remaining_time = 0;
            state = SyncClockManager::SyncState::SYNC_STATE_DROP;
        } else {
            *remaining_time = 0;
            state = SyncClockManager::SyncState::SYNC_STATE_NEXT;
        }
    }

    // Log::get_instance().log_debug("[Disco][VideoMasterSyncStrategy::get_current_video_sync_state] sync_state = %d\n", state);
    return state;

}


double AudioMasterSyncStrategy::compute_real_video_last_duration()
{
    double sync_threshold, diff = 0;
    double video_last_duration = mVideoClock.get_transformed_last_duration();
    double video_real_last_duration = video_last_duration;
    sync_threshold = std::max(AV_SYNC_THRESHOLD_MIN, std::min(AV_SYNC_THRESHOLD_MAX, video_last_duration));
    //TODO video 和 audio的timebase 一致吗
    diff = mVideoClock.get_transformed_last_pts() - mAudioClock.get_transformed_last_pts();

    // if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
    /* -- by bbcallen: replace is->max_frame_duration with AV_NOSYNC_THRESHOLD */
    //将video的last duration 和  两个时钟的diff 合并
    if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
        if (diff <= -sync_threshold)
            video_real_last_duration = std::max(0.0, video_last_duration + diff);
        //视频播放快了
        else if (diff >= sync_threshold && video_last_duration > AV_SYNC_FRAMEDUP_THRESHOLD)
            video_real_last_duration = video_last_duration + diff;
        //视频播放快了
        else if (diff >= sync_threshold)
            video_real_last_duration = 2 * video_last_duration;
    }
    return video_real_last_duration;
}

void AudioMasterSyncStrategy::resume()
{

}

void AudioMasterSyncStrategy::pause()
{
    
}

int64_t AudioMasterSyncStrategy::get_current_position()
{
    return mAudioClock.get_transformed_last_pts();
}

void AudioMasterSyncStrategy::seek(uint64_t postion)
{
    int64_t current_time = av_gettime_relative() / 1000000.0;
    mAudioClock.seek(current_time, postion);
    mVideoClock.seek(current_time, postion);
}

double AudioMasterSyncStrategy::get_video_pts()
{
    return mVideoClock.get_last_pts();
}

double AudioMasterSyncStrategy::get_audio_pts()
{
    return mAudioClock.get_last_pts();
}

int64_t AudioMasterSyncStrategy::get_video_position()
{
    return mVideoClock.get_transformed_last_pts();
}

int64_t AudioMasterSyncStrategy::get_audio_position()
{
    return mAudioClock.get_transformed_last_pts();
}
