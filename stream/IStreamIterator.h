#ifndef __STREAM_ITERATOR_H__
#define __STREAM_ITERATOR_H__

struct AVStream;

class IStreamIterator
{
public:
    virtual AVStream * next() = 0;
    virtual bool has_next() const = 0;
};
#endif
