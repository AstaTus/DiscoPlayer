#include "ConcurrentQueue.h"

template <class T>
ConcurrentQueue<T>::ConcurrentQueue(int init_size)
    : mMaxSize(init_size),
      pCurrentConsumed(NULL)
{
}
template <class T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
    mWriteMutex.lock();
    while (mWriteQueue.size > 0)
    {
        destory_node(mWriteQueue.pop());
    }
    mWriteMutex.unlock();

    mReadMutex.lock();
    while (mReadQueue.size > 0)
    {
        destory_node(mReadQueue.pop());
    }
    mReadMutex.unlock();
}

template <class T>
const T *ConcurrentQueue<T>::get_empty_node()
{
    T *node = NULL;

    mWriteMutex.lock();
    if (mWriteQueue.size > 0)
    {
        node = mWriteQueue.pop();
    }
    else
    {
        bool is_full = false;
        mReadMutex.lock();
        is_full = (mMaxSize == mReadQueue.size);
        mReadMutex.unlock();
        if (is_full)
        {
            CondProcessor::cond_wait(mWriteCond, mWriteMutex);
            if (mWriteQueue.size > 0)
            {
                node = mWriteQueue.pop();
            }
        }
        else
        {
            node = create_node();
        }
    }

    mWriteMutex.unlock();

    return node;
}

template <class T>
void ConcurrentQueue<T>::push_node(const T *node)
{
    mReadMutex.lock();
    mReadQueue.push(node);
    mReadMutex.unlock();
    mReadCond.signal();
}
template <class T>
void ConcurrentQueue<T>::recycle_node(const T *node)
{
    mWriteMutex.lock();
    mWriteQueue.push(node);
    mWriteMutex.unlock();
    mWriteCond.signal();
}

template <class T>
const T *ConcurrentQueue<T>::pop_node()
{
    T *node = NULL;

    mReadMutex.lock();
    if (mReadQueue.size > 0)
    {
        node = mReadQueue.pop();
    }
    else
    {
        bool is_full = false;
        mWriteMutex.lock();
        is_full = (mMaxSize == mWriteQueue.size);
        mWriteMutex.unlock();
        if (is_full)
        {
            CondProcessor::cond_wait(mReadCond, mReadMutex);
            if (mReadQueue.size > 0)
            {
                node = mReadQueue.pop();
            }
        }
        else
        {
            node = create_node();
        }
    }

    mReadMutex.unlock();

    return node;
}