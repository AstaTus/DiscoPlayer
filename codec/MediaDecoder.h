#ifndef __MEDIA_DECODER_H__
#define __MEDIA_DECODER_H__

#include <pthread.h>
#include <vector>
#include <queue>
#include <map>
#include <future>

#include "../common/structure/ConcurrentQueue.h"
#include "../common/cache/ConcurrentCachePool.h"
#include "../common/thread/Semaphore.h"

#include "CodecHeader.h"

class IStreamIterator;
class Reader;
struct AVCodecContext;
struct AVPacket;
struct AVStream;
struct PacketWrapper;
class FrameReader;
extern "C"
{
	#include "libavutil/avutil.h"
}

using namespace std;


class MediaDecoder
{
private:
    static const int PACKET_QUEUE_CACHE_SIZE = 150;
    static const int FRAME_QUEUE_CACHE_SIZE = 2;

    IStreamIterator * pInputStreamIterator;
    
    vector<AVCodecContext*> mDecodes;
    AVCodecContext * mpVideoCodecContext;
    AVCodecContext * mpAudioCodecContext;

    vector<AVStream*> mStreams;
    AVStream * mpVideoStream;
    AVStream * mpAudioStream;

    map<AVMediaType, int> mMediaTypeIndexMap;
    map<int, AVMediaType> mMediaIndexTypeMap;

    //解packet
    Reader * pPacketReader;
    ConcurrentCachePool<PacketWrapper> mPacketWrapperConcurrentCachePool;
    ConcurrentQueue<PacketWrapper> mVideoWrapperPacketQueue;
    ConcurrentQueue<PacketWrapper> mAudioWrapperPacketQueue;

    //解frame
    vector<FrameQueue*> mFrameQueues;
    vector<FrameConcurrentCachePool*> mFrameCachePools;

    FrameQueue * mpVideoQueue;
    FrameQueue * mpAudioQueue;

    FrameConcurrentCachePool * mpVideoFrameCachePool;
    FrameConcurrentCachePool * mpAudioFrameCachePool;


    std::future<void> mDecodePacketFuture;
    std::future<void> mDecodeVideoFrameFuture;
    std::future<void> mDecodeAudioFrameFuture;

    std::atomic<bool> mIsClearVideoBufferAndPause;
    std::atomic<bool> mIsClearAudioBufferAndPause;
    std::atomic<bool> mIsClearPacketBufferAndPause;

    Semaphore mVideoDecodeSemaphore;
    Semaphore mAudioDecodeSemaphore;
    Semaphore mPacketUnpackSemaphore;
    Semaphore mClearBufferSemaphore;
    Semaphore mClearBufferSyncSemaphore;

    std::atomic<bool> mIsStop;
    
    bool init_decodes();

    void unpack_packet_loop();
    void unpack_video_frame_loop();
    void unpack_audio_frame_loop();

    void clear_packet_buffer();
    void clear_frame_buffer(FrameQueue* frame_queue, 
        FrameConcurrentCachePool* frame_cache_pool,
        AVCodecContext * codec_context);

    void unpack_frame_loop(
        ConcurrentQueue<PacketWrapper> * packet_queue,
        FrameQueue * frame_queue,
        AVCodecContext * codec_context,
        FrameConcurrentCachePool * frame_cache_pool,
        AVStream * stream,
        std::atomic<bool>& is_clear_buffer,
        Semaphore& semaphore);
public:
    MediaDecoder(IStreamIterator * input_stream_iterator, Reader * packet_reader);
    ~MediaDecoder();

    bool start();
    bool stop();
    bool clear_buffer_and_pause();
    bool resume();

    FrameReader * get_video_frame_reader();

    FrameReader * get_audio_frame_reader();
};
#endif