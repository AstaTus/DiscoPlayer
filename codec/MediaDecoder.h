#ifndef __MEDIA_DECODER_H__
#define __MEDIA_DECODER_H__

#include <pthread.h>
#include <vector>
#include <queue>
#include <map>
#include <future>

extern "C"
{
    #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

#include "../common/structure/ConcurrentQueue.h"
#include "../common/cache/ConcurrentCachePool.h"

class IStreamIterator;
class Reader;
class AVCodecContext;
class AVPacket;

using namespace std;
using FrameConcurrentCachePool = ConcurrentCachePool<AVFrame>;
using FrameQueue = ConcurrentQueue<AVFrame>;

class MediaDecoder
{
private:
    static const int PACKET_QUEUE_CACHE_SIZE = 40;
    static const int FRAME_QUEUE_CACHE_SIZE = 40;

    IStreamIterator * pInputStreamIterator;
    
    vector<AVCodecContext*> mDecodes;
    map<AVMediaType, int> mMediaTypeIndexMap;
    //解packet
    Reader * pPacketReader;
    ConcurrentCachePool<AVPacket> mPacketConcurrentCachePool;
    ConcurrentQueue<AVPacket> mPacketQueue;

    //解frame
    vector<FrameQueue*> mFrameQueues;
    vector<FrameConcurrentCachePool*> mFrameCachePools;

    std::future<void> mDecodePacketFuture;
    std::future<void> mDecodeFrameFuture;
    
    bool init_decodes();

    void unpack_packet_loop();
    void unpack_frame_loop();
public:
    MediaDecoder(IStreamIterator * input_stream_iterator, Reader * packet_reader);
    ~MediaDecoder();

    AVFrame * pop_frame(AVMediaType type);
    void recycle_frame(AVMediaType type, AVFrame* frame);

    bool start();
    bool stop();
    bool pause();
    bool resume();
};
#endif