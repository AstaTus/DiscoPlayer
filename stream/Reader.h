#ifndef __READER_H__
#define __READER_H__

#include <stdint.h>
struct PacketWrapper;
class Reader
{
public:
    virtual ~Reader();
    virtual int read(PacketWrapper *) const = 0;

    virtual int serial() = 0;

    virtual int64_t serial_start_time() = 0;
};
#endif