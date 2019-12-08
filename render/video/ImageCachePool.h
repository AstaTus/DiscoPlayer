#ifndef IMAGE_CONCURRENT_CACHE_POOL_H__
#define IMAGE_CONCURRENT_CACHE_POOL_H__

#include "../common/cache/ConcurrentCachePool.h"

class Image;

class ImageConcurrentCachePool: public ConcurrentCachePool<Image>
{
private:
    /* data */
public:
    ImageConcurrentCachePool(/* args */);
    ~ImageConcurrentCachePool();
};


#endif
