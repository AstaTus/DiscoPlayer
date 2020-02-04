#include "RenderSynchronizer.h"
#include "../render/video/VideoFrameTransformer.h"
#include "../render/audio/AudioFrameTransformer.h"
#include "../codec/FrameWrapper.h"
#include "RenderSynchronizerFinishListener.h"
#include "../common/thread/ThreadUtils.h"

RenderSynchronizer::RenderSynchronizer(VideoFrameTransformer * video_frame_transformer, 
        AudioFrameTransformer * audio_frame_transformer,
        RenderSynchronizerFinishListener * finish_listener)
        :mpVideoFrameTransformer(video_frame_transformer),
        mpAudioFrameTransformer(audio_frame_transformer),
        mpFinishListener(finish_listener)
{
}

RenderSynchronizer::~RenderSynchronizer()
{
}

void RenderSynchronizer::start(int serial)
{
    mSerial = serial;
    mSynchronizerFutrue = std::async(std::launch::async, &RenderSynchronizer::wait_for_synchronizer, this);
}

void RenderSynchronizer::wait_for_synchronizer()
{
    ThreadUtils::set_thread_name("Disco Render Synchronizer");
    VideoTransformNode * video_node = mpVideoFrameTransformer->block_peek_transformed_node();
    while (video_node->frame_wrapper->serial != mSerial)
    {
        mpVideoFrameTransformer->block_pop_transformed_node();
        mpVideoFrameTransformer->recycle(video_node);
        video_node = mpVideoFrameTransformer->block_peek_transformed_node();
    }

    AudioTransformNode * audio_node = mpAudioFrameTransformer->block_peek_transformed_node();
    while (audio_node->frame_wrapper->serial != mSerial)
    {
        mpAudioFrameTransformer->block_pop_transformed_node();
        mpAudioFrameTransformer->recycle(audio_node);
        audio_node = mpAudioFrameTransformer->block_peek_transformed_node();
    }

    mpFinishListener->on_synchronizer_finish();
}