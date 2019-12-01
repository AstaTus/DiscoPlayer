#ifndef __PACKET_READER_H__
#define __PACKET_READER_H__
#include "IReader.h"
class AVFormatContext;

class PacketReader : public IReader
{
private:
    AVFormatContext *pFormatContext;
public:
    PacketReader(AVFormatContext * format_context);
    ~PacketReader();

    int read(AVPacket * packet) const;
};
#endif