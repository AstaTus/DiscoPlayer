#ifndef __CLOCK_H__
#define __CLOCK_H__
class Clock
{
private:
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
public:
    Clock(/* args */);
    ~Clock();
};

Clock::Clock(/* args */)
{
}

Clock::~Clock()
{
}

#endif