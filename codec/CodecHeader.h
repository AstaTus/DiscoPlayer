#ifndef __CODEC_HEADER_H__
#define __CODEC_HEADER_H__
struct FrameWrapper;

using FrameConcurrentCachePool = ConcurrentCachePool<FrameWrapper>;
using FrameQueue = ConcurrentQueue<FrameWrapper>;

#endif