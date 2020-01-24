#ifndef __FRAME_TRANSFORM_H__
#define __FRAME_TRANSFORM_H__
#include <iostream>
#include "VideoHeader.h"

struct AVFrame;
class Image;

class VideoTransformProcessor
{
public:
    VideoTransformProcessor(/* args */);
    ~VideoTransformProcessor();

    void transform_by_libyuv(AVFrame *frame, Image * const image, int width, int height);
};

#endif
