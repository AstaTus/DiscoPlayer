#include "PacketQueue.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
	// #include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

PacketQueue::PacketQueue()
{

}

PacketQueue::~PacketQueue()
{
}

void PacketQueue::destroy_node(AVPacket * node)
{
    av_packet_unref(node);
}


