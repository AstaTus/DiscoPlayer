
#ifndef __OPENGL_RENDER_WIDGET_H__
#define __OPENGL_RENDER_WIDGET_H__
 
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "../../render/video/RenderView.h"
#include <condition_variable>
class Image;
class OpenGLRenderWidget : public QOpenGLWidget, public RenderView, protected QOpenGLFunctions
{
    Q_OBJECT
 
public:
    OpenGLRenderWidget(QWidget *parent = 0);
    ~OpenGLRenderWidget();
 
    GLuint      m_uiFBO;
    GLuint      m_uiDepthBuff;
    GLuint      m_uiTex;

    virtual void refresh(const Image * const image) override;

    virtual void resize(int width, int height) override;

    virtual void invalid_image() override;
protected:
    void        initializeGL() override;
    void        resizeGL(int, int) override;
    void        paintGL() override;

private:
    QOpenGLShaderProgram mShaderProgram;
    //shader中yuv变量地址
    GLuint m_textureUniformY, m_textureUniformU , m_textureUniformV;
    //创建纹理
    GLuint m_idy , m_idu , m_idv;
    const Image * mpImage;
    std::mutex mImageMutex;

};

#endif