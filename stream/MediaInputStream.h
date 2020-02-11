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
    Reader * mpReader;
public:
    MediaInputStream(/* args */);
    ~MediaInputStream();

    bool open(const string& url);
    bool close();
    bool pause();

    int get_serial();
    int64_t get_serial_start_time();

    int64_t get_duration();
    IStreamIterator* get_stream_iterator() const;
    Reader* get_packet_reader();

    void seek(int64_t position);
};
#endif
