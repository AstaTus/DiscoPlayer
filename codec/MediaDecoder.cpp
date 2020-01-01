#include "MediaDecoder.h"
#include "../stream/IStreamIterator.h"
#include "../stream/Reader.h"
#include <future>

MediaDecoder::MediaDecoder(IStreamIterator *input_stream_iterator, Reader *packet_reader)
    : pInputStreamIterator(input_stream_iterator),
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
    while (pInputStreamIterator->has_next())
    {
        const AVStream *stream = pInputStreamIterator->next();
        if (stream != NULL)
        {
            AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
            if (codec != 0)
            {
                AVCodecContext *codec_context = avcodec_alloc_context3(codec);
                if (codec_context == NULL)
                {
                    return false;
                }
                avcodec_parameters_to_context(codec_context, stream->codecpar);

                if (avcodec_open2(codec_context, codec, NULL) < 0)
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
    mDecodePacketFuture = std::async(std::launch::async, &MediaDecoder::unpack_packet_loop, this);
    //创建解packet线程
    // pthread_create(&mUnpackPacketThreadId, NULL, unpack_packet_thread_func, (void*)this);

    mDecodeFrameFuture = std::async(std::launch::async, &MediaDecoder::unpack_frame_loop, this);
    //创建解frame线程
    // pthread_create(&mUnpackFrameThreadId, NULL, unpack_frame_thread_func, (void*)this);

    return true;
}

void MediaDecoder::unpack_packet_loop()
{
    //TODO 状态判断，如果暂停就挂起
    while (true)
    {
        AVPacket *packet = mPacketConcurrentCachePool.get_empty_node();
        pPacketReader->read(packet);
        mPacketQueue.push_node(packet);
    }
}

void MediaDecoder::unpack_frame_loop()
{
    int receive_frame_ret = 0;
    int send_packet_ret = 0;
    bool is_continue = true;
    //TODO 状态判断，如果暂停就挂起
    while (is_continue)
    {
        AVPacket *packet = mPacketQueue.block_pop_node();
        FrameQueue *frame_queue = mFrameQueues[packet->stream_index];
        AVCodecContext *codec_context = mDecodes[packet->stream_index];
        FrameConcurrentCachePool *frame_pool = mFrameCachePools[packet->stream_index];
        AVFrame *frame = frame_pool->get_empty_node();
        send_packet_ret = avcodec_send_packet(codec_context, packet);
        if (0 == send_packet_ret)
        {
            receive_frame_ret = avcodec_receive_frame(codec_context, frame);
            if(0 == receive_frame_ret) {
                frame_queue->push_node(frame);
            }
            else if (AVERROR(EAGAIN) == receive_frame_ret)
            {
                
            }
            else if (AVERROR_EOF == receive_frame_ret)
            {
                is_continue = false;
            }
            else if (AVERROR(EINVAL) == receive_frame_ret)
            {
                is_continue = false;
            }
            else
            {
                is_continue = false;
                av_log(NULL, AV_LOG_INFO, "[player]:MediaDecoder::avcodec_receive_frame is error\n");
            }
        }
        else if (AVERROR(EAGAIN) == send_packet_ret)
        {
            is_continue = false;
        }
        else if (AVERROR_EOF == send_packet_ret)
        {
            is_continue = false;
        }
        else if (AVERROR(EINVAL) == send_packet_ret)
        {
            is_continue = false;
        }
        else if (AVERROR(ENOMEM) == send_packet_ret)
        {
            is_continue = false;
        }
        else
        {
            is_continue = false;
        }

        mPacketConcurrentCachePool.recycle_node(packet);
    }
}

AVFrame *MediaDecoder::pop_frame(AVMediaType type)
{
    map<AVMediaType, int>::iterator itr = mMediaTypeIndexMap.find(type);
    if (itr != mMediaTypeIndexMap.end())
    {
        FrameQueue *frame_queue = mFrameQueues[itr->second];
        return frame_queue->block_pop_node();
    }

    // TODO 无此type 抛异常
    return NULL;
}
void MediaDecoder::recycle_frame(AVMediaType type, AVFrame *frame)
{
    map<AVMediaType, int>::iterator itr = mMediaTypeIndexMap.find(type);
    if (itr != mMediaTypeIndexMap.end())
    {
        FrameConcurrentCachePool *frame_cache_pool = mFrameCachePools[itr->second];
        frame_cache_pool->recycle_node(frame);
    }
    else
    {
        // TODO 无此type 抛异常
    }
}
