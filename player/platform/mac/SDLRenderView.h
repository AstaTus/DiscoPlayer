#ifndef __SDL_RENDER_VIEW_H__
#define __SDL_RENDER_VIEW_H__

#include "SDL2/SDL.h"

#include "../../../render/video/RenderView.h"

class SDLRenderView : public RenderView
{
private:
    
    SDL_Texture *pSDLTexture;
    SDL_Renderer *pSDLRenderer;
    SDL_Rect mSDLRect;

    void recreate_texture();

public:
    SDLRenderView(SDL_Renderer * sdl_render, int width, int height);
    ~SDLRenderView();

    void refresh(const uint8_t * const data, int pitch);

    void resize(int width, int height);
};


#endif