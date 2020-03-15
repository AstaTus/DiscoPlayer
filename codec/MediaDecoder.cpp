#include "MediaDecoder.h"
#include "../stream/IStreamIterator.h"
#include "../stream/Reader.h"
#include "../stream/PacketWrapper.h"
#include <future>
#include "../common/log/Log.h"
#include "FrameWrapper.h"
#include "FrameReader.h"
#include "../common/thread/ThreadUtils.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/avutil.h"
}

MediaDecoder::MediaDecoder(IStreamIterator *input_stream_iterator, Reader *packet_reader)
    : pInputStreamIterator(input_stream_iterator),
      mpVideoCodecContext(nullptr),
      mpAudioCodecContext(nullptr),
      mpVideoStream(nullptr),
      mpAudioStream(nullptr),
      mMediaIndexTypeMap(),
      pPacketReader(packet_reader),
      mVideoPacketWrapperConcurrentCachePool(PACKET_QUEUE_CACHE_SIZE),
      mVideoWrapperPacketQueue(),
      mAudioWrapperPacketQueue(),
      mpVideoQueue(nullptr),
      mpAudioQueue(nullptr),
      mpVideoFrameReader(nullptr),
      mpAudioFrameReader(nullptr),
      mpVideoFrameCachePool(nullptr),
      mpAudioFrameCachePool(nullptr),
      mIsStop(false)
{
}

MediaDecoder::~MediaDecoder()
{
    mDecodePacketFuture.wait();
    mDecodeVideoFrameFuture.wait();
    mDecodeAudioFrameFuture.wait();

    FrameWrapper *frame_wrapper = mpVideoQueue->non_block_peek_node();
    while (frame_wrapper != nullptr)
    {
        mpVideoFrameCachePool->recycle_node(frame_wrapper);
        frame_wrapper = mpVideoQueue->non_block_peek_node();
    }

    frame_wrapper = mpAudioQueue->non_block_peek_node();
    while (frame_wrapper != nullptr)
    {
        mpAudioFrameCachePool->recycle_node(frame_wrapper);
        frame_wrapper = mpAudioQueue->non_block_peek_node();
    }

    PacketWrapper *packet_wrapper = mVideoWrapperPacketQueue.non_block_pop_node();
    while (packet_wrapper != nullptr)
    {
        recycle_packet_wrapper(packet_wrapper);
        packet_wrapper = mVideoWrapperPacketQueue.non_block_pop_node();
    }

    packet_wrapper = mAudioWrapperPacketQueue.non_block_pop_node();
    while (packet_wrapper != nullptr)
    {
        recycle_packet_wrapper(packet_wrapper);
        packet_wrapper = mAudioWrapperPacketQueue.non_block_pop_node();
    }

    delete mpVideoFrameReader;
    delete mpAudioFrameReader;


    delete mpVideoQueue;
    delete mpAudioQueue;

    delete mpVideoFrameCachePool;
    delete mpAudioFrameCachePool;

    avcodec_close(mpVideoCodecContext);
    avcodec_close(mpAudioCodecContext);

    pInputStreamIterator = nullptr;
    pPacketReader = nullptr;

    mpAudioCodecContext = nullptr;
    mpVideoCodecContext = nullptr;

    mpAudioStream = nullptr;
    mpVideoStream = nullptr;

    mpVideoFrameReader = nullptr;
    mpAudioFrameReader = nullptr;

    mpAudioQueue = nullptr;
    mpVideoQueue = nullptr;

    mpVideoFrameCachePool = nullptr;
    mpAudioFrameCachePool = nullptr;

    mMediaIndexTypeMap.clear();
}

bool MediaDecoder::init_decodes()
{
    int index = 0;
    while (pInputStreamIterator->has_next())
    {
        AVStream *stream = pInputStreamIterator->next();
        if (stream != nullptr)
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

                // mMediaTypeIndexMap[codec->type] = index;
                mMediaIndexTypeMap[index] = codec->type;

                if (codec->type == AVMEDIA_TYPE_VIDEO)
                {
                    mpVideoCodecContext = codec_context;
                    mpVideoStream = stream;
                    mpVideoFrameCachePool = new FrameConcurrentCachePool(FRAME_QUEUE_CACHE_SIZE);
                    mpVideoQueue = new FrameQueue();
                }
                else if (codec->type == AVMEDIA_TYPE_AUDIO)
                {
                    mpAudioCodecContext = codec_context;
                    mpAudioStream = stream;
                    mpAudioFrameCachePool = new FrameConcurrentCachePool(FRAME_QUEUE_CACHE_SIZE);
                    mpAudioQueue = new FrameQueue();
                }
                ++index;
            }
        }
    }

    return true;
}

void MediaDecoder::recycle_packet_wrapper(PacketWrapper *packet_wrapper)
{
    packet_wrapper->recycle();
    mVideoPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
}

bool MediaDecoder::start()
{
    init_decodes();
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
    ThreadUtils::set_thread_name("Disco Unpack Packet");

    //TODO 状态判断，如果暂停就挂起
    while (!mIsStop)
    {
        //TODO 此处可能视频或者音频在外部不消费，此处mPacketConcurrentCachePool达到上线后就会挂起
        PacketWrapper *packet_wrapper = mVideoPacketWrapperConcurrentCachePool.get_empty_node();
        int ret = pPacketReader->read(packet_wrapper);
        if (ret == 0)
        {
            if (packet_wrapper->serial != pPacketReader->serial())
            {
                recycle_packet_wrapper(packet_wrapper);
                Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_packet_loop discard packet\n");
            }

            AVMediaType type = mMediaIndexTypeMap[packet_wrapper->packet->stream_index];
            if (type == AVMEDIA_TYPE_VIDEO)
            {
                mVideoWrapperPacketQueue.push_node(packet_wrapper);
                Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_packet_loop unpack video packet is key packet = %d\n", packet_wrapper->is_key_packet());
            }
            else if (type == AVMEDIA_TYPE_AUDIO)
            {
                Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_packet_loop unpack audio packet is key packet = %d\n", packet_wrapper->is_key_packet());
                mAudioWrapperPacketQueue.push_node(packet_wrapper);
            }
        }
        //stream is end
        else if (ret == AVERROR_EOF)
        {
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_packet_loop read stream end\n");
            PacketWrapper *new_packet_wrapper = mVideoPacketWrapperConcurrentCachePool.get_empty_node();
            new_packet_wrapper->is_end = true;
            packet_wrapper->is_end = true;
            mVideoWrapperPacketQueue.push_node(packet_wrapper);
            mAudioWrapperPacketQueue.push_node(new_packet_wrapper);
            break;
        }
        else
        {
            Log::get_instance().log_error("[Disco]MediaDecoder::unpack_packet_loop read error\n");
            break;
        }
    }
    Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_packet_loop thread is over\n");
}

void MediaDecoder::unpack_frame_loop(
    ConcurrentQueue<PacketWrapper> *packet_queue,
    FrameQueue *frame_queue,
    AVCodecContext *codec_context,
    FrameConcurrentCachePool *frame_cache_pool,
    AVStream *stream,
    AVMediaType type)
{
    int receive_frame_ret = 0;
    int send_packet_ret = 0;
    bool is_continue = true;
    int last_packet_serial = 0;

    FrameWrapper *frame_wrapper = nullptr;
    std::string type_str;
    if (type == AVMEDIA_TYPE_VIDEO)
    {
        type_str = "video";
    } else if (type == AVMEDIA_TYPE_AUDIO){
        type_str = "audio";
    }
    
    while (is_continue && !mIsStop)
    {
        PacketWrapper *packet_wrapper = packet_queue->block_pop_node();
        //stream is end
        if (packet_wrapper->is_end)
        {
            frame_wrapper = frame_cache_pool->get_empty_node();
            frame_wrapper->is_end = true;
            frame_queue->push_node(frame_wrapper);
            recycle_packet_wrapper(packet_wrapper);
            break;
        }

        if (packet_wrapper->serial != pPacketReader->serial())
        {
            recycle_packet_wrapper(packet_wrapper);
            continue;
        }
        if (packet_wrapper->serial != last_packet_serial)
        {
            avcodec_flush_buffers(codec_context);
        }

        send_packet_ret = avcodec_send_packet(codec_context, packet_wrapper->packet);
        last_packet_serial = packet_wrapper->serial;
        if (0 == send_packet_ret)
        {
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet send a valid packet %s\n", type_str.c_str());
            frame_wrapper = frame_cache_pool->get_empty_node();
            receive_frame_ret = avcodec_receive_frame(codec_context, frame_wrapper->frame);
            if (0 == receive_frame_ret)
            {
                frame_wrapper->serial = packet_wrapper->serial;
                if (frame_wrapper->serial != pPacketReader->serial() ||
                    frame_wrapper->get_transformed_pts() < pPacketReader->serial_start_time())
                {
                    frame_cache_pool->recycle_node(frame_wrapper);
                    Log::get_instance().log_debug(
                        "[Disco]MediaDecoder::unpack_frame_loop unpack discard frame %s\n", type_str.c_str());
                }
                else
                {
                    Log::get_instance().log_debug(
                        "[Disco]MediaDecoder::unpack_frame_loop unpack frame stream_index = %d, %s\n",
                        packet_wrapper->packet->stream_index, type_str.c_str());
                    frame_wrapper->time_base = stream->time_base;
                    frame_queue->push_node(frame_wrapper);
                }
            }
            else if (AVERROR(EAGAIN) == receive_frame_ret)
            {
                frame_cache_pool->recycle_node(frame_wrapper);
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return error code = AVERROR(EAGAIN) type = %s\n", type_str.c_str());
            }
            else if (AVERROR_EOF == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return error code = AVERROR_EOF type = %s\n", type_str.c_str());
            }
            else if (AVERROR(EINVAL) == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return error code = AVERROR(EINVAL) type = %s\n", type_str.c_str());
            }
            else
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return other error code = %d, type = %s\n", type_str.c_str());
            }
        }
        else if (AVERROR(EAGAIN) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR(EAGAIN) type = %s\n", type_str.c_str());
        }
        else if (AVERROR_EOF == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR_EOF type = %s\n", type_str.c_str());
        }
        else if (AVERROR(EINVAL) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR(EINVAL) type = %s\n", type_str.c_str());
        }
        else if (AVERROR(ENOMEM) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR(ENOMEM) type = %s\n", type_str.c_str());
        }
        else
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return other error code = %d type = %s\n", send_packet_ret, type_str.c_str());
        }

        recycle_packet_wrapper(packet_wrapper);
    }

    Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop unpack_frame_loop thread over type = %s\n", type_str.c_str());
}

void MediaDecoder::unpack_audio_frame_loop()
{
    ThreadUtils::set_thread_name("Disco Unpack Audio Frame");
    unpack_frame_loop(&mAudioWrapperPacketQueue, mpAudioQueue, mpAudioCodecContext,
                      mpAudioFrameCachePool, mpAudioStream, AVMEDIA_TYPE_AUDIO);
}

void MediaDecoder::unpack_video_frame_loop()
{
    ThreadUtils::set_thread_name("Disco Unpack Video Frame");
    unpack_frame_loop(&mVideoWrapperPacketQueue, mpVideoQueue, mpVideoCodecContext,
                      mpVideoFrameCachePool, mpVideoStream, AVMEDIA_TYPE_VIDEO);
}

bool MediaDecoder::resume()
{
    return true;
}

bool MediaDecoder::stop()
{
    resume();
    mIsStop = true;
    return true;
}

FrameReader *MediaDecoder::get_video_frame_reader()
{
    if(mpVideoFrameReader == nullptr) {
        mpVideoFrameReader = new FrameReader(&mpVideoQueue, &mpVideoFrameCachePool, &pPacketReader);
    }

    return mpVideoFrameReader; 
}

FrameReader *MediaDecoder::get_audio_frame_reader()
{
    if(mpAudioFrameReader == nullptr) {
        mpAudioFrameReader = new FrameReader(&mpAudioQueue, &mpAudioFrameCachePool, &pPacketReader);
    }

    return mpAudioFrameReader; 
}

void MediaDecoder::clear_packet_buffer()
{
    Log::get_instance().log_error("[Disco]MediaDecoder::clear_packet_buffer\n");
    PacketWrapper *packet_wrapper;
    packet_wrapper = mVideoWrapperPacketQueue.non_block_pop_node();
    while (packet_wrapper != nullptr)
    {
        recycle_packet_wrapper(packet_wrapper);
        packet_wrapper = mVideoWrapperPacketQueue.non_block_pop_node();
    }

    packet_wrapper = mAudioWrapperPacketQueue.non_block_pop_node();
    while (packet_wrapper != nullptr)
    {
        recycle_packet_wrapper(packet_wrapper);
        packet_wrapper = mAudioWrapperPacketQueue.non_block_pop_node();
    }
}

void MediaDecoder::clear_frame_buffer(FrameQueue *frame_queue,
                                      FrameConcurrentCachePool *frame_cache_pool,
                                      AVCodecContext *codec_context)
{
    Log::get_instance().log_error("[Disco]MediaDecoder::clear_frame_buffer clear frame_queue\n");
    FrameWrapper *frame_wrapper = frame_queue->non_block_pop_node();
    while (frame_wrapper != nullptr)
    {
        frame_cache_pool->recycle_node(frame_wrapper);
        frame_wrapper = frame_queue->non_block_pop_node();
    }

    Log::get_instance().log_error("[Disco]MediaDecoder::clear_frame_buffer avcodec_flush_buffers start\n");
    avcodec_flush_buffers(codec_context);
    Log::get_instance().log_error("[Disco]MediaDecoder::clear_frame_buffer avcodec_flush_buffers end\n");
}