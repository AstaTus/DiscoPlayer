#ifndef __RENDER_SYNCHRONIZER_FINISH_LISTENER_H__
#define __RENDER_SYNCHRONIZER_FINISH_LISTENER_H__
class RenderSynchronizerFinishListener
{
private:
    /* data */
public:
    RenderSynchronizerFinishListener(/* args */) = default;
    virtual ~RenderSynchronizerFinishListener() = default;

    virtual void on_synchronizer_finish() = 0;
};

#endif