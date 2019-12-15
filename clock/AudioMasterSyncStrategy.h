#ifndef __AUDIO_MASTER_SYNC_STRATEGY_H__
#define __AUDIO_MASTER_SYNC_STRATEGY_H__

#include "SyncStrategy.h"
/* */
class AudioMasterSyncStrategy : public SyncStrategy
{
private:
    double compute_delay();

public:
    AudioMasterSyncStrategy(/* args */);
    ~AudioMasterSyncStrategy();

    SyncClockManager::SyncState get_current_video_sync_state(double next_pts);

    SyncClockManager::SyncState get_current_audio_sync_state(double next_pts);
};

#endif