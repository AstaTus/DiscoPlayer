#ifndef __VIDEO_MASTER_SYNC_STRATEGY_H__
#define __VIDEO_MASTER_SYNC_STRATEGY_H__

#include "SyncStrategy.h"
/* */
class VideoMasterSyncStrategy: public SyncStrategy
{
private:

public:
    VideoMasterSyncStrategy(/* args */);
    ~VideoMasterSyncStrategy();


    SyncClockManager::SyncState get_current_video_sync_state(double next_pts, double * remaining_time);

    SyncClockManager::SyncState get_current_audio_sync_state(double next_pts, double * remaining_time);
};

#endif