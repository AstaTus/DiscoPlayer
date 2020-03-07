#include "AudioDeviceBuffer.h"

#include "AudioDeviceBufferProcessor.h"
#include "AudioDeviceLastBufferProcessor.h"
#include "AudioDataRequestListener.h"
#include "AudioClip.h"
#include "AudioTransformNode.h"
#include "../../codec/FrameWrapper.h"
extern "C"
{
    #include "libavutil/frame.h"
    #include "libavformat/avformat.h"
}

AudioDeviceBuffer::AudioDeviceBuffer()
    : mpRootAudioDeviceBufferProcessor(nullptr),
    mpAudioDataRequestListener(nullptr)
{
}

AudioDeviceBuffer::~AudioDeviceBuffer()
{
}

void AudioDeviceBuffer::push_back_porcessor(AudioDeviceBufferProcessor *processor)
{
    if (mpRootAudioDeviceBufferProcessor == nullptr)
    {
        mpRootAudioDeviceBufferProcessor = processor;
    } else {
        mpRootAudioDeviceBufferProcessor->push_back_child_processor(processor);
    }
}

void AudioDeviceBuffer::push_back_last_porcessor(AudioDeviceLastBufferProcessor *processor)
{
    mpLastAudioDeviceBufferProcessor = processor;
    push_back_porcessor(processor);
}

void AudioDeviceBuffer::set_audio_data_request_listener(
        AudioDataRequestListener * audio_data_request_listener)
{
    mpAudioDataRequestListener = audio_data_request_listener;
}

void AudioDeviceBuffer::fill_target_buffer(uint8_t * buffer, uint32_t size, float speed, float volume, AVSampleFormat out_format)
{
    mpLastAudioDeviceBufferProcessor->set_target_buffer(buffer, size);

    AudioTransformNode * audio_node = nullptr;
	bool same_serial = false;
    mpAudioDataRequestListener->on_audio_data_request_begin();
	
	while (!mpLastAudioDeviceBufferProcessor->is_target_buffer_full())
	{
        if(mpLastAudioDeviceBufferProcessor->fill_buffer_by_cache(volume)) {
            
        } else {
            same_serial = mpAudioDataRequestListener->on_audio_data_request(&audio_node);
            if (!same_serial)
            {	
                mpRootAudioDeviceBufferProcessor->delivery_clear_buffer();
                break;
            } 
            else if (audio_node == nullptr)
            {
                continue;
            }
            process_audio_node(audio_node, speed, volume, out_format);
        }
	}

	mpAudioDataRequestListener->on_audio_data_request_end();
}

long AudioDeviceBuffer::process_audio_node(AudioTransformNode * node, float speed, float volume, AVSampleFormat out_format)
{
    AudioClip * clip = node->clip;
    AVFrame * frame = node->frame_wrapper->frame;
    int bytes_per_sample = av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format));

    mpRootAudioDeviceBufferProcessor->delivery_process_buffer(
        clip->data(), clip->size(), frame->sample_rate, frame->nb_samples, 
        frame->channels, speed, bytes_per_sample, out_format, volume);
}

uint32_t AudioDeviceBuffer::get_cache_buffer_size()
{
    return mpRootAudioDeviceBufferProcessor->get_delivery_cache_buffer_size();
}