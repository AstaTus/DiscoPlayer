#ifndef __SYNC_CLOCK_MANAGER_H__
#define __SYNC_CLOCK_MANAGER_H__
#include "Clock.h"

class SyncStrategy;

class SyncClockManager
{
private:
    SyncStrategy * pSyncStrategy;

public:

    enum SyncStrategy {
        SYNC_STRATEGY_AUDIO = 1,
        SYNC_STRATEGY_VIDEO = 2,
        SYNC_STRATEGY_NATURAL = 3
    };

    enum SyncState {
        SYNC_STATE_KEEP = 1,
        SYNC_STATE_NEXT = 2,
        SYNC_STATE_DROP = 3
    };

    SyncClockManager(SyncStrategy strategy);
    ~SyncClockManager();

    

    void update_system_time();

    SyncState get_current_video_sync_state(double next_pts, double * remaining_time);

    SyncState get_current_audio_sync_state(double next_pts, double * remaining_time);


};
#endif