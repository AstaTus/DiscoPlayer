#include "Mutex.h"

extern "C"
{
// #include "libavcodec/avcodec.h"
// #include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

Mutex::Mutex(/* args */)
{
    if (pthread_mutex_init(&mId, NULL) != 0)
    {
        av_log(this, AV_LOG_FATAL, "[player]:Mutex::Mutex init fatal");
    }
    else
    {
        mIsValid = true;
    }
}

Mutex::~Mutex()
{
    if (is_valid())
    {
        pthread_mutex_destroy(&mId);
    }
}

bool Mutex::is_valid()
{
    return mIsValid;
}

bool Mutex::lock()
{
    if (pthread_mutex_lock(&mId) != 0)
    {
        av_log(this, AV_LOG_FATAL, "[player]:Mutex::Mutex lock fatal");
        return false;
    }

    return true;
}

bool Mutex::unlock()
{
    if (pthread_mutex_unlock(&mId) != 0)
    {
        av_log(this, AV_LOG_FATAL, "[player]:Mutex::Mutex unlock fatal");
        return false;
    }

    return true;
}
