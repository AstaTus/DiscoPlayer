#ifndef __PACKET_READER_H__
#define __PACKET_READER_H__
#include "Reader.h"
class AVFormatContext;

class PacketReader : public Reader
{
private:
    AVFormatContext *pFormatContext;
public:
    PacketReader(AVFormatContext * format_context);
    virtual ~PacketReader();

    int read(AVPacket * packet) const;
};
#endif