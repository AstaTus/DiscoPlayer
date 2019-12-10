#ifndef __MEDIA_INPUT_STREAM_H__
#define __MEDIA_INPUT_STREAM_H__

#include "IStreamIterator.h"
#include "IReader.h"
#include <string>
using namespace std;

extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

class MediaInputStream
{
private:
    AVFormatContext	*pFormatContext;
public:
    MediaInputStream(/* args */);
    ~MediaInputStream();

    bool open(const string& url);
    bool close();
    bool pause();
    IStreamIterator* get_stream_iterator() const;
    IReader* get_packet_reader() const;
};
#endif
