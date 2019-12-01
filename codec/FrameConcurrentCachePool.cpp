#include "FrameConcurrentCachePool.h"

extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

FrameConcurrentCachePool::FrameConcurrentCachePool(/* args */)
{
}

FrameConcurrentCachePool::~FrameConcurrentCachePool()
{
}

AVFrame* FrameConcurrentCachePool::create_node()
{
    return (AVFrame *)av_malloc(sizeof(AVFrame));
}

void FrameConcurrentCachePool::destroy_node(AVFrame * node)
{
    av_frame_free(node);
}