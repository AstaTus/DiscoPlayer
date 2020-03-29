#ifndef __IMAGE_H__
#define __IMAGE_H__

// #include <stdint.h>

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

    int mSrcWidth;
    int mSrcHeight;

    uint8_t * mpSrcBuffer[AV_NUM_DATA_POINTERS];
    uint16_t mpPitches[AV_NUM_DATA_POINTERS];

    void release_buffer();
public:
    Image();
    ~Image();

    void reformat_RGBA(int width, int height);

    void reformat(AVPixelFormat pix_fmt, int width, int height);

    void fill_data(AVFrame * frame);

    uint8_t * const data() const;

    int pitch() const;

    int height() const;

    int width() const;

    uint8_t * get_src_data(int index) const;

    int get_src_width() const;

    int get_src_height() const;
};
#endif