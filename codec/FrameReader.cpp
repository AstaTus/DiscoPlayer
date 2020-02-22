#include "FrameReader.h"
#include "../stream/Reader.h"

FrameReader::FrameReader(FrameQueue * const * frame_queue, 
                        FrameConcurrentCachePool * const * frame_cache_pool,
                        Reader * const * packet_reader)
    : mppFrameCachePool(frame_cache_pool),
      mppFrameQueue(frame_queue),
      mppPacketReader(packet_reader)
{
}

FrameReader::~FrameReader()
{
    mppFrameCachePool = nullptr;
    mppFrameQueue = nullptr;
    mppPacketReader = nullptr;
}

FrameWrapper *FrameReader::pop_frame()
{
    return (*mppFrameQueue)->block_pop_node();
}
void FrameReader::recycle_frame(FrameWrapper *frame)
{
    (*mppFrameCachePool)->recycle_node(frame);
}

int FrameReader::serial()
{
    return (*mppPacketReader)->serial();
}

int64_t FrameReader::serial_start_time()
{
    return (*mppPacketReader)->serial_start_time();
}