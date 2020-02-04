#include "Semaphore.h"

Semaphore::Semaphore(int count)
:mCount(0),
mInitCount(count)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::signal()
{
    {
        std::unique_lock<std::mutex> semaphore_lock(mSemaphoreMutex);
        mCount--;
    }
    mSemaphoreCond.notify_all();
}
 
void Semaphore::wait()
{
    std::unique_lock<std::mutex> semaphore_lock(mSemaphoreMutex);
    mCount = mInitCount;
    mSemaphoreCond.wait(semaphore_lock, [this] () { return mCount == 0;});
    mCount = 0;
}