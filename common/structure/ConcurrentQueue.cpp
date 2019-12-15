#include "ConcurrentQueue.h"

template <class T>
ConcurrentQueue<T>::ConcurrentQueue()
{
}

template <class T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
    mQueueMutex.lock();
    while (mQueue.size > 0)
    {
        destroy_node(mQueue.pop());
    }
    mQueueMutex.unlock();
}

template <class T>
void ConcurrentQueue<T>::push_node(const T *node)
{
    mQueueMutex.lock();
    mQueue.push(node);
    mQueueMutex.unlock();
    mQueueCond.signal();
}

template <class T>
T * ConcurrentQueue<T>::block_pop_node()
{
    T *node = NULL;

    mQueueMutex.lock();
    if (mQueue.size > 0)
    {
        node = mQueue.pop();
    }
    else
    {
        CondProcessor::cond_wait(mQueueCond, mQueueMutex);
        if (mQueue.size > 0)
        {
            node = mQueue.pop();
        }
    }

    mQueueMutex.unlock();

    return node;
}


template <class T>
T *ConcurrentQueue<T>::block_peek_node()
{
    T *node = NULL;

    mQueueMutex.lock();
    if (mQueue.size > 0)
    {
        node = mQueue.front();
    }
    else
    {
        CondProcessor::cond_wait(mQueueCond, mQueueMutex);
        if (mQueue.size > 0)
        {
            node = mQueue.front();
        }
    }

    mQueueMutex.unlock();

    return node;
}


template <class T>
T *ConcurrentQueue<T>::non_block_pop_node()
{
    T *node = NULL;

    mQueueMutex.lock();
    if (mQueue.size > 0)
    {
        node = mQueue.pop();
    }

    mQueueMutex.unlock();

    return node;
}

template <class T>
T *ConcurrentQueue<T>::non_block_peek_node()
{
    T *node = NULL;

    mQueueMutex.lock();
    if (mQueue.size > 0)
    {
        node = mQueue.pop();
    }

    mQueueMutex.unlock();

    return node;
}