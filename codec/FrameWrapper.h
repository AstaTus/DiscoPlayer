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
    int64_t start_pts;
    int serial;
    bool is_end;

    FrameWrapper()
        : frame(nullptr),
          time_base(),
          start_pts(0),
          serial(0),
          is_end(false)
    {
        frame = av_frame_alloc();
    }

    ~FrameWrapper()
    {
        av_frame_free(&frame);
    }
};
#endif