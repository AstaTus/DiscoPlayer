#include "PacketConcurrentCachePool.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
	// #include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

PacketConcurrentCachePool::PacketConcurrentCachePool(int size)
:ConcurrentCachePool(size)
{
}

PacketConcurrentCachePool::~PacketConcurrentCachePool()
{
}

AVPacket* PacketConcurrentCachePool::create_node()
{
    return (AVPacket *)av_malloc(sizeof(AVPacket));
}

void PacketConcurrentCachePool::destroy_node(AVPacket * node)
{
    av_packet_unref(node);
}
