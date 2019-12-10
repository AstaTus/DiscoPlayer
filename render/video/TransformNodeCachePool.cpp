#include "TransformNodeCachePool.h"

#include "TransformNode.h"

TransformNode * TransformNodeCachePool::create_node()
{
    return new TransformNode();
}

void TransformNodeCachePool::destroy_node(TransformNode * node)
{
    delete node;
}