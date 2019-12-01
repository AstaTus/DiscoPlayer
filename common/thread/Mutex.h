#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>
#include "CondProcessor.h"

class Mutex
{
private:
    pthread_mutex_t mId;
    bool mIsValid;

    friend class CondProcessor;
public:
    Mutex(/* args */);
    ~Mutex();

    bool lock();
    bool unlock();

    bool is_valid();
};

#endif