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

double Clock::get_last_update_time()
{
    return mLastUpdateTime;
}
double Clock::get_last_duration()
{
    return mLastDuration;
}

double Clock::get_last_pts()
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

double Clock::get_transformed_last_pts()
{
    if (mRational.den == 0 && mRational.num == 0)
    {
        return mSeekPosition;
    } else {
        return av_q2d(mRational) * get_last_pts() * 1000;
    } 
}

double Clock::get_transformed_last_duration()
{
    if (mRational.den == 0 && mRational.num == 0)
    {
        return 0;
    } else {
        return av_q2d(mRational) * get_last_duration() * 1000;
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

