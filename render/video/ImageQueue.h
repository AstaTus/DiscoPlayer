#ifndef __IMAGE_QUEUE_H__
#define __IMAGE_QUEUE_H__

#include "../../common/structure/ConcurrentQueue.h"

class Image;

class ImageQueue : public ConcurrentQueue<Image>
{
private:
    /* data */

protected:
    void destroy_node(Image * image);
public:
    ImageQueue(/* args */);
    ~ImageQueue();
};
#endif