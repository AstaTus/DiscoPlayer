#ifndef __RENDER_SYNCHRONIZER_H__
#define __RENDER_SYNCHRONIZER_H__

#include <future>

class AudioFrameTransformer;
class VideoFrameTransformer;
class RenderSynchronizerFinishListener;
class RenderSynchronizer
{
private:
    int mSerial;
    VideoFrameTransformer * mpVideoFrameTransformer;
    AudioFrameTransformer * mpAudioFrameTransformer;
    RenderSynchronizerFinishListener * mpFinishListener;

    std::future<void> mSynchronizerFutrue;
    void wait_for_synchronizer_by_end();

    void wait_for_synchronizer_by_serial();
public:
    RenderSynchronizer(VideoFrameTransformer * video_frame_transformer, 
        AudioFrameTransformer * audio_frame_transformer,
        RenderSynchronizerFinishListener * finish_listener);

    ~RenderSynchronizer();

    void start_by_serial(int serial);

    void start_by_end();

    void wait();
};


#endif