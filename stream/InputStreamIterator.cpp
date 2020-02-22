#include "InputStreamIterator.h"
extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

InputStreamIterator::InputStreamIterator(AVFormatContext * const * format_context)
: mppFormatContext(format_context),
mIndex(0)
{
    
}
InputStreamIterator::~InputStreamIterator()
{
    mppFormatContext = nullptr;
}

bool InputStreamIterator::has_next() const
{
    if (mIndex < (*mppFormatContext)->nb_streams)
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
    if (mIndex < (*mppFormatContext)->nb_streams)
    {
        return (*mppFormatContext)->streams[mIndex++];
    }
    //TODO  是否要抛出异常
    return nullptr;
}