#ifndef __MEDIA_DECODER_H__
#define __MEDIA_DECODER_H__

#include <pthread.h>

#include <vector>

class IStreamIterator;
class IReader;
class AVCodecContext;
class PacketQueue;
class FrameQueue;

using namespace std;
class MediaDecoder
{
private:
    IStreamIterator * pInputStreamIterator;
    IReader * pPacketReader;
    vector<AVCodecContext*> mDecodes;
    vector<PacketQueue*> mPacketQueues;
    vector<FrameQueue*> mFrameQueues;
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

    bool start();
    bool stop();
    bool pause();
};
#endif