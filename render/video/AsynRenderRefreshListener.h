#ifndef __ASYN_RENDER_REFRESH_LISTENER_H__
#define __ASYN_RENDER_REFRESH_LISTENER_H__
class AsynRenderRefreshListener
{
private:
    /* data */
public:
    AsynRenderRefreshListener(/* args */) = default;
    ~AsynRenderRefreshListener() = default;

    virtual void on_render_finish() = 0;
};

#endif