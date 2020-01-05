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
    T * get_empty_node();
    void recycle_node(T *node);
};


template <class T>
ConcurrentCachePool<T>::ConcurrentCachePool(int size)
    : mMaxSize(size),
      mCurrentSize(0),
      mCacheMutex(),
      mCacheCond()
{
}

template <class T>
ConcurrentCachePool<T>::~ConcurrentCachePool()
{
}

template <class T>
T * ConcurrentCachePool<T>::get_empty_node()
{
    T *node = NULL;
    std::unique_lock<std::mutex> cache_lock(mCacheMutex);
    
    if (mCacheQueue.size() > 0)
    {
        node = mCacheQueue.front();
        mCacheQueue.pop();
    }
    else
    {
        if (mMaxSize == mCurrentSize)
        {
            mCacheCond.wait(cache_lock, [this] () { return (mCacheQueue.size() > 0);});
            node = mCacheQueue.front();
            mCacheQueue.pop();
        }
        else
        {
            node = create_node();
            ++mCurrentSize;
        }
    }

    return node;
}

template <class T>
void ConcurrentCachePool<T>::recycle_node(T *node)
{
    std::lock_guard<std::mutex> cache_lock(mCacheMutex);
    mCacheQueue.push(node);
    mCacheCond.notify_all();
}

template <class T>
T * ConcurrentCachePool<T>::create_node()
{
    return new T();
}

template <class T>
void ConcurrentCachePool<T>::destroy_node(T * node)
{
    delete node;
}

#endif