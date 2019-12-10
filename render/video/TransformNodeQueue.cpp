#include "TransformNodeQueue.h"
#include "Image.h"

TransformNodeQueue::TransformNodeQueue(/* args */)
:ConcurrentQueue()
{
}

TransformNodeQueue::~TransformNodeQueue()
{
}

void TransformNodeQueue::destroy_node(TransformNode * node)
{
    // if (image != NULL)
    // {
    //     delete image;
    // }
}
