#ifndef __DISCO_FRAME_H__
#define __DISCO_FRAME_H__

extern "C"
{
    #include "libavutil/frame.h"
    #include "libavutil/avutil.h"
}

struct FrameWrapper
{
    AVFrame *frame;
    AVRational time_base;

    FrameWrapper()
        : frame(nullptr),
          time_base()
    {
        frame = new AVFrame();
    }

    ~FrameWrapper()
    {
        delete frame;
    }
};
#endif