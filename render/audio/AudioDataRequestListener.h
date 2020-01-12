#ifndef __AUDIO_DATA_REQUEST_LISTENER_H__
#define __AUDIO_DATA_REQUEST_LISTENER_H__
class AudioClip;

class AudioDataRequestListener
{
private:
    /* data */
public:
    AudioDataRequestListener(/* args */) = default;
    virtual ~AudioDataRequestListener() = default;

    virtual void on_audio_data_request_begin() = 0;
    virtual void on_audio_data_request_end() = 0;
    virtual AudioClip * const on_audio_data_request(int len) = 0;
};
#endif