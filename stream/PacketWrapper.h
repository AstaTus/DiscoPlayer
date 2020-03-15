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
    bool is_end;

    PacketWrapper()
        : packet(nullptr),
          serial(0),
          serial_start_time(0),
          is_end(false)
    {
        packet = new AVPacket();
    }

    ~PacketWrapper()
    {
        delete packet;
    }

    void recycle() 
    {
        av_packet_unref(packet);
    }

    bool is_key_packet()
    {
        return (packet->flags & AV_PKT_FLAG_KEY);
    }
};
#endif