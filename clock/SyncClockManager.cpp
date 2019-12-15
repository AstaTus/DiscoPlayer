#include "SyncClockManager.h"
#include "AudioMasterSyncStrategy.h"
#include "VideoMasterSyncStrategy.h"

SyncClockManager::SyncClockManager(SyncStrategy strategy)
{
    if(strategy == SYNC_STRATEGY_AUDIO) {
         pSyncStrategy = new AudioMasterSyncStrategy();
    } else if (strategy == SYNC_STRATEGY_VIDEO)
    {
        pSyncStrategy = new AudioMasterSyncStrategy();
    }
}

SyncClockManager::~SyncClockManager()
{
}

void SyncClockManager::update_system_time()
{

}

SyncClockManager::SyncState SyncClockManager::get_current_video_sync_state(double next_pts, double * remaining_time)
{
    return pSyncStrategy->get_current_video_sync_state(next_pts, remaining_time);
}

SyncClockManager::SyncState SyncClockManager::get_current_audio_sync_state(double next_pts, double * remaining_time)
{
    return pSyncStrategy->get_current_audio_sync_state(next_pts, remaining_time);
}
