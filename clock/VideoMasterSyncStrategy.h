#ifndef __VIDEO_MASTER_SYNC_STRATEGY_H__
#define __VIDEO_MASTER_SYNC_STRATEGY_H__

#include "SyncStrategy.h"
/* */
class VideoMasterSyncStrategy: public SyncStrategy
{
private:

public:
    VideoMasterSyncStrategy(/* args */);
    virtual ~VideoMasterSyncStrategy();


    SyncClockManager::SyncState get_current_video_sync_state(double next_pts, AVRational & time_base, int serial, double * remaining_time) override;

    SyncClockManager::SyncState get_current_audio_sync_state(double next_pts, AVRational & time_base, int serial, double * remaining_time) override;

    void seek(uint64_t postion) override;
    
    void resume() override;

    void pause() override;

    int64_t get_current_position() override;

    double get_video_pts() override;

    double get_audio_pts() override;

    int64_t get_video_position() override;

    int64_t get_audio_position() override;
};

#endif