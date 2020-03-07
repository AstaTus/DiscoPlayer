// #include "SoundTouchDeviceBufferProcessor.h"
// #include "./soundtouch/SoundTouch.h"

// extern "C"
// {
// #include "libavutil/mem.h"
// }

// SoundTouchDeviceBufferProcessor::SoundTouchDeviceBufferProcessor(/* args */)
// :mpSoundTouch(nullptr)
// {
//     mpSoundTouch = new soundtouch::SoundTouch();
// }

// SoundTouchDeviceBufferProcessor::~SoundTouchDeviceBufferProcessor()
// {
//     if (mpSoundBuffer != nullptr)
//     {
//         /* code */
//     }

//     if (mpSoundTouch != nullptr)
//     {
//         mpSoundTouch->clear();
//         delete mpSoundTouch;
//         mpSoundTouch = nullptr;
//     }
// }

// long SoundTouchDeviceBufferProcessor::process_buffer(uint8_t * buffer, long buffer_size, long need_size, 
//     int sample_rate, int sample_size, int channel_size, float speed, int bytes_per_sample, enum AVSampleFormat out_format)
// {
//     //TODO wanted_sample_rate is does't same orginal sample rate?
//     float wanted_sample_rate = sample_rate;
//     int out_sample_size = (int)(sample_size * wanted_sample_rate / sample_rate + 256);
//     int out_buffer_size = av_samples_get_buffer_size(nullptr, channel_size, out_sample_size, out_format, 0);
//     av_fast_malloc(&mpSoundBuffer, &mSoundBufferSize, out_buffer_size);

//     // 将解码后的buffer(uint8*)转换为soundtouch::SAMPLETYPE，也就是singed int 16
    
//     auto len = sample_size * channel_size * bytes_per_sample;
//     for (auto i = 0; i < len / 2; i++)
//     {
//         mpSoundBuffer[i] = (short)(buffer[i * 2] | (buffer[i * 2 + 1] << 8));
//     }

//     // mpSoundTouch->setPitch(pitch);
//     mpSoundTouch->setRate(speed);

//     mpSoundTouch->setSampleRate(sample_rate);
//     mpSoundTouch->setChannels(channel_size);

//     auto receive_data_size = 0;
//     auto multiple = 1;

//     clip->init_sound_touch_data(multiple);
//     mpSoundTouch->putSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size);

//     uint nb_receive_sample = mpSoundTouch->receiveSamples((soundtouch::SAMPLETYPE*)mpSoundBuffer, sample_size);
    
//     return nb_receive_sample * channel_size * bytes_per_sample;
// }