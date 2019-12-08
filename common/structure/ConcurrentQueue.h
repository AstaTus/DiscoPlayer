#ifndef __CONCURRENT_QUEUE_H__
#define __CONCURRENT_QUEUE_H__
#include <queue>

#include "../thread/Mutex.h"
#include "../thread/Cond.h"

using namespace std;

template <class T>
class ConcurrentQueue
{
private:
    Mutex mQueueMutex;
    Cond mQueueCond;

    queue<T *> mQueue;

protected:
    virtual void destory_node(T * node) = 0;
public:
    ConcurrentQueue();
    ~ConcurrentQueue();

    const T *pop_node();
    void push_node(const T *node);
};
#endif