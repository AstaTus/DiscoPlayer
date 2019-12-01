#ifndef __CONCURRENT_QUEUE_H__
#define __CONCURRENT_QUEUE_H__
#include <queue>
#include <pthread.h>

#include "../common/thread/Mutex.h"
#include "../common/thread/Cond.h"

using namespace std;

template <class T>
class ConcurrentQueue
{
private:
    Mutex mReadMutex;
    Cond mReadCond;

    Mutex mWriteMutex;
    Cond mWriteCond;

    queue<T *> mReadQueue;
    queue<T *> mWriteQueue;
    T *pCurrentConsumed;

    int mMaxSize;

protected:
    virtual T * create_node();
    virtual void destroy_node(T * node);

public:
    ConcurrentQueue(int init_size);
    ~ConcurrentQueue();

    //
    const T *get_empty_node();
    void recycle_node(const T *node);

    const T *pop_node();
    void push_node(const T *node);
};
#endif