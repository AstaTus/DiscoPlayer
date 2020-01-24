#ifndef _Q_RENDER_WIDGET_H__
#define _Q_RENDER_WIDGET_H__

#include <QWidget>

#include "../../render/video/RenderView.h"

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;
struct SDL_Rect;

class QRenderWidget : public QWidget, public RenderView
{
    Q_OBJECT
public:
    QRenderWidget(QWidget *parent = 0);
    ~QRenderWidget();

    virtual void refresh(const Image * const image) override;

    virtual void resize(int width, int height) override;

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
private:
    void recreate_texture();
private:
    SDL_Renderer*        m_pRender;
    SDL_Texture*         m_pTexture;
    SDL_Window*          m_pWindow;
    SDL_Rect*            m_pSDLRect;
};

#endif // _SDL_RENDER_WND_H__