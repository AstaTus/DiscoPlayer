#ifndef __AUDIO_TRANSFORM_NODE_H__
#define __AUDIO_TRANSFORM_NODE_H__

class AudioClip;
struct FrameWrapper;

struct AudioTransformNode
{
    AudioClip *clip;
    FrameWrapper *frame_wrapper;
    bool is_end = false;
};

#endif
