#ifndef __SDL_RENDER_VIEW_H__
#define __SDL_RENDER_VIEW_H__

#include "SDL.h"

#include "../../render/video/RenderView.h"

class SDLRenderView : public RenderView
{
private:
    SDL_Texture *pSDLTexture;
    SDL_Renderer *pSDLRenderer;
    SDL_Rect mSDLRect;

    void recreate_texture();

public:
    SDLRenderView(SDL_Renderer * sdl_render, int width, int height);
    virtual ~SDLRenderView();

    void refresh(const Image * const image) override;

    void resize(int width, int height) override;
};


#endif