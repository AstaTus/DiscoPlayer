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