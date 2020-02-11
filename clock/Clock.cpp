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
mRational(),
mSeekPosition(0),
mLastSerial(-1)
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

void Clock::update(double time, double pts, AVRational & time_base, int serial)
{
    if (mLastSerial != serial)
    {
        mLastDuration = 0;
    } else {
        mLastDuration = pts - mLastPts;
    }

    mLastUpdateTime = time;
    mLastPts = pts;
    mLastSerial = serial;
    mLastPauseDuration = 0.0;
    mRational.den = time_base.den;
    mRational.num = time_base.num;
    mSeekPosition = 0;
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
    if (mRational.den == 0 && mRational.num == 0)
    {
        return mSeekPosition;
    } else {
        return av_q2d(mRational) * getLastPts() * 1000;
    } 
}

int64_t Clock::getTransformedLastDuration()
{
    if (mRational.den == 0 && mRational.num == 0)
    {
        return 0;
    } else {
        return av_q2d(mRational) * getLastDuration() * 1000;
    }
}

void Clock::seek(int64_t time, int64_t position)
{
    mSeekPosition = position;
    mLastUpdateTime = time;
    //TODO mLastPts 赋值
    // mLastPts = position;
    mLastDuration = 0;
    mRational.den = 0;
    mRational.num = 0;
    mLastSerial = -1;
}

