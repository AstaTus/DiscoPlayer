#include "MediaInputStream.h"
#include "InputStreamIterator.h"
#include "PacketReader.h"
#include "../common/log/Log.h"
extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

MediaInputStream::MediaInputStream(/* args */)
:pFormatContext(nullptr),
mpReader(nullptr),
mpInputStreamIterator(nullptr)
{
}

MediaInputStream::~MediaInputStream()
{
    close();
    delete mpReader;
    delete mpInputStreamIterator;
    mpInputStreamIterator = nullptr;
    mpReader = nullptr;
}

IStreamIterator* MediaInputStream::get_stream_iterator()
{
    if (mpInputStreamIterator == nullptr)
    {
       mpInputStreamIterator = new InputStreamIterator(&pFormatContext);
    }

    return mpInputStreamIterator;
}

Reader* MediaInputStream::get_packet_reader()
{
    if (mpReader == nullptr)
    {
        mpReader = new PacketReader(&pFormatContext);
    }

    return mpReader;
}

bool MediaInputStream::open(const string& url) 
{
    if (pFormatContext != nullptr)
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
    if (pFormatContext != nullptr)
    {
        avformat_close_input(&pFormatContext);
        pFormatContext = nullptr;
    }
    
    return true;
}
bool MediaInputStream::pause()
{
    return true;
}

int64_t MediaInputStream::get_duration()
{
    if (pFormatContext != nullptr)
    {
        return pFormatContext->duration / 1000;
    }

    return 0;
}

void MediaInputStream::seek(int64_t position)
{
    if (mpReader != nullptr)
    {
        mpReader->seek(position);
    }
}

int MediaInputStream::get_serial()
{
    return mpReader->serial();
}

int64_t MediaInputStream::get_serial_start_time()
{
    return mpReader->serial_start_time();
}