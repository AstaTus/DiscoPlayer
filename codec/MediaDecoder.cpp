#include "MediaDecoder.h"
#include "../stream/IStreamIterator.h"
#include "../stream/Reader.h"
#include <future>
#include "../log/Log.h"
#include "FrameWrapper.h"

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
        AVStream *stream = pInputStreamIterator->next();
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
                mMediaIndexTypeMap[index] = codec->type;
                mDecodes.push_back(codec_context);
                mStreams.push_back(stream);

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

    mDecodeVideoFrameFuture = std::async(std::launch::async, &MediaDecoder::unpack_video_frame_loop, this);
    //创建解frame线程
    // pthread_create(&mUnpackFrameThreadId, NULL, unpack_frame_thread_func, (void*)this);
    mDecodeAudioFrameFuture = std::async(std::launch::async, &MediaDecoder::unpack_audio_frame_loop, this);

    return true;
}

void MediaDecoder::unpack_packet_loop()
{
    //TODO 状态判断，如果暂停就挂起
    while (true)
    {
        //TODO 此处可能视频或者音频在外部不消费，此处mPacketConcurrentCachePool达到上线后就会挂起
        AVPacket *packet = mPacketConcurrentCachePool.get_empty_node();
        pPacketReader->read(packet);
        AVMediaType type = mMediaIndexTypeMap[packet->stream_index];
        if (type == AVMEDIA_TYPE_VIDEO)
        {
            mVideoPacketQueue.push_node(packet);
        }
        else if (type == AVMEDIA_TYPE_AUDIO)
        {
            mAudioPacketQueue.push_node(packet);
        }
        else
        {
            //TODO log
            av_log(nullptr, AV_LOG_ERROR, "[Disco]MediaDecoder::unpack_packet_loop has other stream");
        }
    }
}

void MediaDecoder::unpack_audio_frame_loop()
{
    // int receive_frame_ret = 0;
    // int send_packet_ret = 0;
    // bool is_continue = true;
    // //TODO 状态判断，如果暂停就挂起
    // while (is_continue)
    // {
    //     AVPacket *packet = mAudioPacketQueue.block_pop_node();
    //     FrameQueue *frame_queue = mFrameQueues[packet->stream_index];
    //     AVCodecContext *codec_context = mDecodes[packet->stream_index];
    //     FrameConcurrentCachePool *frame_pool = mFrameCachePools[packet->stream_index];

    //     send_packet_ret = avcodec_send_packet(codec_context, packet);
    //     if (0 == send_packet_ret)
    //     {
    //         Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet send a valid packet\n");
    //         AVFrame *frame = frame_pool->get_empty_node();
    //         receive_frame_ret = avcodec_receive_frame(codec_context, frame);
    //         if (0 == receive_frame_ret)
    //         {
    //             frame_queue->push_node(frame);
    //             Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame receive a valid frame stream_index = %d\n", packet->stream_index);
    //         }
    //         else if (AVERROR(EAGAIN) == receive_frame_ret)
    //         {
    //             frame_pool->recycle_node(frame);
    //             Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return error code = AVERROR(EAGAIN)\n");
    //         }
    //         else if (AVERROR_EOF == receive_frame_ret)
    //         {
    //             is_continue = false;
    //             Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return error code = AVERROR_EOF\n");
    //         }
    //         else if (AVERROR(EINVAL) == receive_frame_ret)
    //         {
    //             is_continue = false;
    //             Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return error code = AVERROR(EINVAL)\n");
    //         }
    //         else
    //         {
    //             is_continue = false;
    //             Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return other error code = %d\n", receive_frame_ret);
    //         }
    //     }
    //     else if (AVERROR(EAGAIN) == send_packet_ret)
    //     {
    //         is_continue = false;
    //         Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR(EAGAIN)\n");
    //     }
    //     else if (AVERROR_EOF == send_packet_ret)
    //     {
    //         is_continue = false;
    //         Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR_EOF\n");
    //     }
    //     else if (AVERROR(EINVAL) == send_packet_ret)
    //     {
    //         is_continue = false;
    //         Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR(EINVAL)\n");
    //     }
    //     else if (AVERROR(ENOMEM) == send_packet_ret)
    //     {
    //         is_continue = false;
    //         Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR(ENOMEM)\n");
    //     }
    //     else
    //     {
    //         is_continue = false;
    //         Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return other error code = %d\n", send_packet_ret);
    //     }

    //     mPacketConcurrentCachePool.recycle_node(packet);
    // }

    // Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop unpack_frame_loop thread over");
}

void MediaDecoder::unpack_video_frame_loop()
{
    int receive_frame_ret = 0;
    int send_packet_ret = 0;
    bool is_continue = true;
    //TODO 状态判断，如果暂停就挂起
    while (is_continue)
    {
        AVPacket *packet = mVideoPacketQueue.block_pop_node();
        FrameQueue *frame_queue = mFrameQueues[packet->stream_index];
        AVCodecContext *codec_context = mDecodes[packet->stream_index];
        FrameConcurrentCachePool *frame_pool = mFrameCachePools[packet->stream_index];
        AVStream * stream = mStreams[packet->stream_index];
        send_packet_ret = avcodec_send_packet(codec_context, packet);
        if (0 == send_packet_ret)
        {
            Log::get_instance().log_debug("[Disco][MediaDecoder] avcodec_send_packet send a valid packet\n");
            FrameWrapper *frame_wrapper = frame_pool->get_empty_node();
            receive_frame_ret = avcodec_receive_frame(codec_context, frame_wrapper->frame);
            if (0 == receive_frame_ret)
            {
                frame_wrapper->time_base = stream->time_base;
                frame_queue->push_node(frame_wrapper);
                Log::get_instance().log_debug("[Disco][MediaDecoder] avcodec_receive_frame receive a valid frame stream_index = %d\n", packet->stream_index);
            }
            else if (AVERROR(EAGAIN) == receive_frame_ret)
            {
                frame_pool->recycle_node(frame_wrapper);
                Log::get_instance().log_error("[Disco][MediaDecoder] avcodec_receive_frame return error code = AVERROR(EAGAIN)\n");
            }
            else if (AVERROR_EOF == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco][MediaDecoder] avcodec_receive_frame return error code = AVERROR_EOF\n");
            }
            else if (AVERROR(EINVAL) == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco][MediaDecoder] avcodec_receive_frame return error code = AVERROR(EINVAL)\n");
            }
            else
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco][MediaDecoder] avcodec_receive_frame return other error code = %d\n", receive_frame_ret);
            }
        }
        else if (AVERROR(EAGAIN) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco][MediaDecoder] avcodec_send_packet return error code = AVERROR(EAGAIN)\n");
        }
        else if (AVERROR_EOF == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco][MediaDecoder] avcodec_send_packet return error code = AVERROR_EOF\n");
        }
        else if (AVERROR(EINVAL) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco][MediaDecoder] avcodec_send_packet return error code = AVERROR(EINVAL)\n");
        }
        else if (AVERROR(ENOMEM) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco][MediaDecoder] avcodec_send_packet return error code = AVERROR(ENOMEM)\n");
        }
        else
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco][MediaDecoder] avcodec_send_packet return other error code = %d\n", send_packet_ret);
        }

        mPacketConcurrentCachePool.recycle_node(packet);
    }

    Log::get_instance().log_debug("[Disco][MediaDecoder] unpack_frame_loop thread over");
}

FrameWrapper *MediaDecoder::pop_frame(AVMediaType type)
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
void MediaDecoder::recycle_frame(AVMediaType type, FrameWrapper *frame)
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