#ifndef __IMAGE_QUEUE_H__
#define __IMAGE_QUEUE_H__

#include "../../common/structure/ConcurrentQueue.h"
#include "TransformNode.h"

class TransformNodeQueue : public ConcurrentQueue<TransformNode>
{
private:
    /* data */

protected:
    void destroy_node(TransformNode * image);
public:
    TransformNodeQueue(/* args */);
    ~TransformNodeQueue();
};
#endif