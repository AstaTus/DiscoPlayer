#include "FrameTransform.h"


FrameTransform::FrameTransform()
    : mScaleDstWidth(0),
      mScaleDstHeight(0),
      mScaleSrcWidth(0),
      mScaleSrcHeight(0),
      mScalePixelFormat(AV_PIX_FMT_NONE),
      mCurrentVideoWidth(0),
      mCurrentVideoHeight(0),
      pSwsContext(NULL),
      pOutBuffer(NULL)
{
}

FrameTransform::~FrameTransform()
{
}

const uint8_t *const FrameTransform::transform(AVFrame *frame)
{
    SwsContext *sws_context = get_sws_context(mCurrentVideoWidth, mCurrentVideoHeight, 
        frame->width, frame->height, static_cast<AVPixelFormat>(frame->format));
    
    sws_scale(sws_context, 
        (const uint8_t* const*)frame->data, frame->linesize, 0, 
        frame->height, pFrameYUV->data, pFrameYUV->linesize);
}

SwsContext * FrameTransform::get_sws_context(int dst_width, 
    int dst_height, int src_width, int src_height, AVPixelFormat src_format)
{
    if (dst_width != mScaleDstWidth || dst_height != mScaleDstHeight ||
        src_width != mScaleSrcWidth || src_height != mScaleSrcHeight ||
        src_format != mScalePixelFormat || pSwsContext == NULL)
    {
        mScaleDstWidth = dst_width;
        mScaleDstHeight = dst_height;
        mScaleSrcWidth = src_width;
        mScaleSrcHeight = src_height;
        mScalePixelFormat = src_format;

        if (pSwsContext != NULL)
        {
            sws_freeContext(pSwsContext);
            pSwsContext = NULL;
        }

        pSwsContext = sws_getContext(mScaleSrcWidth, mScaleSrcHeight,
            mScalePixelFormat, mScaleDstWidth, mScaleDstHeight, 
            AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

        if (pOutBuffer != NULL)
        {
            /* code */
        }
        
    }

    return pSwsContext;
}

void FrameTransform::resize_target(int width, int height)
{
    mCurrentVideoWidth = width;
    mCurrentVideoHeight = height;
}