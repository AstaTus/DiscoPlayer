#include "PacketReader.h"
extern "C"
{
#include "libavformat/avformat.h"
}
#include "PacketWrapper.h"
#include "../common/log/Log.h"

PacketReader::PacketReader(AVFormatContext *const *format_context)
    : mppFormatContext(format_context),
      mSerial(0),
      mSerialStartTime(0)
{
}

PacketReader::~PacketReader()
{
}

int PacketReader::read(PacketWrapper *packet_wrapper)
{
    std::lock_guard<std::mutex> seek_lock(mSeekMutex);
    packet_wrapper->serial = mSerial;
    packet_wrapper->serial_start_time = mSerialStartTime;
    return av_read_frame(*mppFormatContext, packet_wrapper->packet);
}

int PacketReader::serial()
{
    return mSerial;
}

int64_t PacketReader::serial_start_time()
{
    return mSerialStartTime;
}

void PacketReader::seek(int64_t position)
{
    std::lock_guard<std::mutex> seek_lock(mSeekMutex);
    Log::get_instance().log_error("[Disco]PacketReader::seek start\n");

    int64_t start_timestamp = (*mppFormatContext)->start_time == AV_NOPTS_VALUE ? 0 : (*mppFormatContext)->start_time;
    
    // pFormatContext->streams[i]->timesc
    int64_t timestamp = start_timestamp + av_rescale(position, AV_TIME_BASE, 1000);
    int ret = av_seek_frame((*mppFormatContext), -1, timestamp, AVSEEK_FLAG_BACKWARD);
    if (ret < 0)
    {
        Log::get_instance().log_error("[Disco]PacketReader::seek error ret = %d\n", ret);
    }

    mSerial++;
    mSerialStartTime = position;
    Log::get_instance().log_error("[Disco]PacketReader::seek end\n");
}