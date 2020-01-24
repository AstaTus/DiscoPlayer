#include "SDLRenderView.h"
#include "../../render/video/Image.h"
SDLRenderView::SDLRenderView(SDL_Renderer *sdl_render, int width, int height)
    : RenderView(width, height),
      pSDLTexture(nullptr),
      pSDLRenderer(sdl_render),
      mSDLRect()
{
    recreate_texture();
}

SDLRenderView::~SDLRenderView()
{
    if (pSDLTexture != NULL)
    {
        SDL_DestroyTexture(pSDLTexture);
        pSDLTexture = NULL;
    }
}

void SDLRenderView::refresh(const Image * const image)
{
    SDL_UpdateTexture(pSDLTexture, NULL, image->data(), image->pitch());
    SDL_RenderClear(pSDLRenderer);
    SDL_RenderCopy(pSDLRenderer, pSDLTexture, NULL, &mSDLRect);
    SDL_RenderPresent(pSDLRenderer);
}

void SDLRenderView::resize(int width, int height)
{
    RenderView::resize(width, height);

    recreate_texture();
}

void SDLRenderView::recreate_texture()
{
    if (pSDLTexture != NULL)
    {
        SDL_DestroyTexture(pSDLTexture);
        pSDLTexture = NULL;
    }

    mSDLRect.x = 0;
    mSDLRect.y = 0;
    mSDLRect.w = mWidth;
    mSDLRect.h = mHeight;

    pSDLTexture = SDL_CreateTexture(pSDLRenderer, SDL_PIXELFORMAT_IYUV,
                                    SDL_TEXTUREACCESS_STREAMING, mWidth, mHeight);
}