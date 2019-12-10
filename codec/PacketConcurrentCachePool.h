#ifndef __PACKET_CONCURRENT_CACHE_POOL_H__
#define __PACKET_CONCURRENT_CACHE_POOL_H__
#include "../common/cache/ConcurrentCachePool.h"

class AVPacket;
class PacketConcurrentCachePool: public ConcurrentCachePool<AVPacket>
{
private:
    /* data */


protected:
    AVPacket * create_node();
    void destroy_node(AVPacket * node);
    
public:
    PacketConcurrentCachePool(int size);
    ~PacketConcurrentCachePool();
};

#endif