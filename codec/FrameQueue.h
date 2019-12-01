#ifndef __FRAME_QUEUE_H__
#define __FRAME_QUEUE_H__

#include "ConcurrentQueue.h"

class AVFrame;

class FrameQueue: public ConcurrentQueue<AVFrame>
{

protected:
    void destroy_node(AVFrame * node);
public:
    FrameQueue();
    ~FrameQueue();
};
#endif