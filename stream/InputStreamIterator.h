#ifndef __INPUT_STREAM_ITERATOR_H__
#define __INPUT_STREAM_ITERATOR_H__
#include "./IStreamIterator.h"
class InputStreamIterator : public IStreamIterator
{
private:
    AVFormatContext *pFormatContext;
    int mIndex;
public:
    InputStreamIterator(AVFormatContext * format_context);
    ~InputStreamIterator();

    AVStream * next();
    bool has_next() const;
};
#endif