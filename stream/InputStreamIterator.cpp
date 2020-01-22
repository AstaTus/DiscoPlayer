#include "InputStreamIterator.h"
extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

InputStreamIterator::InputStreamIterator(AVFormatContext * format_context)
: pFormatContext(format_context),
mIndex(0)
{
    
}
InputStreamIterator::~InputStreamIterator()
{
}

bool InputStreamIterator::has_next() const
{
    if (mIndex < pFormatContext->nb_streams)
    {
        return true;
    }
    else  
    {
        return false;
    }
}

AVStream * InputStreamIterator::next()
{
    if (mIndex < pFormatContext->nb_streams)
    {
        return pFormatContext->streams[mIndex++];
    }
    //TODO  是否要抛出异常
    return nullptr;
}