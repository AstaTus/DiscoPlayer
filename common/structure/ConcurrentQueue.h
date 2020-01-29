#ifndef __CONCURRENT_QUEUE_H__
#define __CONCURRENT_QUEUE_H__
#include <queue>
#include <condition_variable>
template <class T>
class ConcurrentQueue
{
private:
    std::mutex mQueueMutex;
    std::condition_variable mQueueCond;
    std::queue<T *> mQueue;

protected:
    void destroy_node(T * node);
public:
    ConcurrentQueue();
    virtual ~ConcurrentQueue();

    T * block_pop_node();
    T * block_peek_node();

    //non block 与 block 系列函数不能交叉调用，
    //用了non block  就不能用 block 系列
    T * non_block_pop_node();
    T * non_block_peek_node();

    void push_node(T *node);
};


template <class T>
ConcurrentQueue<T>::ConcurrentQueue()
{
}

template <class T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    while (mQueue.size() > 0)
    {
        destroy_node(mQueue.front());
        mQueue.pop();
    }
}

template <class T>
void ConcurrentQueue<T>::push_node(T *node)
{
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    mQueue.push(node);
    mQueueCond.notify_all();
}

template <class T>
T * ConcurrentQueue<T>::block_pop_node()
{
    T *node = NULL;
    std::unique_lock<std::mutex> queue_lock(mQueueMutex);
    if (mQueue.size() > 0)
    {
        node = mQueue.front();
        mQueue.pop();
    }
    else
    {
        mQueueCond.wait(queue_lock, [this] () { return mQueue.size() > 0;});
        node = mQueue.front();
        mQueue.pop();
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
    T *node = nullptr;
    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    if (mQueue.size() > 0)
    {
        node = mQueue.front();
        mQueue.pop();
    }

    return node;
}

template <class T>
T *ConcurrentQueue<T>::non_block_peek_node()
{
    T *node = NULL;

    std::lock_guard<std::mutex> queue_lock(mQueueMutex);
    if (mQueue.size() > 0)
    {
        node = mQueue.front();
    }

    return node;
}

template <class T>
void ConcurrentQueue<T>::destroy_node(T * node)
{
    delete node;
}

#endif