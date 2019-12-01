#ifndef __COND_H__
#define __COND_H__
#include <pthread.h>

class Cond
{
private:
    pthread_cond_t mId;
    bool mIsValid;

    friend class CondProcessor;

    void reset();
public:
    Cond();
    ~Cond();
    
    bool is_valid();

    bool signal();

    bool broadcast();
};
#endif