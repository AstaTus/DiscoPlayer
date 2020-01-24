#include "QRenderWidget.h"
extern "C" {
#include "SDL.h"
}
#include "../../render/video/Image.h"

QRenderWidget::QRenderWidget(QWidget *parent)
    : QWidget(parent),
      RenderView(0, 0),
      m_pTexture(nullptr),
      m_pRender(nullptr),
      m_pWindow(nullptr),
      m_pSDLRect(nullptr)
{
    m_pWindow = SDL_CreateWindowFrom((void*)winId());
    m_pRender = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    m_pSDLRect = new SDL_Rect();
}

QRenderWidget::~QRenderWidget()
{
    if (m_pWindow)
        SDL_DestroyWindow(m_pWindow);
    if (m_pRender)
        SDL_DestroyRenderer(m_pRender);
    if (m_pTexture)
        SDL_DestroyTexture(m_pTexture);
    if (m_pSDLRect)
    {
        delete m_pSDLRect;
    }
}

void QRenderWidget::refresh(const Image * const image)
{
    SDL_UpdateTexture(m_pTexture, NULL, image->data(), image->pitch());
    SDL_RenderClear(m_pRender);
    SDL_RenderCopy(m_pRender, m_pTexture, NULL, m_pSDLRect);
    SDL_RenderPresent(m_pRender);
}

void QRenderWidget::resize(int width, int height)
{
    RenderView::resize(width, height);
    recreate_texture();
}

void QRenderWidget::recreate_texture()
{
    if (m_pTexture != nullptr)
    {
        SDL_DestroyTexture(m_pTexture);
        m_pTexture = nullptr;
    }

    m_pSDLRect->x = 0;
    m_pSDLRect->y = 0;
    m_pSDLRect->w = mWidth;
    m_pSDLRect->h = mHeight;

    m_pTexture = SDL_CreateTexture(m_pRender, SDL_PIXELFORMAT_IYUV,
                                    SDL_TEXTUREACCESS_STREAMING, mWidth, mHeight);
}

void QRenderWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    resize(frameGeometry().size().width(), frameGeometry().size().height());
}