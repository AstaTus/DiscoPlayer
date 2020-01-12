#include "AudioClip.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/audio_fifo.h"
}

AudioClip::AudioClip(/* args */)
    : pBuffer(nullptr),
      mSampleFormat(AV_SAMPLE_FMT_NONE),
      mNbSamples(0),
      mSize(0),
      mBufferPos(0),
      mChannelLayout(AV_CH_LAYOUT_STEREO)
{
}

AudioClip::~AudioClip()
{
    if (pBuffer != nullptr)
    {
        delete pBuffer;
        pBuffer = nullptr;
    }
}

void AudioClip::reformat(uint64_t channel_layout, int nb_samples, AVSampleFormat sample_format)
{
    if (pBuffer == nullptr || mNbSamples != nb_samples || mChannelLayout != channel_layout || mSampleFormat != sample_format)
    {
        if (pBuffer != nullptr)
        {
            delete pBuffer;
            pBuffer = nullptr;
        }

        int nb_channels = av_get_channel_layout_nb_channels(channel_layout);
        mSize = av_samples_get_buffer_size(nullptr, nb_channels, nb_samples, sample_format, 0);
        pBuffer = new uint8_t[mSize];

        mNbSamples = nb_samples;
        mChannelLayout = channel_layout;
        mSampleFormat = sample_format;
    }

    mBufferPos = 0;
}

uint8_t *const AudioClip::data() const
{
    return pBuffer + mBufferPos;
}

uint8_t ** AudioClip::data_ptr()
{
    return &pBuffer;
}

int AudioClip::size() const
{
    return (mSize - mBufferPos);
}

bool AudioClip::isFinish()
{
    return mSize == mBufferPos;
}

void AudioClip::set_buffer_pos(int pos)
{
    mBufferPos = pos;
}