#include "TransformPorcessor.h"
#include "Image.h"
#include "libyuv.h"

TransformPorcessor::TransformPorcessor()
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

TransformPorcessor::~TransformPorcessor()
{
}

void TransformPorcessor::transform_by_libyuv(AVFrame *frame, Image *const image)
{
    image->reformat(AV_PIX_FMT_YUV420P, mCurrentVideoWidth, mCurrentVideoHeight);

    libyuv::ConvertToI420((const uint8_t *)(frame->data),
                          frame->linesize[0] * frame->height,
                          image->data(), mCurrentVideoWidth,
                          image->data() + mCurrentVideoWidth * mCurrentVideoHeight, mCurrentVideoWidth / 2,
                          image->data() + mCurrentVideoWidth * mCurrentVideoHeight * 5 / 4, mCurrentVideoWidth / 2,
                          0, 0, // 以左上角为原点，裁剪起始点
                          frame->width, frame->height,
                          mCurrentVideoWidth, mCurrentVideoHeight,
                          libyuv::kRotate0,
                          libyuv::FOURCC_I420);
}

void TransformPorcessor::transform_by_ffmpeg(AVFrame *frame, Image *const image)
{
    AVPixelFormat format = static_cast<AVPixelFormat>(frame->format);
    SwsContext *sws_context = get_sws_context(mCurrentVideoWidth, mCurrentVideoHeight,
                                              frame->width, frame->height, format);

    image->reformat(AV_PIX_FMT_YUV420P, mCurrentVideoWidth, mCurrentVideoHeight);

    sws_scale(sws_context,
              (const uint8_t* const*)frame->data, frame->linesize, 0,
              frame->height, (uint8_t* const*)image->data(), image->frame()->linesize);
}

SwsContext *TransformPorcessor::get_sws_context(int dst_width,
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

void TransformPorcessor::resize_target(int width, int height)
{
    mCurrentVideoWidth = width;
    mCurrentVideoHeight = height;
}