#pragma once
#include "VideoRenderTransformProcessor.h"

class YUV2RGBTransformProcessor : public VideoRenderTransformProcessor
{
private:
    /* data */
public:
    YUV2RGBTransformProcessor(/* args */);
    ~YUV2RGBTransformProcessor();

    virtual VideoTransformNode * process(VideoTransformNode * node);

    virtual bool is_need_process(VideoTransformNode * node);
};