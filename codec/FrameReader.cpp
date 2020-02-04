#include "FrameReader.h"

FrameReader::FrameReader(FrameQueue * const * frame_queue, 
                        FrameConcurrentCachePool * const * frame_cache_pool)
    : mppFrameCachePool(frame_cache_pool),
      mppFrameQueue(frame_queue)
{
}

FrameReader::~FrameReader()
{
}

FrameWrapper *FrameReader::pop_frame()
{
    return (*mppFrameQueue)->block_pop_node();
}
void FrameReader::recycle_frame(FrameWrapper *frame)
{
    (*mppFrameCachePool)->recycle_node(frame);
}