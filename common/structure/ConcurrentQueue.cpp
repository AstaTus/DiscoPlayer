#include "ConcurrentQueue.h"

template <class T>
ConcurrentQueue<T>::ConcurrentQueue()
{
}

template <class T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    while (mQueue.size > 0)
    {
        destroy_node(mQueue.pop());
    }
}

template <class T>
void ConcurrentQueue<T>::push_node(const T *node)
{
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    mQueue.push(node);
    mQueueCond.notify_one();
}

template <class T>
T * ConcurrentQueue<T>::block_pop_node()
{
    T *node = NULL;
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    if (mQueue.size > 0)
    {
        node = mQueue.pop();
    }
    else
    {
        mQueueCond.wait(queue_lock, [this] () { mQueue.size > 0; });
        node = mQueue.pop();
    }

    return node;
}


template <class T>
T *ConcurrentQueue<T>::block_peek_node()
{
    T *node = nullptr;
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    if (mQueue.size > 0)
    {
        node = mQueue.front();
    }
    else
    {
        mQueueCond.wait(queue_lock, [this] () { mQueue.size > 0; });
        node = mQueue.front();
    }

    return node;
}


template <class T>
T *ConcurrentQueue<T>::non_block_pop_node()
{
    T *node = NULL;
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    if (mQueue.size > 0)
    {
        node = mQueue.pop();
    }

    return node;
}

template <class T>
T *ConcurrentQueue<T>::non_block_peek_node()
{
    T *node = NULL;

    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    if (mQueue.size > 0)
    {
        node = mQueue.front();
    }

    return node;
}

template <class T>
void ConcurrentQueue<T>::destory_node(T * node)
{
    delete node;
}