#ifndef __FRAME_READER_H__
#define __FRAME_READER_H__

#include "../common/structure/ConcurrentQueue.h"
#include "../common/cache/ConcurrentCachePool.h"

#include "CodecHeader.h"
class FrameReader
{
private:
    FrameConcurrentCachePool * const mpFrameCachePool;
    FrameQueue* const mpFrameQueue;
public:
    FrameReader(FrameQueue * frame_queue, FrameConcurrentCachePool* frame_cache_pool);
    ~FrameReader();
    FrameWrapper * pop_frame();
    void recycle_frame(FrameWrapper* frame);

};
#endif