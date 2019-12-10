#include "FrameQueue.h"

extern "C"
{
     #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	 #include "libavutil/imgutils.h"
}

FrameQueue::FrameQueue()
:ConcurrentQueue<AVFrame>()
{

}

FrameQueue::~FrameQueue()
{
}

void FrameQueue::destroy_node(AVFrame* node)
{
    av_frame_free(&node);
}

