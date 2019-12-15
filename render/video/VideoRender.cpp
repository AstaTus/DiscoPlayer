#include "VideoRender.h"

#include "Image.h"

VideoRender::VideoRender(RenderView *const render_view)
    : pRenderView(render_view)
{
}

VideoRender::~VideoRender()
{
}

void VideoRender::refresh(const Image * const image)
{
    pRenderView->refresh(image->data(), image->pitch());
}