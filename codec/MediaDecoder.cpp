#include "MediaDecoder.h"
#include "../stream/IStreamIterator.h"
#include "../stream/IReader.h"
#include "FrameQueue.h"
#include "PacketQueue.h"
#include "PacketConcurrentCachePool.h"
#include "FrameConcurrentCachePool.h"

MediaDecoder::MediaDecoder(IStreamIterator * input_stream_iterator, IReader * packet_reader)
:pInputStreamIterator(input_stream_iterator),
pPacketReader(packet_reader),
mPacketConcurrentCachePool(PACKET_QUEUE_CACHE_SIZE)
{
    init_decodes();
}

MediaDecoder::~MediaDecoder()
{
}

bool MediaDecoder::init_decodes()
{
    int index = 0;
    while(pInputStreamIterator->has_next()) 
    {
        const AVStream * stream = pInputStreamIterator->next();
        if (stream != NULL)
        {
            AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
            if (codec != 0)
            {
                AVCodecContext	* codec_context = avcodec_alloc_context3(codec);
                if (codec_context == NULL)
                {
                    return false;
                }
                avcodec_parameters_to_context(codec_context, stream->codecpar);

                if(avcodec_open2(codec_context, codec, NULL) < 0)
                {
                    return false;
                }

                mMediaTypeIndexMap[codec->type] = index;
                mDecodes.push_back(codec_context);
                mFrameCachePools.push_back(new FrameConcurrentCachePool(FRAME_QUEUE_CACHE_SIZE));
                mFrameQueues.push_back(new FrameQueue());
                ++index;
            }
        }
    }

    return true;
}

bool MediaDecoder::start()
{
    //创建解packet线程
    pthread_create(&mUnpackPacketThreadId, NULL, unpack_packet_thread_func, (void*)this);

    //创建解frame线程
    pthread_create(&mUnpackFrameThreadId, NULL, unpack_frame_thread_func, (void*)this);
}

void MediaDecoder::unpack_packet_thread_func(void *self)
{
    static_cast<MediaDecoder *>(self)->unpack_packet_loop();
}

void MediaDecoder::unpack_frame_thread_func(void *self)
{
    static_cast<MediaDecoder *>(self)->unpack_frame_loop();
}


void MediaDecoder::unpack_packet_loop()
{
    //TODO 状态判断，如果暂停就挂起
    while (true)
    {
        AVPacket* packet = mPacketConcurrentCachePool.get_empty_node();
        pPacketReader->read(packet);
        mPacketQueue.push_node(packet);
    }
}

void MediaDecoder::unpack_frame_loop()
{
    bool got_picture = false;
    //TODO 状态判断，如果暂停就挂起
    while (true)
    {
        AVPacket * packet = mPacketQueue.pop_node();
        FrameQueue * frame_queue = mFrameQueues[packet->stream_index];
        AVCodecContext * codec_context = mDecodes[packet->stream_index];
        FrameConcurrentCachePool * frame_pool = mFrameCachePools[packet->stream_index];
        AVFrame * frame = frame_pool->get_empty_node();
        got_picture = avcodec_receive_frame(codec_context, frame);
        if (got_picture == 0)
        {
            frame_queue->push_node(frame);
        } else {
            av_log(this, AV_LOG_INFO, "[player]:MediaDecoder::avcodec_receive_frame is error"); 
        }
    }
}

const AVFrame * const MediaDecoder::pop_frame(AVMediaType type)
{
    map<AVMediaType, int>::iterator itr = mMediaTypeIndexMap.find(type);
    if (itr != mMediaTypeIndexMap.end())
    {
        FrameQueue * frame_queue = mFrameQueues[itr->second];
        return frame_queue->pop_node();
    }

    // TODO 无此type 抛异常
    return NULL;
    
}
void MediaDecoder::recycle_frame(AVMediaType type, AVFrame* frame)
{
    map<AVMediaType, int>::iterator itr = mMediaTypeIndexMap.find(type);
    if (itr != mMediaTypeIndexMap.end())
    {
        FrameConcurrentCachePool * frame_cache_pool = mFrameCachePools[itr->second];
        frame_cache_pool->recycle_node(frame);
    } else {
        // TODO 无此type 抛异常
    }
}

