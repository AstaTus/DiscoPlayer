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
    virtual ~AudioMasterSyncStrategy();

    virtual SyncClockManager::SyncState get_current_video_sync_state(double next_pts, double * remaining_time) override;

    virtual SyncClockManager::SyncState get_current_audio_sync_state(double next_pts, double * remaining_time) override;
};

#endif