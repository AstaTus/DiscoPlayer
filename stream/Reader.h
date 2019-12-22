#ifndef __READER_H__
#define __READER_H__

class AVPacket;
class Reader
{
public:
    virtual ~Reader();
    virtual int read(AVPacket *) const = 0;
};
#endif