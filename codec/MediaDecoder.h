#ifndef __MEDIA_DECODER_H__
#define __MEDIA_DECODER_H__

#include <pthread.h>
#include <vector>
#include <queue>

extern "C"
{
    #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

class IStreamIterator;
class IReader;
class AVCodecContext;
class FrameQueue;
class PacketQueue;
class AVPacket;
class AVFrame;

class PacketConcurrentCachePool;
class FrameConcurrentCachePool;

using namespace std;
class MediaDecoder
{
private:
    IStreamIterator * pInputStreamIterator;
    
    vector<AVCodecContext*> mDecodes;

    //解packet
    IReader * pPacketReader;
    PacketConcurrentCachePool mPacketConcurrentCachePool;
    PacketQueue mPacketQueue;

    //解frame
    vector<FrameQueue*> mFrameQueues;
    vector<FrameConcurrentCachePool*> mFrameCachePools;
    
    pthread_t mUnpackPacketThreadId;
    pthread_t mUnpackFrameThreadId;


    bool init_decodes();

    static void unpack_packet_thread_func(void *self);
    static void unpack_frame_thread_func(void *self);
    void unpack_packet_loop();
    void unpack_frame_loop();
public:
    MediaDecoder(IStreamIterator * input_stream_iterator, IReader * packet_reader);
    ~MediaDecoder();

    const AVFrame * const pop_frame(AVMediaType type);
    void recycle_frame(AVMediaType type, AVFrame* frame);

    bool start();
    bool stop();
    bool pause();
    bool resume();
};
#endif