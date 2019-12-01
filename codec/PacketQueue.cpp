#include "PacketQueue.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
	// #include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

PacketQueue::PacketQueue(int init_size)
:ConcurrentQueue(init_size)
{

}

PacketQueue::~PacketQueue()
{
}

AVPacket* PacketQueue::create_node()
{
    return (AVPacket *)av_malloc(sizeof(AVPacket));
}

void PacketQueue::destroy_node(AVPacket * node)
{
    av_packet_unref(node);
}

