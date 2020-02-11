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
      pPacketReader(packet_reader),
      mVideoPacketWrapperConcurrentCachePool(PACKET_QUEUE_CACHE_SIZE),
      mAudioPacketWrapperConcurrentCachePool(PACKET_QUEUE_CACHE_SIZE),
      mIsClearVideoBufferAndPause(false),
      mIsClearAudioBufferAndPause(false),
      mIsClearPacketBufferAndPause(false),
      mVideoDecodeSemaphore(),
      mAudioDecodeSemaphore(),
      mPacketUnpackSemaphore(),
      mClearBufferSemaphore(1),
      mClearBufferSyncSemaphore(2),
      mIsStop(false)

{
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
                
                // mDecodes.push_back(codec_context);
                // mStreams.push_back(stream);

                // mFrameCachePools.push_back(new FrameConcurrentCachePool(FRAME_QUEUE_CACHE_SIZE));
                // mFrameQueues.push_back(new FrameQueue());
                ++index;
            }
        }
    }

    return true;
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
        
        // if (mIsClearPacketBufferAndPause)
        // {
        //     clear_packet_buffer();
        //     mClearBufferSemaphore.signal();
        //     mPacketUnpackSemaphore.wait();
        //     mIsClearPacketBufferAndPause = false;
        // }

        //TODO 此处可能视频或者音频在外部不消费，此处mPacketConcurrentCachePool达到上线后就会挂起
        PacketWrapper *packet_wrapper = mVideoPacketWrapperConcurrentCachePool.get_empty_node();
        pPacketReader->read(packet_wrapper);
        AVMediaType type = mMediaIndexTypeMap[packet_wrapper->packet->stream_index];
        if (type == AVMEDIA_TYPE_VIDEO)
        {
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_packet_loop unpack video packet\n");
            mVideoWrapperPacketQueue.push_node(packet_wrapper);
        }
        else if (type == AVMEDIA_TYPE_AUDIO)
        {
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_packet_loop unpack audio packet\n");
            mAudioWrapperPacketQueue.push_node(packet_wrapper);
        }
        else
        {
            //TODO log
            Log::get_instance().log_error("[Disco]MediaDecoder::unpack_packet_loop has other stream\n");
        }
    }
}

void MediaDecoder::unpack_frame_loop(
    ConcurrentQueue<PacketWrapper> * packet_queue,
    FrameQueue * frame_queue,
    AVCodecContext * codec_context,
    FrameConcurrentCachePool * frame_cache_pool,
    AVStream * stream,
    std::atomic<bool>& is_clear_buffer,
    Semaphore& semaphore)
 {
    int receive_frame_ret = 0;
    int send_packet_ret = 0;
    bool is_continue = true;
    int last_packet_serial = 0;

    while (is_continue && !mIsStop)
    {
        // if (is_clear_buffer)
        // {
        //     clear_frame_buffer(frame_queue, frame_cache_pool, codec_context);
        //     mClearBufferSyncSemaphore.signal();
        //     semaphore.wait();
        //     is_clear_buffer = false;
        // }
        
        PacketWrapper *packet_wrapper = packet_queue->block_pop_node();

        if (packet_wrapper->serial != pPacketReader->serial())
        {
            mVideoPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
            continue;
        }
        if (packet_wrapper->serial != last_packet_serial)
        {
            avcodec_flush_buffers(codec_context);
        }
        
        send_packet_ret = avcodec_send_packet(codec_context, packet_wrapper->packet);
        last_packet_serial =  packet_wrapper->serial;
        if (0 == send_packet_ret)
        {
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet send a valid packet\n");
            FrameWrapper *frame_wrapper = frame_cache_pool->get_empty_node();
            receive_frame_ret = avcodec_receive_frame(codec_context, frame_wrapper->frame);
            if (0 == receive_frame_ret)
            {
                frame_wrapper->serial = packet_wrapper->serial;
                if (frame_wrapper->serial != pPacketReader->serial() ||
                        frame_wrapper->frame->pts * 1000 * av_q2d(stream->time_base) < pPacketReader->serial_start_time())
                {
                    frame_cache_pool->recycle_node(frame_wrapper);
                } else {
                    Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop unpack frame stream_index = %d\n", packet_wrapper->packet->stream_index);
                    frame_wrapper->time_base = stream->time_base;
                    frame_queue->push_node(frame_wrapper);
                }
            }
            else if (AVERROR(EAGAIN) == receive_frame_ret)
            {
                frame_cache_pool->recycle_node(frame_wrapper);
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return error code = AVERROR(EAGAIN)\n");
            }
            else if (AVERROR_EOF == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return error code = AVERROR_EOF\n");
            }
            else if (AVERROR(EINVAL) == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return error code = AVERROR(EINVAL)\n");
            }
            else
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_frame_loop avcodec_receive_frame return other error code = %d\n", receive_frame_ret);
            }
        }
        else if (AVERROR(EAGAIN) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR(EAGAIN)\n");
        }
        else if (AVERROR_EOF == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR_EOF\n");
        }
        else if (AVERROR(EINVAL) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR(EINVAL)\n");
        }
        else if (AVERROR(ENOMEM) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return error code = AVERROR(ENOMEM)\n");
        }
        else
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_frame_loop avcodec_send_packet return other error code = %d\n", send_packet_ret);
        }

        mVideoPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
    }

    Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop unpack_frame_loop thread over\n");

}

void MediaDecoder::unpack_audio_frame_loop()
{
    ThreadUtils::set_thread_name("Disco Unpack Audio Frame");
    unpack_frame_loop(&mAudioWrapperPacketQueue, mpAudioQueue, mpAudioCodecContext, 
            mpAudioFrameCachePool, mpAudioStream, mIsClearAudioBufferAndPause, mAudioDecodeSemaphore);

}

void MediaDecoder::unpack_video_frame_loop()
{
    ThreadUtils::set_thread_name("Disco Unpack Video Frame");
    unpack_frame_loop(&mVideoWrapperPacketQueue, mpVideoQueue, mpVideoCodecContext, 
            mpVideoFrameCachePool, mpVideoStream, mIsClearVideoBufferAndPause, mVideoDecodeSemaphore);

}

bool MediaDecoder::clear_buffer_and_pause()
{
    if (mIsClearVideoBufferAndPause || mIsClearAudioBufferAndPause || mIsClearPacketBufferAndPause)
    {
        return false;
    }
    

    mIsClearAudioBufferAndPause = true;
    mIsClearVideoBufferAndPause = true;
    mIsClearPacketBufferAndPause = true;

    mClearBufferSemaphore.wait();
    return true;
}

bool MediaDecoder::resume()
{
    mVideoDecodeSemaphore.signal();
    mAudioDecodeSemaphore.signal();
    mPacketUnpackSemaphore.signal();

    return true;
}

bool MediaDecoder::stop()
{
    resume();
    mIsStop = true;
    return true;
}

FrameReader * MediaDecoder::get_video_frame_reader()
{
    return new FrameReader(&mpVideoQueue, &mpVideoFrameCachePool, &pPacketReader);
}

FrameReader * MediaDecoder::get_audio_frame_reader()
{
    return new FrameReader(&mpAudioQueue, &mpAudioFrameCachePool, &pPacketReader);
}

void MediaDecoder::clear_packet_buffer()
{
    Log::get_instance().log_error("[Disco]MediaDecoder::clear_packet_buffer\n");
    PacketWrapper * packet_wrapper;
    packet_wrapper = mVideoWrapperPacketQueue.non_block_pop_node();
    while (packet_wrapper != nullptr)
    {
        mVideoPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
        packet_wrapper = mVideoWrapperPacketQueue.non_block_pop_node();
    }

    packet_wrapper = mAudioWrapperPacketQueue.non_block_pop_node();
    while (packet_wrapper != nullptr)
    {
        mVideoPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
        packet_wrapper = mAudioWrapperPacketQueue.non_block_pop_node();
    }
}

void MediaDecoder::clear_frame_buffer(FrameQueue* frame_queue, 
        FrameConcurrentCachePool* frame_cache_pool,
        AVCodecContext * codec_context)
{
    Log::get_instance().log_error("[Disco]MediaDecoder::clear_frame_buffer clear frame_queue\n");
    FrameWrapper * frame_wrapper = frame_queue->non_block_pop_node();
    while (frame_wrapper != nullptr)
    {
        frame_cache_pool->recycle_node(frame_wrapper);
        frame_wrapper = frame_queue->non_block_pop_node();
    }

    Log::get_instance().log_error("[Disco]MediaDecoder::clear_frame_buffer avcodec_flush_buffers start\n");
    avcodec_flush_buffers(codec_context);
    Log::get_instance().log_error("[Disco]MediaDecoder::clear_frame_buffer avcodec_flush_buffers end\n");

}