#include "Clock.h"

extern "C"
{
    #include "libavutil/time.h"
}

Clock::Clock(/* args */)
:mLastDuration(0.0),
mLastUpdateTime(0.0),
mLastPts(0.0),
mLastPauseDuration(0.0),
mIsPause(false),
mPauseStartTime(0.0)
{
}

Clock::~Clock()
{
}

double Clock::getLastUpdateTime()
{
    return mLastUpdateTime;
}
double Clock::getLastDuration()
{
    return mLastDuration;
}

double Clock::getLastPts()
{
    return mLastPts;
}

void Clock::setLastUpdateTime(double time)
{
    mLastUpdateTime = time;
}

void Clock::setLastDuration(double duration)
{
    mLastDuration = duration;
}

void Clock::setLastPts(double pts)
{
    mLastPts = pts;
}

double Clock::getTime(double speed)
{
    if (mIsPause) {
        return mLastPts + (mPauseStartTime - mLastUpdateTime - mLastPauseDuration) * speed;
    } else {
        double current_time = av_gettime_relative() / 1000000.0;
        return mLastPts + (current_time - mLastUpdateTime - mLastPauseDuration) * speed;
    }
}

double Clock::getMapingSystemTime(double speed)
{
    if (mIsPause) {
        return mLastUpdateTime + (mPauseStartTime - mLastUpdateTime - mLastPauseDuration) * speed;
    } else {
        double current_time = av_gettime_relative() / 1000000.0;
        return mLastUpdateTime + (current_time - mLastUpdateTime - mLastPauseDuration) * speed;
    }
}

void Clock::update(double time, double duration, double pts)
{
    mLastDuration = duration;
    mLastUpdateTime = time;
    mLastPts = pts;
    mLastPauseDuration = 0.0;
}

void Clock::pasue()
{
    if (!mIsPause) {
        mPauseStartTime = av_gettime_relative() / 1000000.0;
        mIsPause = true;
    }
    
}

void Clock::resume()
{
    if(mIsPause) {
        mLastPauseDuration +=  (av_gettime_relative() / 1000000.0 - mPauseStartTime);
        mIsPause = false;
    }
}

