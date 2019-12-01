#include "VideoRender.h"



VideoRender::VideoRender(IRenderView *const render_view)
    : pRenderView(render_view)
{
}

VideoRender::~VideoRender()
{
}

void VideoRender::refresh(const uint8_t * const data, int pitch)
{
    pRenderView->refresh(data, pitch);
}