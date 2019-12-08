#ifndef __SDL_RENDER_VIEW_H__
#define __SDL_RENDER_VIEW_H__

#include "../../../render/video/IRenderView.h"

class SDLRenderView : public IRenderView
{
private:
    /* data */
public:
    SDLRenderView(/* args */);
    ~SDLRenderView();
};


#endif