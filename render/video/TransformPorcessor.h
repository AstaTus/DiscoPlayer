#ifndef __FRAME_TRANSFORM_H__
#define __FRAME_TRANSFORM_H__
#include <iostream>
#include "VideoHeader.h"

class AVFrame;
class Image;

extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

class TransformPorcessor
{
public:
    TransformPorcessor(/* args */);
    ~TransformPorcessor();

    void transform_by_libyuv(AVFrame *frame, Image * const image, int width, int height);
};

#endif
