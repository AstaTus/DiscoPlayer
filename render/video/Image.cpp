#include "Image.h"

Image::Image()
:mPixFormat(AV_PIX_FMT_NONE),
mWidth(0),
mHeight(0),
pBuffer(NULL),
pFrameYUV(NULL)
{
    pFrameYUV = av_frame_alloc();
}

Image::~Image()
{
    release_buffer();
}
uint8_t * const Image::data() const
{
    return pBuffer;
}

void Image::release_buffer()
{
    if (pBuffer != NULL)
    {
        av_free(pBuffer);
    }

    if(pFrameYUV != NULL)
    {
        av_frame_free(&pFrameYUV);
    }
    
    pBuffer = NULL;
    pFrameYUV = NULL;
}

void Image::reformat(AVPixelFormat pix_fmt, int width, int height)
{
    if (width != mWidth || height != mHeight || mPixFormat != pix_fmt)
    {
        if (pBuffer != NULL)
        {
            release_buffer();
        }

        //TODO algin 默认为1
        pBuffer = (uint8_t *)av_malloc(av_image_get_buffer_size(pix_fmt, 
		    mWidth, mHeight, 1));

        //将FrameYUV的data指向out_buffer
	    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, pBuffer, mPixFormat, 
		    mWidth, mHeight, 1);
    }
}

AVFrame * const Image::frame() const
{
    return pFrameYUV;
}

int Image::pitch() const
{
    return mWidth;
}

