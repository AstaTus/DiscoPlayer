#ifndef __IMAGE_H__
#define __IMAGE_H__

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

class Image
{
private:
    uint8_t * pBuffer;
    AVPixelFormat mPixelFormat;
    int mWidth;
    int mHeight;

    void release_buffer();
public:
    Image();
    ~Image();

    void reformat(AVPixelFormat pix_fmt, int width, int height);

    uint8_t * const data() const;

    int pitch() const;
};
#endif