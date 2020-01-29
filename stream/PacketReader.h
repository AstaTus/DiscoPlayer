#ifndef __PACKET_READER_H__
#define __PACKET_READER_H__
#include "Reader.h"
class AVFormatContext;

class PacketReader : public Reader
{
private:
    AVFormatContext *pFormatContext;
    const int * mpSerial;
    const int64_t * mpSerialStartTime;
public:
    PacketReader(AVFormatContext * format_context, 
            const int * serial, const int64_t * serial_start_time);
    virtual ~PacketReader();

    int read(PacketWrapper * packet_wrapper) const override;

    int serial() override;

    virtual int64_t serial_start_time() override;
};
#endif