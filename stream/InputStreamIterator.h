#ifndef __INPUT_STREAM_ITERATOR_H__
#define __INPUT_STREAM_ITERATOR_H__
#include "./IStreamIterator.h"
struct AVFormatContext;
struct AVStream;
class InputStreamIterator : public IStreamIterator
{
private:
    AVFormatContext * const * mppFormatContext;
    int mIndex;
public:
    InputStreamIterator(AVFormatContext * const * format_context);
    ~InputStreamIterator();

    AVStream * next();
    bool has_next() const;
};
#endif