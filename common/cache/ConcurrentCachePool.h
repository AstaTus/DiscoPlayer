#ifndef __CONCURRENT_QUEUE_NODE_CACHE_POOL_H__
#define __CONCURRENT_QUEUE_NODE_CACHE_POOL_H__

#include <queue>
#include <pthread.h>

#include "../thread/Mutex.h"
#include "../thread/Cond.h"

using namespace std;

template <class T>
class ConcurrentCachePool
{
private:
    //最大的size
    int mMaxSize;
    //当前创建了几个node
    int mCurrentSize;

    Mutex mCacheMutex;
    Cond mCacheCond;

    queue<T *> mCacheQueue;

protected:
    virtual T * create_node();
    virtual void destroy_node(T * node);
    
public:
    ConcurrentCachePool(int size);
    ~ConcurrentCachePool();

        //
    T * const get_empty_node();
    void recycle_node(const T *node);
};

#endif