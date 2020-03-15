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

    /**
     * 返回毫秒
    */
    double get_transformed_pts()
    {
        if(frame != nullptr) {
            return frame->pts * 1000 * av_q2d(time_base);
        }
        return 0.0;
    }
};
#endif