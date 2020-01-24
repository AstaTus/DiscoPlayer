#ifndef __VIDEO_RENDER_H__
#define __VIDEO_RENDER_H__

#include <unistd.h>
#include<iostream>

#include "RenderView.h"
struct AVFrame;
class Image;

class VideoRender
{
private:
    //TODO 双buf
    // uint8_t * const pBuffer1;
    // uint8_t * const pBuffer2;
    //渲染的view
    RenderView * const pRenderView;

    // TODO 转换好的frame cache
    

public:
    VideoRender(RenderView * const render_view);
    ~VideoRender();

    //
    void refresh(const Image * const image);

    void invalid_image();
};

#endif