#ifndef __MEDIA_DECODER_H__
#define __MEDIA_DECODER_H__

#include <pthread.h>
#include <vector>
#include <queue>
#include <map>
#include <future>

#include "../common/structure/ConcurrentQueue.h"
#include "../common/cache/ConcurrentCachePool.h"

class IStreamIterator;
class Reader;
struct AVCodecContext;
struct AVPacket;
struct FrameWrapper;
struct AVStream;

extern "C"
{
	#include "libavutil/avutil.h"
}

using namespace std;
using FrameConcurrentCachePool = ConcurrentCachePool<FrameWrapper>;
using FrameQueue = ConcurrentQueue<FrameWrapper>;

class MediaDecoder
{
private:
    static const int PACKET_QUEUE_CACHE_SIZE = 400;
    static const int FRAME_QUEUE_CACHE_SIZE = 2;

    IStreamIterator * pInputStreamIterator;
    
    vector<AVCodecContext*> mDecodes;
    vector<AVStream*> mStreams;
    map<AVMediaType, int> mMediaTypeIndexMap;
    map<int, AVMediaType> mMediaIndexTypeMap;

    //解packet
    Reader * pPacketReader;
    ConcurrentCachePool<AVPacket> mPacketConcurrentCachePool;
    ConcurrentQueue<AVPacket> mVideoPacketQueue;
    ConcurrentQueue<AVPacket> mAudioPacketQueue;

    //解frame
    vector<FrameQueue*> mFrameQueues;
    vector<FrameConcurrentCachePool*> mFrameCachePools;

    std::future<void> mDecodePacketFuture;
    std::future<void> mDecodeVideoFrameFuture;
    std::future<void> mDecodeAudioFrameFuture;
    
    
    bool init_decodes();

    void unpack_packet_loop();
    void unpack_video_frame_loop();
    void unpack_audio_frame_loop();
public:
    MediaDecoder(IStreamIterator * input_stream_iterator, Reader * packet_reader);
    ~MediaDecoder();

    FrameWrapper * pop_frame(AVMediaType type);
    void recycle_frame(AVMediaType type, FrameWrapper* frame);

    bool start();
    bool stop();
    bool pause();
    bool resume();
};
#endif