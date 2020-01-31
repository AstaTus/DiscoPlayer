#ifndef __SEEKING_STATE_H__
#define __SEEKING_STATE_H__

#include "BaseState.h"

class AudioDevice;
class MediaInputStream;
class MediaDecoder;
class VideoFrameTransformer;
class AudioFrameTransformer;
class ActivateNodeManager;
class SeekingState : public BaseState
{
private:
    AudioDevice * const mpAudioDevice;
    MediaInputStream * const mpInputStream;
    MediaDecoder * const mpMediaDecoder;
    VideoFrameTransformer * const mpVideoFrameTransformer;
    AudioFrameTransformer * const mpAudioFrameTransformer;
    ActivateNodeManager * const mpActivateNodeManager;

    /* data */
public:
    SeekingState(AudioDevice * audio_device, 
                MediaInputStream * input_stream,
                MediaDecoder * media_decoder,
                VideoFrameTransformer * video_frame_transformer,
                AudioFrameTransformer * audio_frame_transformer,
                ActivateNodeManager * activate_node_manager);
    ~SeekingState();

    void on_state_enter() override;
    void on_state_exit() override;
};

#endif
