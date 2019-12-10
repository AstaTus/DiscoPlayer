
#ifndef __TRANSFORM_NODE_H__
#define __TRANSFORM_NODE_H__
class Image;
class AVFrame;

struct TransformNode
{
    Image *image;
    AVFrame *frame;
};
#endif
