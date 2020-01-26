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
mPauseStartTime(0.0),
mRational()
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

void Clock::update(double time, double duration, double pts, AVRational & time_base)
{
    mLastDuration = duration;
    mLastUpdateTime = time;
    mLastPts = pts;
    mLastPauseDuration = 0.0;
    mRational.den = time_base.den;
    mRational.num = time_base.num;
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

int64_t Clock::getTransformedLastPts()
{
    return av_q2d(mRational) * getLastPts();
}

