#pragma once
#include "VideoRenderTransformProcessor.h"

class SuperResolutionTransformProcessor: public VideoRenderTransformProcessor
{
private:
    /* data */
public:
    SuperResolutionTransformProcessor(/* args */);
    ~SuperResolutionTransformProcessor();

    virtual VideoTransformNode * process(VideoTransformNode * node);

    virtual bool is_need_process(VideoTransformNode * node);
};

