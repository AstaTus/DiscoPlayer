#ifndef __DISCO_AUDIO_CLIP_H__
#define __DISCO_AUDIO_CLIP_H__

extern "C"
{
    #include "libavutil/samplefmt.h"
}
class AudioClip
{
private:
    uint8_t * pBuffer;

    int mNbSamples;
    uint64_t mChannelLayout;
    AVSampleFormat mSampleFormat;

    int mSize;
public:
    AudioClip(/* args */);
    ~AudioClip();

    /**
     * @param channel_num 声道数
     * @param nb_samples 采样数
     * @param sample_format 采样格式
     */
    void reformat(uint64_t channel_layout, int nb_samples, AVSampleFormat sample_format);

    uint8_t * const data() const;

    uint8_t ** data_ptr();
    int size() const;
};

#endif