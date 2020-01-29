#ifndef __PACKET_WRAPPER_H__
#define __PACKET_WRAPPER_H__

extern "C"
{
    #include "libavutil/avutil.h"
    #include "libavcodec/avcodec.h"
}

struct PacketWrapper
{
    AVPacket *packet;
    int serial;
    //转换过的时间点
    int64_t serial_start_time;

    PacketWrapper()
        : packet(nullptr),
          serial(0),
          serial_start_time(0)
    {
        packet = new AVPacket();
    }

    ~PacketWrapper()
    {
        delete packet;
    }
};
#endif