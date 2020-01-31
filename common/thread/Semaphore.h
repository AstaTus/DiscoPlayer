#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include <condition_variable>
#include <mutex>

class Semaphore
{
private:
    std::mutex mSemaphoreMutex;
    std::condition_variable mSemaphoreCond;
    int mFlag;
public:
    Semaphore(/* args */);
    ~Semaphore();
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    void wait();

    void signal();
};


#endif