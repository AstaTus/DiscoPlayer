#ifndef __IRENDER_VIEW_H__
#define __IRENDER_VIEW_H__
#include <iostream>

class IRenderView
{
public:
    virtual void init() = 0;

    virtual void refresh(const uint8_t * const data, int pitch) = 0;
};

#endif