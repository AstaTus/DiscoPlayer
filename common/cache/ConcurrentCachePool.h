#ifndef __CONCURRENT_QUEUE_NODE_CACHE_POOL_H__
#define __CONCURRENT_QUEUE_NODE_CACHE_POOL_H__

#include <queue>
#include <condition_variable>

template <class T>
class ConcurrentCachePool
{
private:
    //最大的size
    int mMaxSize;
    //当前创建了几个node
    int mCurrentSize;
    std::condition_variable mCacheCond;
    std::mutex mCacheMutex;

    std::queue<T *> mCacheQueue;

protected:
    T * create_node();
    void destroy_node(T * node);
    
public:
    ConcurrentCachePool(int size);
    ~ConcurrentCachePool();

        //
    T * const get_empty_node();
    void recycle_node(const T *node);
};

#endif