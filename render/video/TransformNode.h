
#ifndef __TRANSFORM_NODE_H__
#define __TRANSFORM_NODE_H__
class Image;
class FrameWrapper;

struct TransformNode
{
    Image *image;
    FrameWrapper *frame_wrapper;
};
#endif
