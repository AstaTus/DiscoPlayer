#include "SoundTouchDeviceBufferProcessor.h"
#include "../../common/type/NumberUtils.h"
extern "C"
{
#include "libavutil/mem.h"
}
#include "../../dependency/sound-touch/2.1.2/include/soundtouch/soundtouch_config.h"
#include "../../dependency/sound-touch/2.1.2/include/soundtouch/SoundTouch.h"
#include "../../common/log/Log.h"
#include "./WavFile.h"

#define BUFF_SIZE           6720

SoundTouchDeviceBufferProcessor::SoundTouchDeviceBufferProcessor(/* args */)
:mpSoundTouch(nullptr),
mpSoundBuffer(nullptr),
mSoundTouchPipeCacheSize(0),
mOutputBuffer(nullptr),
mOutputArraySize(0),
mOutputWritePos(0),
mBytesPerSample(0),
mpWavFile(nullptr),
mpInWavFile(nullptr)
{
    mpSoundTouch = new soundtouch::SoundTouch();

    mSoundBufferSize = 5120;
    mpSoundBuffer = (short*)av_malloc(mSoundBufferSize);

    Log::get_instance().log_debug("[Disco]SoundTouchDeviceBufferProcessor::SOUNDTOUCH_INTEGER_SAMPLES = %d", SOUNDTOUCH_INTEGER_SAMPLES);
}

bool SoundTouchDeviceBufferProcessor::is_need_process(uint8_t * buffer, long buffer_size, 
        int sample_rate, int sample_size, int channel_size, float speed, 
        int bytes_per_sample, enum AVSampleFormat out_format, int volume)
{
    if (NumberUtils::float_equal(speed, 1.0f))
    {
        return false;
    }

    return true;
}

SoundTouchDeviceBufferProcessor::~SoundTouchDeviceBufferProcessor()
{
    if (mpSoundBuffer != nullptr)
    {
        /* code */
    }

    if (mpSoundTouch != nullptr)
    {
        mpSoundTouch->clear();
        delete mpSoundTouch;
        mpSoundTouch = nullptr;
    }
}

// //保存 原始buffer到wav
// uint32_t SoundTouchDeviceBufferProcessor::process_buffer(uint8_t * buffer, long buffer_size, 
//     int sample_rate, int sample_size, int channel_size, float speed, 
//     int bytes_per_sample, enum AVSampleFormat out_format, int volume, uint8_t ** out_buffer)
// {
//     //TODO wanted_sample_rate is does't same orginal sample rate?
//     mBytesPerSample = bytes_per_sample;
//     // 将解码后的buffer(uint8*)转换为soundtouch::SAMPLETYPE，也就是singed int 16
//     for (auto i = 0; i < buffer_size / 2; i++)
//     {
//         mpSoundBuffer[i] = (short)(buffer[i * 2] | (buffer[i * 2 + 1] << 8));
//     }

//     // if(mpInWavFile == nullptr) {
//     //     mpInWavFile = new WavInFile("./IN_test.wav");
//     // }
//     static int count = 0;
//     if (mpWavFile == nullptr)
//     {
//         // mpWavFile = new WavOutFile("./test.wav", mpInWavFile->getSampleRate(), mpInWavFile->getNumBits(), mpInWavFile->getNumChannels());
//         mpWavFile = new WavOutFile("./source_test.wav", sample_rate, bytes_per_sample * 8, channel_size);
//     }

//     mpWavFile->write(mpSoundBuffer, sample_size * channel_size);
//     ++count;
//     if(count == 400) {
//         delete mpWavFile;
//     }

//     mSoundTouchPipeCacheSize += buffer_size;
//     mpSoundTouch->setTempo(speed);
//     mpSoundTouch->setSampleRate(sample_rate);
//     mpSoundTouch->setChannels(channel_size);

//     mpSoundTouch->putSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size);
//     uint32_t nb = 0;
//     uint32_t pcm_data_size = 0;
//     mOutputWritePos = 0;
//     do {
//         nb = mpSoundTouch->receiveSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size);
//         pcm_data_size = nb * channel_size * bytes_per_sample;
        
//         if (pcm_data_size > 0)
//         {
//             if (mOutputArraySize < mOutputWritePos + pcm_data_size + 256)
//             {
//                 uint8_t * newOutputBuffer = nullptr;
//                 av_fast_malloc(&newOutputBuffer, &mOutputArraySize,  mOutputWritePos + pcm_data_size + 256);
//                 if (mOutputWritePos > 0)
//                 {
//                     memcpy(newOutputBuffer, mOutputBuffer, mOutputWritePos);
//                 }

//                 if (mOutputBuffer != nullptr)
//                 {
//                     delete [] mOutputBuffer;
//                 }

//                 mOutputBuffer = newOutputBuffer;
//             }

//             memcpy(mOutputBuffer + mOutputWritePos, mpSoundBuffer, pcm_data_size);
//             mOutputWritePos += pcm_data_size;
//         }
//     } while (nb != 0);
//     mSoundTouchPipeCacheSize -= mOutputWritePos;
//     (*out_buffer) = mOutputBuffer;
//     return mOutputWritePos;
// }

//将外部wav经过soundtouch处理 保存成wav
// uint32_t SoundTouchDeviceBufferProcessor::process_buffer(uint8_t * buffer, long buffer_size, 
//     int sample_rate, int sample_size, int channel_size, float speed, 
//     int bytes_per_sample, enum AVSampleFormat out_format, int volume, uint8_t ** out_buffer)
// {
//     //TODO wanted_sample_rate is does't same orginal sample rate?
//     mBytesPerSample = bytes_per_sample;
//     // 将解码后的buffer(uint8*)转换为soundtouch::SAMPLETYPE，也就是singed int 16
//     for (auto i = 0; i < buffer_size / 2; i++)
//     {
//         mpSoundBuffer[i] = (short)(buffer[i * 2] | (buffer[i * 2 + 1] << 8));
//     }

//     if(mpInWavFile == nullptr) {
//         mpInWavFile = new WavInFile("./source_test.wav");
//     }

//     if (mpWavFile == nullptr)
//     {
//         mpWavFile = new WavOutFile("./sound_touch_test.wav", mpInWavFile->getSampleRate(), mpInWavFile->getNumBits(), mpInWavFile->getNumChannels());
//     }

//     mSoundTouchPipeCacheSize += buffer_size;
//     mpSoundTouch->setTempo(speed);
//     mpSoundTouch->setSampleRate(mpInWavFile->getSampleRate());
//     mpSoundTouch->setChannels(mpInWavFile->getNumChannels());

//     static int count = 0;

//     int nSamples;
//     int nChannels;
//     int buffSizeSamples;
//     soundtouch::SAMPLETYPE sampleBuffer[BUFF_SIZE];
//     nChannels = (int)mpInWavFile->getNumChannels();
//     assert(nChannels > 0);
//     buffSizeSamples = BUFF_SIZE / nChannels;
//     while (mpInWavFile->eof() == 0)
//     {
//         int num;
//         num = mpInWavFile->read(sampleBuffer, BUFF_SIZE);
//         nSamples = num / (int)mpInWavFile->getNumChannels();
      
//         mpSoundTouch->putSamples(sampleBuffer, nSamples);
//         do 
//         {
//             nSamples = mpSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
//             mpWavFile->write(sampleBuffer, nSamples * nChannels);
//         } while (nSamples != 0);
//     }
//     mpSoundTouch->flush();
//     do 
//     {
//         nSamples = mpSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
//         mpWavFile->write(sampleBuffer, nSamples * nChannels);
//     } while (nSamples != 0);
//     delete mpWavFile;

//     return mOutputWritePos;
//     // mpSoundTouch->putSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size  /2);
//     // uint32_t nb = 0;
//     // uint32_t pcm_data_size = 0;
//     // mOutputWritePos = 0;
//     // do {
//     //     memset(mpSoundBuffer, 0, mSoundBufferSize);
//     //     nb = mpSoundTouch->receiveSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size);
//     //     pcm_data_size = nb * channel_size * bytes_per_sample;
        
//     //     if (pcm_data_size > 0)
//     //     {
//     //         mpWavFile->write(mpSoundBuffer, sample_size * channel_size);
//     //         ++count;
//     //         if(count == 200) {
//     //             delete mpWavFile;
//     //         } 

//     //         if (mOutputArraySize < mOutputWritePos + pcm_data_size + 256)
//     //         {
//     //             uint8_t * newOutputBuffer = nullptr;
//     //             av_fast_malloc(&newOutputBuffer, &mOutputArraySize,  mOutputWritePos + pcm_data_size + 256);
//     //             if (mOutputWritePos > 0)
//     //             {
//     //                 memcpy(newOutputBuffer, mOutputBuffer, mOutputWritePos);
//     //             }

//     //             if (mOutputBuffer != nullptr)
//     //             {
//     //                 delete [] mOutputBuffer;
//     //             }

//     //             mOutputBuffer = newOutputBuffer;
//     //         }

//     //         memcpy(mOutputBuffer + mOutputWritePos, mpSoundBuffer, pcm_data_size);
//     //         mOutputWritePos += pcm_data_size;
//     //     }
//     // } while (nb != 0);
//     // mSoundTouchPipeCacheSize -= mOutputWritePos;
//     // (*out_buffer) = mOutputBuffer;
//     // return mOutputWritePos;
// }



uint32_t SoundTouchDeviceBufferProcessor::process_buffer(uint8_t * buffer, long buffer_size, 
    int sample_rate, int sample_size, int channel_size, float speed, 
    int bytes_per_sample, enum AVSampleFormat out_format, int volume, uint8_t ** out_buffer)
{
    //TODO wanted_sample_rate is does't same orginal sample rate?
    mBytesPerSample = bytes_per_sample;
    // 将解码后的buffer(uint8*)转换为soundtouch::SAMPLETYPE，也就是singed int 16
    for (auto i = 0; i < buffer_size / 2; i++)
    {
        mpSoundBuffer[i] = (short)(buffer[i * 2] | (buffer[i * 2 + 1] << 8));
    }

    // if(mpInWavFile == nullptr) {
    //     mpInWavFile = new WavInFile("./IN_test.wav");
    // }

    // if (mpWavFile == nullptr)
    // {
    //     // mpWavFile = new WavOutFile("./test.wav", mpInWavFile->getSampleRate(), mpInWavFile->getNumBits(), mpInWavFile->getNumChannels());
    //     mpWavFile = new WavOutFile("./source_buff_test.wav", sample_rate, bytes_per_sample * 8, channel_size);
    // }
    mSoundTouchPipeCacheSize += buffer_size;
    mpSoundTouch->setTempo(speed);
    mpSoundTouch->setSampleRate(sample_rate);
    mpSoundTouch->setChannels(channel_size);

    mpSoundTouch->putSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size);
    uint32_t nb = 0;
    uint32_t pcm_data_size = 0;
    mOutputWritePos = 0;
    do {
        memset(mpSoundBuffer, 0, mSoundBufferSize);
        nb = mpSoundTouch->receiveSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size);
        pcm_data_size = nb * channel_size * bytes_per_sample;
        
        if (pcm_data_size > 0)
        {
            // mpWavFile->write(mpSoundBuffer, sample_size * channel_size);
            // ++count;
            // if(count == 200) {
            //     delete mpWavFile;
            // } 

            if (mOutputArraySize < mOutputWritePos + pcm_data_size + 256)
            {
                uint8_t * newOutputBuffer = nullptr;
                av_fast_malloc(&newOutputBuffer, &mOutputArraySize,  mOutputWritePos + pcm_data_size + 256);
                if (mOutputWritePos > 0)
                {
                    memcpy(newOutputBuffer, mOutputBuffer, mOutputWritePos);
                }

                if (mOutputBuffer != nullptr)
                {
                    delete [] mOutputBuffer;
                }

                mOutputBuffer = newOutputBuffer;
            }

            memcpy(mOutputBuffer + mOutputWritePos, mpSoundBuffer, pcm_data_size);
            mOutputWritePos += pcm_data_size;
        }
    } while (nb != 0);
    mSoundTouchPipeCacheSize -= mOutputWritePos;
    (*out_buffer) = mOutputBuffer;
    return mOutputWritePos;
}

void SoundTouchDeviceBufferProcessor::clear_buffer()
{
    mpSoundTouch->flush();
    //TODO fix magic number
    mpSoundTouch->receiveSamples(100000);
    mSoundTouchPipeCacheSize = 0;
}

//当前BufferProcessor缓冲区的buffsize
uint32_t SoundTouchDeviceBufferProcessor::get_cache_buffer_size()
{
    return mpSoundTouch->numUnprocessedSamples() * mpSoundTouch->getInputOutputSampleRatio() * mBytesPerSample;
}