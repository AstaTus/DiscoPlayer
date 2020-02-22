#ifndef __IRENDER_VIEW_H__
#define __IRENDER_VIEW_H__
#include <iostream>
class Image;
class VideoFrameTransformer;
class RenderView
{
protected:
    int mWidth;
    int mHeight;
    VideoFrameTransformer *mpVideoFrameTransformer;

public:
    RenderView(int width, int height);
    virtual ~RenderView() = default;

    virtual void refresh(const Image * const image) = 0;

    virtual void resize(int width, int height);

    //用于异步处理，在image回首时调用，让渲染窗口在image回收后，不异常调用
    virtual void invalid_image();

    int get_width();
    int get_height();
    void set_video_frame_transformer(VideoFrameTransformer *video_frame_transformer);
};

#endif