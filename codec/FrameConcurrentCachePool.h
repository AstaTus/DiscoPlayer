#ifndef FRAME_CONCURRENT_CACHE_POOL_H__
#define FRAME_CONCURRENT_CACHE_POOL_H__

#include "../common/cache/ConcurrentCachePool.h"

class AVFrame;

class FrameConcurrentCachePool: public ConcurrentCachePool<AVFrame>
{
private:
    /* data */
public:
    FrameConcurrentCachePool(/* args */);
    ~FrameConcurrentCachePool();
};


#endif
