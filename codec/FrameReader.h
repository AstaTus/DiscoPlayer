#ifndef __FRAME_READER_H__
#define __FRAME_READER_H__

#include "../common/structure/ConcurrentQueue.h"
#include "../common/cache/ConcurrentCachePool.h"

#include "CodecHeader.h"
class Reader;
class FrameReader
{
private:
    FrameConcurrentCachePool * const * mppFrameCachePool;
    FrameQueue * const * mppFrameQueue;
    Reader * const * mppPacketReader;
public:
    FrameReader(FrameQueue * const * frame_queue, 
                FrameConcurrentCachePool * const * frame_cache_pool,
                Reader * const * packet_reader);
    ~FrameReader();
    FrameWrapper * pop_frame();
    void recycle_frame(FrameWrapper* frame);
    int serial();

    int64_t serial_start_time();

};
#endif