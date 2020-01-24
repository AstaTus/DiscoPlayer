#include "RenderView.h"

RenderView::RenderView(int width, int height) 
: mWidth(width),
mHeight(height)
{
}

void RenderView::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
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