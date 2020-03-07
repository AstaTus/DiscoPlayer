#pragma once

extern "C"
{
#include "libavformat/avformat.h"
};

struct AudioTransformNode;
class AudioDeviceBufferProcessor;
class AudioDeviceLastBufferProcessor;
class AudioDataRequestListener;

class AudioDeviceBuffer
{
private:
    AudioDeviceBufferProcessor * mpRootAudioDeviceBufferProcessor;
    AudioDataRequestListener *mpAudioDataRequestListener;
    AudioDeviceLastBufferProcessor * mpLastAudioDeviceBufferProcessor;

    long process_audio_node(AudioTransformNode * node, float speed, float volume, AVSampleFormat out_format);

public:
    AudioDeviceBuffer(/* args */);
    ~AudioDeviceBuffer();

    void push_back_porcessor(AudioDeviceBufferProcessor *processor);

    void push_back_last_porcessor(AudioDeviceLastBufferProcessor *processor);

    void fill_target_buffer(uint8_t * buffer, uint32_t size, float speed, float volume, AVSampleFormat out_format);

    void set_audio_data_request_listener(AudioDataRequestListener * audio_data_request_listener);

    uint32_t get_cache_buffer_size();
};
