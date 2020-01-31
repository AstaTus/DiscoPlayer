#include "FrameReader.h"

FrameReader::FrameReader(FrameQueue *frame_queue, FrameConcurrentCachePool * frame_cache_pool)
    : mpFrameCachePool(frame_cache_pool),
      mpFrameQueue(frame_queue)
{
}

FrameReader::~FrameReader()
{
}

FrameWrapper *FrameReader::pop_frame()
{
    return mpFrameQueue->block_pop_node();
}
void FrameReader::recycle_frame(FrameWrapper *frame)
{
    mpFrameCachePool->recycle_node(frame);
}