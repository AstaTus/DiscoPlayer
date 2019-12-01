#include "FrameQueue.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
	// #include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

FrameQueue::FrameQueue(int init_size)
:ConcurrentQueue(init_size)
{

}

FrameQueue::~FrameQueue()
{
}

AVFrame* FrameQueue::create_node()
{
    return (AVFrame *)av_malloc(sizeof(AVFrame));
}

void FrameQueue::destroy_node(AVFrame * node)
{
    av_frame_free(&pFrame);

