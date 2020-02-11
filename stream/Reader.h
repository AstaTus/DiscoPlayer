#ifndef __READER_H__
#define __READER_H__

#include <stdint.h>
struct PacketWrapper;
class Reader
{
public:
    virtual ~Reader();
    virtual int read(PacketWrapper *) = 0;

    virtual int serial() = 0;

    virtual int64_t serial_start_time() = 0;

    virtual void seek(int64_t position) = 0;
};
#endif