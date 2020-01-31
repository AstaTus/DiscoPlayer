#include "Semaphore.h"

Semaphore::Semaphore(/* args */)
:mFlag(0)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::signal()
{
    {
        std::unique_lock<std::mutex> semaphore_lock(mSemaphoreMutex);
        mFlag = 1;
    }
    mSemaphoreCond.notify_one();
}
 
void Semaphore::wait()
{
    std::unique_lock<std::mutex> semaphore_lock(mSemaphoreMutex);
    mSemaphoreCond.wait(semaphore_lock, [this] () { return mFlag == 1;});
    mFlag = 0;
}