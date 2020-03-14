#include "ThreadUtils.h"
#include <pthread.h>

ThreadUtils::ThreadUtils(/* args */)
{
}

ThreadUtils::~ThreadUtils()
{
}


void ThreadUtils::set_thread_name(const char * name)
{
#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        
        pthread_setname_np(name);
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    // android
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif
}

bool ThreadUtils::set_thread_prority(int prority)
{
    struct sched_param param;
    param.sched_priority = 99;         
    int policy = SCHED_FIFO;
    int ret = pthread_setschedparam(pthread_self(), policy, &param);
    if (ret != 0) {
        return false;
    }
    return true;
}