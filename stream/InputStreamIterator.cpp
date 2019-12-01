#include "InputStreamIterator.h"

InputStreamIterator::InputStreamIterator(AVFormatContext * format_context)
: pFormatContext(format_context),
mIndex(0)
{
    
}
InputStreamIterator::~InputStreamIterator()
{
}

const bool InputStreamIterator::has_next() const
{
    if (mIndex < pFormatContext->nb_streams - 1)
    {
        return true;
    }
    else  
    {
        return false;
    }
}

const AVStream * InputStreamIterator::next() const
{
    if (mIndex < pFormatContext->nb_streams - 1)
    {
        return pFormatContext->streams[mIndex];
    }
    //TODO  是否要抛出异常
    return NULL;
}