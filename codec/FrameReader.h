#ifndef __FRAME_READER_H__
#define __FRAME_READER_H__

#include "../common/structure/ConcurrentQueue.h"
#include "../common/cache/ConcurrentCachePool.h"

#include "CodecHeader.h"
class FrameReader
{
private:
    FrameConcurrentCachePool * const * mppFrameCachePool;
    FrameQueue * const * mppFrameQueue;
public:
    FrameReader(FrameQueue * const * frame_queue, FrameConcurrentCachePool * const * frame_cache_pool);
    ~FrameReader();
    FrameWrapper * pop_frame();
    void recycle_frame(FrameWrapper* frame);

};
#endif