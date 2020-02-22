#include "RenderSynchronizer.h"
#include "../render/video/VideoFrameTransformer.h"
#include "../render/audio/AudioFrameTransformer.h"
#include "../codec/FrameWrapper.h"
#include "RenderSynchronizerFinishListener.h"
#include "../common/thread/ThreadUtils.h"
#include "../common/log/Log.h"
RenderSynchronizer::RenderSynchronizer(VideoFrameTransformer *video_frame_transformer,
                                       AudioFrameTransformer *audio_frame_transformer,
                                       RenderSynchronizerFinishListener *finish_listener)
    : mpVideoFrameTransformer(video_frame_transformer),
      mpAudioFrameTransformer(audio_frame_transformer),
      mpFinishListener(finish_listener)
{
}

RenderSynchronizer::~RenderSynchronizer()
{
}

void RenderSynchronizer::wait()
{
    mSynchronizerFutrue.wait();
}

void RenderSynchronizer::start_by_end()
{
    Log::get_instance().log_debug("[Disco][RenderSynchronizer::start] by end\n");
    mSynchronizerFutrue = std::async(std::launch::async, &RenderSynchronizer::wait_for_synchronizer_by_end, this);
}

void RenderSynchronizer::start_by_serial(int serial)
{
    Log::get_instance().log_debug("[Disco][RenderSynchronizer::start] serial = %d\n", serial);
    mSerial = serial;
    mSynchronizerFutrue = std::async(std::launch::async, &RenderSynchronizer::wait_for_synchronizer_by_serial, this);
}

void RenderSynchronizer::wait_for_synchronizer_by_end()
{
    Log::get_instance().log_debug("[Disco][RenderSynchronizer::wait_for_synchronizer start] by end\n");
    try
    {
        ThreadUtils::set_thread_name("Disco Render Synchronizer");

        bool is_end = false;

        VideoTransformNode *video_node;
        do
        {
            video_node = mpVideoFrameTransformer->block_pop_transformed_node();
            is_end = video_node->is_end;
            mpVideoFrameTransformer->recycle(video_node);

        } while (!is_end);

        is_end = false;
        AudioTransformNode *audio_node;
        do
        {
            audio_node = mpAudioFrameTransformer->block_pop_transformed_node();
            is_end = audio_node->is_end;
            mpAudioFrameTransformer->recycle(audio_node);
        } while (!is_end);

        mpFinishListener->on_synchronizer_finish();
        Log::get_instance().log_debug("[Disco][RenderSynchronizer::wait_for_synchronizer end]  by end\n");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    Log::get_instance().log_debug("[Disco][RenderSynchronizer::wait_for_synchronizer catch finish]  by end\n");
}

void RenderSynchronizer::wait_for_synchronizer_by_serial()
{
    Log::get_instance().log_debug("[Disco][RenderSynchronizer::wait_for_synchronizer start]\n");
    try
    {
        ThreadUtils::set_thread_name("Disco Render Synchronizer");
        VideoTransformNode *video_node = mpVideoFrameTransformer->block_peek_transformed_node();
        while (video_node->frame_wrapper->serial != mSerial)
        {
            mpVideoFrameTransformer->block_pop_transformed_node();
            mpVideoFrameTransformer->recycle(video_node);
            video_node = mpVideoFrameTransformer->block_peek_transformed_node();
        }

        AudioTransformNode *audio_node = mpAudioFrameTransformer->block_peek_transformed_node();
        while (audio_node->frame_wrapper->serial != mSerial)
        {
            mpAudioFrameTransformer->block_pop_transformed_node();
            mpAudioFrameTransformer->recycle(audio_node);
            audio_node = mpAudioFrameTransformer->block_peek_transformed_node();
        }

        mpFinishListener->on_synchronizer_finish();
        Log::get_instance().log_debug("[Disco][RenderSynchronizer::wait_for_synchronizer end]\n");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    Log::get_instance().log_debug("[Disco][RenderSynchronizer::wait_for_synchronizer catch finish]\n");
}