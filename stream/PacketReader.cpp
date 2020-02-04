#include "PacketReader.h"
extern "C"
{
	#include "libavformat/avformat.h"
}
#include "PacketWrapper.h"
PacketReader::PacketReader(AVFormatContext * const * format_context,
                         const int * serial, const int64_t * serial_start_time)
:mppFormatContext(format_context),
mpSerial(serial),
mpSerialStartTime(serial_start_time)
{
}

PacketReader::~PacketReader()
{
}

int PacketReader::read(PacketWrapper * packet_wrapper) const
{
    packet_wrapper->serial = (*mpSerial);
    packet_wrapper->serial_start_time = (*mpSerialStartTime);
    return av_read_frame(*mppFormatContext, packet_wrapper->packet);
}

int PacketReader::serial()
{
    return (*mpSerial);
}

int64_t PacketReader::serial_start_time()
{
    return (*mpSerialStartTime);
}