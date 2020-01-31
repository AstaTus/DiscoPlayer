#include "MediaDecoder.h"
#include "../stream/IStreamIterator.h"
#include "../stream/Reader.h"
#include "../stream/PacketWrapper.h"
#include <future>
#include "../common/log/Log.h"
#include "FrameWrapper.h"
#include "FrameReader.h"
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
      mPacketWrapperConcurrentCachePool(PACKET_QUEUE_CACHE_SIZE),
      mIsVideoPause(false),
      mIsAudioPause(false),
      mIsVideoRequestSeek(false),
      mIsAudioRequestSeek(false)
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
        if (!mIsVideoPause && !mIsAudioPause)
        {
            //TODO 此处可能视频或者音频在外部不消费，此处mPacketConcurrentCachePool达到上线后就会挂起
            PacketWrapper *packet_wrapper = mPacketWrapperConcurrentCachePool.get_empty_node();
            pPacketReader->read(packet_wrapper);
            AVMediaType type = mMediaIndexTypeMap[packet_wrapper->packet->stream_index];
            if (type == AVMEDIA_TYPE_VIDEO)
            {
                mVideoWrapperPacketQueue.push_node(packet_wrapper);
            }
            else if (type == AVMEDIA_TYPE_AUDIO)
            {
                mAudioWrapperPacketQueue.push_node(packet_wrapper);
            }
            else
            {
                //TODO log
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_packet_loop has other stream");
            }
        }
    }
}

void MediaDecoder::unpack_frame_loop(
    ConcurrentQueue<PacketWrapper> * concurrent_queue,
    std::atomic<bool>& is_pause, 
    std::atomic<bool>& is_request_seek)
 {
    int receive_frame_ret = 0;
    int send_packet_ret = 0;
    bool is_continue = true;
    //TODO 状态判断，如果暂停就挂起
    bool is_need_flush_buffer = false;
    while (is_continue)
    {
        if (is_pause)
        {
            if (is_request_seek)
            {
                is_need_flush_buffer = true;
                is_request_seek.store(false);
                is_pause.store(false);
            } else {
                continue;
            }      
        }

        //TODO 与flush冲突?
        PacketWrapper *packet_wrapper = concurrent_queue->block_pop_node();

        if (packet_wrapper->serial != pPacketReader->serial())
        {
            mPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
            continue;
        }
        
        FrameQueue *frame_queue = mFrameQueues[packet_wrapper->packet->stream_index];
        AVCodecContext *codec_context = mDecodes[packet_wrapper->packet->stream_index];
        FrameConcurrentCachePool *frame_pool = mFrameCachePools[packet_wrapper->packet->stream_index];
        AVStream * stream = mStreams[packet_wrapper->packet->stream_index];
        if(is_need_flush_buffer) {
            is_need_flush_buffer = false;
            avcodec_flush_buffers(codec_context);
        }
        send_packet_ret = avcodec_send_packet(codec_context, packet_wrapper->packet);
        if (0 == send_packet_ret)
        {
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet send a valid packet\n");
            FrameWrapper *frame_wrapper = frame_pool->get_empty_node();
            receive_frame_ret = avcodec_receive_frame(codec_context, frame_wrapper->frame);
            if (0 == receive_frame_ret)
            {
                frame_wrapper->serial = packet_wrapper->serial;
                if (frame_wrapper->serial != pPacketReader->serial() && 
                        frame_wrapper->frame->pts * 1000 * av_q2d(stream->time_base) < pPacketReader->serial_start_time())
                {
                    frame_pool->recycle_node(frame_wrapper);
                } else {
                    frame_wrapper->time_base = stream->time_base;
                    frame_queue->push_node(frame_wrapper);
                    Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame receive a valid frame stream_index = %d\n", packet_wrapper->packet->stream_index);
                }
            }
            else if (AVERROR(EAGAIN) == receive_frame_ret)
            {
                frame_pool->recycle_node(frame_wrapper);
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return error code = AVERROR(EAGAIN)\n");
            }
            else if (AVERROR_EOF == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return error code = AVERROR_EOF\n");
            }
            else if (AVERROR(EINVAL) == receive_frame_ret)
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return error code = AVERROR(EINVAL)\n");
            }
            else
            {
                is_continue = false;
                Log::get_instance().log_error("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_receive_frame return other error code = %d\n", receive_frame_ret);
            }
        }
        else if (AVERROR(EAGAIN) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR(EAGAIN)\n");
        }
        else if (AVERROR_EOF == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR_EOF\n");
        }
        else if (AVERROR(EINVAL) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR(EINVAL)\n");
        }
        else if (AVERROR(ENOMEM) == send_packet_ret)
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return error code = AVERROR(ENOMEM)\n");
        }
        else
        {
            is_continue = false;
            Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop avcodec_send_packet return other error code = %d\n", send_packet_ret);
        }

        mPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
    }

    Log::get_instance().log_debug("[Disco]MediaDecoder::unpack_audio_frame_loop unpack_frame_loop thread over");

}

//TODO COMBINE unpack_audio_frame_loop and unpack_video_frame_loop
void MediaDecoder::unpack_audio_frame_loop()
{
    unpack_frame_loop(&mAudioWrapperPacketQueue, mIsAudioPause, mIsAudioRequestSeek);
}

void MediaDecoder::unpack_video_frame_loop()
{
    unpack_frame_loop(&mVideoWrapperPacketQueue, mIsVideoPause, mIsVideoRequestSeek);
}

bool MediaDecoder::pause()
{
    mIsVideoPause.store(true);
    mIsAudioPause.store(true);
    return true;
}

bool MediaDecoder::resume()
{
    mIsVideoRequestSeek.store(true);
    mIsAudioRequestSeek.store(true);
    return true;
}

void MediaDecoder::invalid_cache()
{
    //回收packet cache
    // mAudioWrapperPacketQueue.non_block_pop_node()
    // mPacketWrapperConcurrentCachePool.recycle_node(packet_wrapper);
    //回收frame cache

}

bool MediaDecoder::is_seeking()
{
    return mIsAudioPause || mIsVideoPause;
}

FrameReader * MediaDecoder::get_video_frame_reader()
{
    std::map<AVMediaType, int>::iterator itr = mMediaTypeIndexMap.find(AVMEDIA_TYPE_VIDEO);
    if (itr != mMediaTypeIndexMap.end())
    {
        return new FrameReader(mFrameQueues[itr->second],
            mFrameCachePools[itr->second]);
    }
    //TODO  异常
    return nullptr;
}

FrameReader * MediaDecoder::get_audio_frame_reader()
{
    std::map<AVMediaType, int>::iterator itr = mMediaTypeIndexMap.find(AVMEDIA_TYPE_AUDIO);
    if (itr != mMediaTypeIndexMap.end())
    {
        return new FrameReader(mFrameQueues[itr->second],
            mFrameCachePools[itr->second]);
    }
    //TODO  异常
    return nullptr;
}