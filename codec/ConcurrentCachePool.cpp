#include "ConcurrentCachePool.h"

template <class T>
ConcurrentCachePool<T>::ConcurrentQueueNodeCachePool(int size)
    : mMaxSize(size),
      mCurrentSize(0)
{
}

template <class T>
ConcurrentCachePool<T>::~ConcurrentQueueNodeCachePool()
{
}

template <class T>
T * const ConcurrentCachePool<T>::get_empty_node()
{
    T *node = NULL;

    mCacheMutex.lock();
    if (mWriteQueue.size > 0)
    {
        node = mWriteQueue.pop();
    }
    else
    {
        if (mMaxSize == mCurrentSize)
        {
            CondProcessor::cond_wait(mCacheCond, mCacheMutex);
            if (mCacheQueue.size > 0)
            {
                node = mWriteQueue.pop();
            }
        }
        else
        {
            node = create_node();
            ++mCurrentSize;
        }
    }

    mCacheMutex.unlock();

    return node;
}

template <class T>
void ConcurrentCachePool<T>::recycle_node(const T *node)
{
    mCacheMutex.lock();
    mCacheQueue.push(node);
    mCacheMutex.unlock();
    mCacheCond.signal();
}
