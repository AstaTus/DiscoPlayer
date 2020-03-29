#pragma once

class VideoTransformNode;
class VideoRenderTransformProcessor
{
private:
    /* data */
public:
    VideoRenderTransformProcessor(/* args */);
    ~VideoRenderTransformProcessor();

    virtual VideoTransformNode * process(VideoTransformNode * node) = 0;

    virtual bool is_need_process(VideoTransformNode * node) = 0;

};