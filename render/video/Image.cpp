#include "Image.h"

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

Image::Image()
    : mPixelFormat(AV_PIX_FMT_NONE),
      mWidth(0),
      mHeight(0),
      pBuffer(nullptr)
{
}

Image::~Image()
{
    release_buffer();
}
uint8_t *const Image::data() const
{
    return pBuffer;
}

void Image::release_buffer()
{
    if (pBuffer != nullptr)
    {
        delete pBuffer;
    }

    pBuffer = nullptr;
}

void Image::reformat_RGBA(int width, int height)
{
    if (mWidth != width || mHeight != height || pBuffer == nullptr)
    {
        mWidth = width;
        mHeight = height;
        pBuffer = new uint8_t[mWidth * mHeight * 4];
    }
}

void Image::reformat(AVPixelFormat pix_fmt, int width, int height)
{
    // if (width != mWidth || height != mHeight || mPixelFormat != pix_fmt)
    // {
    //     if (pBuffer != NULL)
    //     {
    //         release_buffer();
    //     }

    //     mWidth = width;
    //     mHeight = height;
    //     mPixelFormat = pix_fmt;
    //     //TODO algin 默认为1
    //     int size = av_image_get_buffer_size(pix_fmt, mWidth, mHeight, 1);
    //     pBuffer = new uint8_t[size];
    // }
}

void Image::fill_data(AVFrame * frame) {
    for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) 
    {
        mpSrcBuffer[i] = frame->data[i];
        mpPitches[i] = frame->linesize[i];
    }

    mSrcWidth = frame->width;
    mSrcHeight = frame->height;
}

int Image::get_src_width() const
{
    return mSrcWidth;
}

int Image::get_src_height() const
{
    return mSrcHeight;
}

uint8_t * Image::get_src_data(int index) const
{
    return mpSrcBuffer[index];
}

int Image::pitch() const
{
    return mWidth;
}


int Image::height() const
{
    return mHeight;
}

int Image::width() const
{
    return mWidth;
}