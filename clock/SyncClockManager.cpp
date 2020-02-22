#include "SyncClockManager.h"
#include "AudioMasterSyncStrategy.h"
#include "VideoMasterSyncStrategy.h"
#include <stdint.h>
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
    delete pSyncStrategy;
    pSyncStrategy = nullptr;
}

SyncClockManager::SyncState SyncClockManager::get_current_video_sync_state(
    double next_pts, AVRational & time_base, int serial, double * remaining_time, double current_time)
{
    return pSyncStrategy->get_current_video_sync_state(next_pts, time_base, serial, remaining_time, current_time);
}

SyncClockManager::SyncState SyncClockManager::get_current_audio_sync_state(
    double next_pts, AVRational & time_base, int serial, double * remaining_time, double current_time)
{
    return pSyncStrategy->get_current_audio_sync_state(next_pts, time_base, serial, remaining_time, current_time);
}

void SyncClockManager::seek(int64_t position)
{
    return pSyncStrategy->seek(position);
}

void SyncClockManager::pause() 
{
    pSyncStrategy->pause();
}

void SyncClockManager::resume()
{
    pSyncStrategy->resume();
}

int64_t SyncClockManager::get_current_position()
{
    return pSyncStrategy->get_current_position();
}

double SyncClockManager::get_video_pts()
{
    return pSyncStrategy->get_video_pts();
}

double SyncClockManager::get_audio_pts()
{
    return pSyncStrategy->get_video_pts();
}

int64_t SyncClockManager::get_video_position()
{
    return pSyncStrategy->get_video_position();
}

int64_t SyncClockManager::get_audio_position()
{
    return pSyncStrategy->get_audio_position();
}
