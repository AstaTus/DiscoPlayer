#include "AudioTransformProcessor.h"
#include "AudioClip.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
};

const uint64_t AudioTransformProcessor::OUT_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
const AVSampleFormat AudioTransformProcessor::OUT_SAMPLE_FORMAT = AV_SAMPLE_FMT_S16;
const int AudioTransformProcessor::OUT_SAMPLE_RATE = 48000;

AudioTransformProcessor::AudioTransformProcessor(/* args */)
    : pSwrContext(nullptr),
      mCurrentChannelLayout(AV_CH_LAYOUT_STEREO),
      mCurrentSampleRate(0),
      mCurrentSampleFormat(0)

{
}

AudioTransformProcessor::~AudioTransformProcessor()
{
    if (pSwrContext != nullptr)
    {
        swr_free(&pSwrContext);
        pSwrContext = nullptr;
    }
}

void AudioTransformProcessor::transform_by_ffmpeg(AVFrame *frame,
                                                  AudioClip *const clip)
{

    clip->reformat(OUT_CHANNEL_LAYOUT, frame->nb_samples, OUT_SAMPLE_FORMAT);
    fit_swr_context(frame->channel_layout, static_cast<AVSampleFormat>(frame->format), frame->sample_rate);

    //音频重采样
    int len = swr_convert(pSwrContext, (uint8_t **)(clip->data_ptr()),
                          frame->nb_samples,
                          (const uint8_t **)frame->data,
                          frame->nb_samples);

    //TODO audio clip‘s size
}

void AudioTransformProcessor::fit_swr_context(uint64_t channel_layout,
                                              AVSampleFormat sample_format, int sample_rate)
{

    if (pSwrContext == nullptr || channel_layout != mCurrentChannelLayout ||
        sample_format != mCurrentSampleFormat || sample_rate != mCurrentSampleRate)
    {
        if (pSwrContext != nullptr)
        {
            swr_free(&pSwrContext);
            pSwrContext = nullptr;
        }

        pSwrContext = swr_alloc();
        pSwrContext = swr_alloc_set_opts(
            pSwrContext,
            OUT_CHANNEL_LAYOUT, //输出
            OUT_SAMPLE_FORMAT,  //编码前你希望的格式
            OUT_SAMPLE_RATE,    //输出
            channel_layout,     //输入
            sample_format,      //PCM源文件的采样格式
            sample_rate,        //输入
            0, nullptr);
        swr_init(pSwrContext);

        mCurrentChannelLayout = channel_layout;
        mCurrentSampleRate = sample_rate;
        mCurrentSampleFormat = sample_format;
    }
}