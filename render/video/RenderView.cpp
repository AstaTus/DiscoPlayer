#include "RenderView.h"
#include "VideoFrameTransformer.h"
RenderView::RenderView(int width, int height) 
: mWidth(width),
mHeight(height),
mpVideoFrameTransformer(nullptr)
{
}

void RenderView::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
    if (mpVideoFrameTransformer != nullptr)
    {
        //mpVideoFrameTransformer->on_resize_render_view(mWidth, mHeight);
    }
}

void RenderView::invalid_image()
{
}

int RenderView::get_width()
{
    return mWidth;
}
int RenderView::get_height()
{
    return mHeight;
}

void RenderView::set_video_frame_transformer(VideoFrameTransformer *video_frame_transformer)
{
    if (video_frame_transformer != nullptr)
    {
        mpVideoFrameTransformer = video_frame_transformer;
        mpVideoFrameTransformer->on_resize_render_view(mWidth, mHeight);
    }
}