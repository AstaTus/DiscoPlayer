#ifndef __IREADER_H__
#define __IREADER_H__

class AVPacket;
class IReader
{
public:
    virtual int read(AVPacket *) const;
};
#endif