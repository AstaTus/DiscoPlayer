#include "ConcurrentCachePool.h"

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
T *const ConcurrentCachePool<T>::get_empty_node()
{
    T *node = NULL;
    std::lock_guard<std::mutex> cache_lock(mCacheMutex);
    
    if (mCacheQueue.size > 0)
    {
        node = mCacheQueue.pop();
    }
    else
    {
        if (mMaxSize == mCurrentSize)
        {
            mCacheCond.wait(cache_lock, [this] () { mCacheQueue.size > 0; });
            node = mCacheQueue.pop();
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
void ConcurrentCachePool<T>::recycle_node(const T *node)
{
    std::lock_guard<std::mutex> cache_lock(mCacheMutex);
    mCacheQueue.push(node);
    mCacheCond.notify_one();
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