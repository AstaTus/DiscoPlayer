#ifndef __MEDIA_INPUT_STREAM_H__
#define __MEDIA_INPUT_STREAM_H__

#include "IStreamIterator.h"
#include "Reader.h"
#include <string>
using namespace std;


struct AVFormatContext;
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
    Reader* get_packet_reader() const;
};
#endif
