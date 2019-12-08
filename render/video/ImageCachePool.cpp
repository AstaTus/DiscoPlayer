#include "ImageCachePool.h"
#include "Image.h"

AVPacket* PacketConcurrentCachePool::create_node()
{
    return new Image();
}

void PacketConcurrentCachePool::destroy_node(Image * node)
{
    delete node;
}