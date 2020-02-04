#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include <condition_variable>
#include <mutex>

class Semaphore
{
private:
    std::mutex mSemaphoreMutex;
    std::condition_variable mSemaphoreCond;
    //针对多个线程
    int mCount;
    int mInitCount;
public:
    Semaphore(int count = 1);
    ~Semaphore();
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    void wait();

    void signal();
};


#endif