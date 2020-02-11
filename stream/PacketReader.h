#ifndef __PACKET_READER_H__
#define __PACKET_READER_H__
#include "Reader.h"
#include <mutex>
struct AVFormatContext;

class PacketReader : public Reader
{
private:
    AVFormatContext * const * mppFormatContext;
    int mSerial;
    int64_t mSerialStartTime;
    std::mutex mSeekMutex;
public:
    PacketReader(AVFormatContext * const * format_context);
    virtual ~PacketReader();

    int read(PacketWrapper * packet_wrapper) override;

    int serial() override;

    virtual int64_t serial_start_time() override;

    virtual void seek(int64_t position) override;
};
#endif