#include "AudioMasterSyncStrategy.h"

#include <math.h>

extern "C"
{
    #include "libavutil/common.h"
    #include "libavutil/time.h"
}


/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.15
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 100.0



AudioMasterSyncStrategy::AudioMasterSyncStrategy(/* args */)
:SyncStrategy()
{

}

AudioMasterSyncStrategy::~AudioMasterSyncStrategy()
{
    
}

SyncClockManager::SyncState AudioMasterSyncStrategy::get_current_video_sync_state(double next_pts)
{
    double delay = compute_delay();

    double current_time = av_gettime_relative() / 1000000.0;
    //该帧还需继续显示
    if (current_time < mVideoClock.getLastUpdateTime() + delay)
    {
        return SyncClockManager::SYNC_STATE_KEEP;
    } 
    
    //更新时钟
    mVideoClock.update(current_time, next_pts - mVideoClock.getLastPts(), next_pts);
    if (current_time > mVideoClock.getLastUpdateTime() + mVideoClock.getLastDuration())
    {
        return SyncClockManager::SYNC_STATE_DROP;
    }


    
    return SyncClockManager::SYNC_STATE_NEXT;

}

SyncClockManager::SyncState AudioMasterSyncStrategy::get_current_audio_sync_state(double next_pts)
{
    
}

double AudioMasterSyncStrategy::compute_delay()
{
    double sync_threshold, diff = 0;
    double delay = mVideoClock.getLastDuration();
    /* update delay to follow master synchronisation source */
    
    /* if video is slave, we try to correct big delays by
        duplicating or deleting a frame */
    diff = mVideoClock.getTime(mSpeed) - mAudioClock.getTime(mSpeed);

    /* skip or repeat frame. We take into account the
        delay to compute the threshold. I still don't know
        if it is the best guess */
    sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
    // if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
    /* -- by bbcallen: replace is->max_frame_duration with AV_NOSYNC_THRESHOLD */
    if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
        //视频播放慢了，将两部分的差值合并
        if (diff <= -sync_threshold)
            delay = FFMAX(0, delay + diff);
        //视频播放快了
        else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
            delay = delay + diff;
        //视频播放快了
        else if (diff >= sync_threshold)
            delay = 2 * delay;
    }
    

//     if (ffp) {
//         ffp->stat.avdelay = delay;
//         ffp->stat.avdiff  = diff;
//     }
// #ifdef FFP_SHOW_AUDIO_DELAY
//     av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
//             delay, -diff);
// #endif

    return delay;
}