#include "PacketReader.h"
extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

PacketReader::PacketReader(AVFormatContext * format_context)
:pFormatContext(format_context)
{
}

PacketReader::~PacketReader()
{
}

int PacketReader::read(AVPacket *packet) const
{
    return av_read_frame(pFormatContext, packet);
}