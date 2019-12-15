#ifndef __IRENDER_VIEW_H__
#define __IRENDER_VIEW_H__
#include <iostream>

class RenderView
{
protected:
    int mWidth;
    int mHeight;
public:
    RenderView(int width, int height);

    virtual void refresh(const uint8_t * const data, int pitch) = 0;

    void resize(int width, int height);
};

#endif