#ifndef __FRAME_TRANSFORM_H__
#define __FRAME_TRANSFORM_H__
#include <iostream>
#include "VideoHeader.h"
#include "ImageCachePool.h"
class AVFrame;
class Image;

extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

class FrameTransform
{
private:
    //视频目标宽高
    int mScaleDstWidth;
    int mScaleDstHeight;
    //视频原始宽高
    int mScaleSrcWidth;
    int mScaleSrcHeight;
    //视频原始的像素格式
    AVPixelFormat mScalePixelFormat;

    //当前需要的视频宽高
    int mCurrentVideoWidth;
    int mCurrentVideoHeight;

    struct SwsContext *pSwsContext;

    SwsContext * get_sws_context(int dst_width, 
        int dst_height, int src_width, int src_height, AVPixelFormat src_format);

public:
    FrameTransform(/* args */);
    ~FrameTransform();

    void transform(AVFrame *frame, Image * const image);

    /**调整 视频目标宽高
     * @param width 目标宽度
     * @param height 目标高度
    */
    void resize_target(int width, int height);
};

#endif
