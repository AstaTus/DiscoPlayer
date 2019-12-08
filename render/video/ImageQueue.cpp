#include "ImageQueue.h"
#include "Image.h"

ImageQueue::ImageQueue(/* args */)
{
}

ImageQueue::~ImageQueue()
{
}

void ImageQueue::destory_node(Image * image)
{
    if (image != NULL)
    {
        delete image;
    }
}
