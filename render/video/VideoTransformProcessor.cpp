#include "VideoTransformProcessor.h"
#include "Image.h"
#include "libyuv.h"
// #include "../../common/log/Log.h"

extern "C"
{
    // #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	// #include "libswscale/swscale.h"
	// #include "libavutil/imgutils.h"
}

VideoTransformProcessor::VideoTransformProcessor()
{
}

VideoTransformProcessor::~VideoTransformProcessor()
{
}

void VideoTransformProcessor::transform_by_libyuv(AVFrame *frame, Image *const image, int width, int height)
{
    image->reformat(AV_PIX_FMT_YUV420P, width, height);
    int format = frame->format;
    if (AV_PIX_FMT_YUV420P ==format)
    {
        // libyuv::ConvertToI420((const uint8_t *)(frame->data),
        //                   frame->linesize[0] * frame->height,
        //                   image->data(), 
        //                   width,
        //                   image->data() + width * height, 
        //                   (width + 1) / 2,
        //                   image->data() + width * height * 5 / 4, 
        //                   (width + 1) / 2,
        //                   0, 
        //                   0, // 以左上角为原点，裁剪起始点
        //                   frame->width, 
        //                   frame->height,
        //                   width, height,
        //                   libyuv::kRotate0,
        //                   libyuv::FOURCC_I420);
        libyuv::I420Scale(frame->data[0],
                    frame->width,
                    frame->data[1], 
                    (frame->width+1)/2,
                    frame->data[2],
                    (frame->width+1)/2,
                    frame->width,
                    frame->height,
                    image->data(), 
                    width,
                    image->data() + width * height,
                    (width + 1) / 2,
                    image->data() + width * height + ((width+1)/2)*((height+1)/2),
                    (width + 1) / 2,
                    width,
                    height,
                    libyuv::kFilterNone);
    } else {
        // Log::get_instance().log_error("[Dsico][TransformPorcessor] frame format in unsupport format = %d", format);
    }
}