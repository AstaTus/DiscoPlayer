#ifndef __FRAME_QUEUE_H__
#define __FRAME_QUEUE_H__

#include "ConcurrentQueue.h"

class AVFrame;

class FrameQueue: public ConcurrentQueue<AVFrame>
{
private:


protected:
    AVFrame * create_node();
    void destroy_node(AVFrame * node);
public:
    FrameQueue(int init_size);
    ~FrameQueue();
};
#endif