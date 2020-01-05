#ifndef __STREAM_ITERATOR_H__
#define __STREAM_ITERATOR_H__
extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

class IStreamIterator
{
public:
    virtual AVStream * next() = 0;
    virtual bool has_next() const = 0;
};
#endif
