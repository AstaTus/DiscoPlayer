
#ifndef __TRANSFORM_NODE_H__
#define __TRANSFORM_NODE_H__
class Image;
class FrameWrapper;

struct VideoTransformNode
{
    Image *image;
    FrameWrapper *frame_wrapper;
};
#endif
