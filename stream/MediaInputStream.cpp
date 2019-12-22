#include "MediaInputStream.h"
#include "InputStreamIterator.h"
#include "PacketReader.h"
MediaInputStream::MediaInputStream(/* args */)
:pFormatContext(NULL)
{
}

MediaInputStream::~MediaInputStream()
{
    close();
}

IStreamIterator* MediaInputStream::get_stream_iterator() const
{
    InputStreamIterator * stream_iterator = new InputStreamIterator(pFormatContext);
    return stream_iterator;
}

Reader* MediaInputStream::get_packet_reader() const
{
    Reader * reader = new PacketReader(pFormatContext);
    return reader;
}

bool MediaInputStream::open(const string& url) 
{
    if (pFormatContext != NULL)
    {
        return false;
    }
    
    pFormatContext = avformat_alloc_context();
    if (avformat_open_input(&pFormatContext, url.c_str(), 0, 0) != 0){
		return false;
	}

	if (avformat_find_stream_info(pFormatContext, 0) < 0){
		return false;
	}

    return true;
}

bool MediaInputStream::close()
{
    return true;
}
bool MediaInputStream::pause()
{
    return true;
}