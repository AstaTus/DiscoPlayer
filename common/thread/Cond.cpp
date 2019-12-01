#include "Cond.h"

extern "C"
{
    // #include "libavcodec/avcodec.h"
	// #include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

Cond::Cond()
:mIsValid(false)
{
    
}

Cond::~Cond()
{
    if (is_valid())
    {
        pthread_cond_destroy(&mId);
    }
}

void Cond::reset()
{
    if (pthread_cond_init(&mId, NULL) != 0) 
    {
        mIsValid = false;
        av_log(this, AV_LOG_FATAL, "[player]:Cond::Cond init fatal"); 
    } else {
        mIsValid = true;
    }
}

bool Cond::is_valid()
{
    return mIsValid;
}
