#ifndef __PACKET_QUEUE_H__
#define __PACKET_QUEUE_H__
#include "../common/structure/ConcurrentQueue.h"

class AVPacket;

class PacketQueue: public ConcurrentQueue<AVPacket>
{
private:


protected:
    void destroy_node(AVPacket * node);
public:
    PacketQueue();
    ~PacketQueue();
};
#endif
