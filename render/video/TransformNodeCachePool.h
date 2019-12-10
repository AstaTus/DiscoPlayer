#ifndef __TRANSFORM_NODE_CACHE_POOL_H__
#define __TRANSFORM_NODE_CACHE_POOL_H__

#include "../../common/cache/ConcurrentCachePool.h"

class TransformNode;

class TransformNodeCachePool: public ConcurrentCachePool<TransformNode>
{
private:
    /* data */


protected:
    TransformNode * create_node();
    void destroy_node(TransformNode * node);
};


#endif
