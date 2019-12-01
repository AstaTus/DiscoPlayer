#ifndef __VIDEO_RENDER_H__
#define __VIDEO_RENDER_H__

#include <unistd.h>
#include<iostream>

#include "IRenderView.h"
class AVFrame;


class VideoRender
{
private:
    //TODO 双buf
    // uint8_t * const pBuffer1;
    // uint8_t * const pBuffer2;
    //渲染的view
    IRenderView * const pRenderView;

    // TODO 转换好的frame cache
    

public:
    VideoRender(IRenderView * const render_view);
    ~VideoRender();

    void refresh(const uint8_t * const data, int pitch);
};

#endif