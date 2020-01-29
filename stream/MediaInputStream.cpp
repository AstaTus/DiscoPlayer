#include "MediaInputStream.h"
#include "InputStreamIterator.h"
#include "PacketReader.h"

extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

MediaInputStream::MediaInputStream(/* args */)
:pFormatContext(nullptr),
mSerial(0),
mSerialStartTime(0)
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
    Reader * reader = new PacketReader(pFormatContext, &mSerial, &mSerialStartTime);
    return reader;
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
    int64_t start_timestamp = pFormatContext->start_time == AV_NOPTS_VALUE ? 0 : pFormatContext->start_time;
    for (int i = 0; i < pFormatContext->nb_streams; i++)
    {
        // pFormatContext->streams[i]->timesc
        int64_t timestamp = start_timestamp + position / 1000 / av_q2d(pFormatContext->streams[i]->time_base);
        int ret = av_seek_frame(pFormatContext, i, timestamp, AVSEEK_FLAG_BACKWARD); 
        if (ret < 0) {
            // ERR("mod:%d, %s: could not seek to position %d\n", handle->id,
            //     handle->params.path, handle->params.seekoffset);
        }
    }
    mSerial++;
    mSerialStartTime = position;
}

int MediaInputStream::get_serial()
{
    return mSerial;
}

int64_t MediaInputStream::get_serial_start_time()
{
    return mSerialStartTime;
}