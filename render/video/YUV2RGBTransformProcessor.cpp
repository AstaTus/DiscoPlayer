#include "YUV2RGBTransformProcessor.h"
#include "libyuv.h"
#include "VideoTransformNode.h"
#include "../../codec/FrameWrapper.h"
#include "Image.h"

extern "C"
{
	#include "libavutil/frame.h"
}

YUV2RGBTransformProcessor::YUV2RGBTransformProcessor(/* args */)
{
}

YUV2RGBTransformProcessor::~YUV2RGBTransformProcessor()
{
}

VideoTransformNode * YUV2RGBTransformProcessor::process(VideoTransformNode * node)
{
    //libyuv::I420ToARGB(const uint8* src_y, int src_stride_y,
        //  const uint8* src_u, int src_stride_u,
        //  const uint8* src_v, int src_stride_v,
        //  uint8* dst_argb, int dst_stride_argb,
        //  int width, int height);

    node->image->reformat_RGBA(node->frame_wrapper->frame->width, node->frame_wrapper->frame->height);
    //TODO 此处可能有大小端问题 导致shader 渲染时是4个分量有问题 所以此处取转换成反的A\B\G\R
    libyuv::I420ToABGR(node->frame_wrapper->frame->data[0], node->frame_wrapper->frame->width,
        node->frame_wrapper->frame->data[1], node->frame_wrapper->frame->width / 2,
        node->frame_wrapper->frame->data[2], node->frame_wrapper->frame->width / 2,
        (uint8_t*)node->image->data(), node->frame_wrapper->frame->width * 4,
        node->frame_wrapper->frame->width, node->frame_wrapper->frame->height);
        return node;
}

bool YUV2RGBTransformProcessor::is_need_process(VideoTransformNode * node)
{
    return true;
}
