#include "FrameTransform.h"
#include "Image.h"

FrameTransform::FrameTransform()
    : mScaleDstWidth(0),
      mScaleDstHeight(0),
      mScaleSrcWidth(0),
      mScaleSrcHeight(0),
      mScalePixelFormat(AV_PIX_FMT_NONE),
      mCurrentVideoWidth(0),
      mCurrentVideoHeight(0),
      pSwsContext(NULL)
{
}

FrameTransform::~FrameTransform()
{
}


void FrameTransform::transform(AVFrame *frame, Image * const image)
{
    AVPixelFormat format = static_cast<AVPixelFormat>(frame->format);
    SwsContext *sws_context = get_sws_context(mCurrentVideoWidth, mCurrentVideoHeight, 
        frame->width, frame->height, format);

    image->reformat(format, frame->width, frame->height);

    sws_scale(sws_context, 
        (const uint8_t* const*)frame->data, frame->linesize, 0, 
        frame->height, image->data(), pFrameYUV->linesize);

    return image;
}

void FrameTransform::recycle_image(Image *image)
{
    mImageCachePool.recycle_node(image);
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
    }

    return pSwsContext;
}

void FrameTransform::resize_target(int width, int height)
{
    mCurrentVideoWidth = width;
    mCurrentVideoHeight = height;
}