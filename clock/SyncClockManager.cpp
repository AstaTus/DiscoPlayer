#include "SyncClockManager.h"
#include "AudioMasterSyncStrategy.h"
#include "VideoMasterSyncStrategy.h"
extern "C"
{
#include "libavutil/rational.h"
}

SyncClockManager::SyncClockManager(SyncStrategy strategy)
{
    if(strategy == SYNC_STRATEGY_AUDIO) {
         pSyncStrategy = new AudioMasterSyncStrategy();
    } else if (strategy == SYNC_STRATEGY_VIDEO)
    {
        pSyncStrategy = new VideoMasterSyncStrategy();
    }
}

SyncClockManager::~SyncClockManager()
{
}

SyncClockManager::SyncState SyncClockManager::get_current_video_sync_state(double next_pts, AVRational & time_base, double * remaining_time)
{
    return pSyncStrategy->get_current_video_sync_state(next_pts, time_base, remaining_time);
}

SyncClockManager::SyncState SyncClockManager::get_current_audio_sync_state(double next_pts, AVRational & time_base, double * remaining_time)
{
    return pSyncStrategy->get_current_audio_sync_state(next_pts, time_base, remaining_time);
}

void SyncClockManager::pause() 
{
    pSyncStrategy->pause();
}

void SyncClockManager::resume()
{
    pSyncStrategy->resume();
}
