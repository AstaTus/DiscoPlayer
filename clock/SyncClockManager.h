#ifndef __SYNC_CLOCK_MANAGER_H__
#define __SYNC_CLOCK_MANAGER_H__
#include "Clock.h"
extern "C"
{
    #include "libavutil/time.h"
}
#include <stdint.h>
class SyncStrategy;
struct AVRational;
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

    enum class SyncState {
        SYNC_STATE_KEEP = 1,
        SYNC_STATE_NEXT = 2,
        SYNC_STATE_DROP = 3
    };

    SyncClockManager(SyncStrategy strategy);
    ~SyncClockManager();

    SyncState get_current_video_sync_state(
        double next_pts, AVRational & time_base, int serial, long long * remaining_time, 
        double current_time = av_gettime_relative() / 1000000.0);

    SyncState get_current_audio_sync_state(
        double next_pts, AVRational & time_base, int serial, long long * remaining_time,
        double current_time = av_gettime_relative() / 1000000.0);

    void seek(int64_t position);
    
    void pause();

    void resume();

    int64_t get_current_position();

    double get_video_pts();

    double get_audio_pts();

    int64_t get_video_position();

    int64_t get_audio_position();

    float get_speed();

    void set_speed(float speed);
};
#endif