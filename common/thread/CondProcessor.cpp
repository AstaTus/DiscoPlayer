#include "CondProcessor.h"
#include "Mutex.h"
#include "Cond.h"
#include <pthread.h>
#include <time.h>

extern "C"
{
    // #include "libavcodec/avcodec.h"
    // #include "libavformat/avformat.h"
    // #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

int CondProcessor::cond_wait(Cond &cond, const Mutex &mutex)
{
    cond.reset();
    return pthread_cond_wait(&cond.mId, &mutex.mId);
}

int CondProcessor::cond_wait_timeout(Cond &cond, const Mutex &mutex, uint32_t millisecond)
{
    int retval;
    //获取时间
    struct timespec outtime;
    clock_gettime(CLOCK_MONOTONIC, &outtime);
    //ms为毫秒，换算成秒
    outtime.tv_sec += millisecond / 1000;
    
    //在outtime的基础上，增加ms毫秒
    //outtime.tv_nsec为纳秒，1微秒=1000纳秒
    //tv_nsec此值再加上剩余的毫秒数 ms%1000，有可能超过1秒。需要特殊处理
    uint64_t  us = outtime.tv_nsec/1000 + 1000 * (millisecond % 1000); //微秒
    //us的值有可能超过1秒，
    outtime.tv_sec += us / 1000000; 

    us = us % 1000000;
    outtime.tv_nsec = us * 1000;//换算成纳秒

    cond.reset();
    while (1) {
        retval = pthread_cond_timedwait(&cond.mId, &mutex.mId, &outtime);
        if (retval == 0)
            return 0; 
        else if (retval == EINTR)
            continue;
        else if (retval == ETIMEDOUT)
        {
            av_log(NULL, AV_LOG_FATAL, "[player]:MutualProcessor::pthread_cond_timedwait fatal");
            return SDL_MUTEX_TIMEDOUT;
        }
        else
            break;
    }
    av_log(NULL, AV_LOG_FATAL, "[player]:MutualProcessor::pthread_cond_timedwait fatal");
    return -1;
}