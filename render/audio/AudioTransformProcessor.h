#ifndef __AUDIO_TRANSFORM_PROCESSOR_H__
#define __AUDIO_TRANSFORM_PROCESSOR_H__


class AudioClip;
struct SwrContext;

extern "C"
{
    #include "libavutil/samplefmt.h"
}
struct AVFrame;
class AudioTransformProcessor
{
private:
    static const uint64_t OUT_CHANNEL_LAYOUT;
    static const AVSampleFormat OUT_SAMPLE_FORMAT;

    SwrContext *pSwrContext;
    uint64_t mCurrentChannelLayout;
    int mCurrentSampleRate;
    AVSampleFormat mCurrentSampleFormat;

    void fit_swr_context(uint64_t channel_layout, AVSampleFormat sample_format, int sample_rate);

public:
    AudioTransformProcessor(/* args */);
    ~AudioTransformProcessor();

    void transform_by_ffmpeg(AVFrame *frame, AudioClip *const clip);

    int get_output_sample_rate();

    AVSampleFormat get_output_sample_format();

    int get_output_channel_num();
};

#endif