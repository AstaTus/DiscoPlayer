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
    void destory_node(T * node);
public:
    ConcurrentQueue();
    virtual ~ConcurrentQueue();

    T * block_pop_node();
    T * block_peek_node();

    //non block 与 block 系列函数不能交叉调用，
    //用了non block  就不能用 block 系列
    T * non_block_pop_node();
    T * non_block_peek_node();

    void push_node(const T *node);
};
#endif