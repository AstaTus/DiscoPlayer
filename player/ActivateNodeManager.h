#ifndef __ACTIVATE_MANAGER_H__
#define __ACTIVATE_MANAGER_H__

class VideoTransformNode;
class AudioTransformNode;
class MediaDecoder;
class VideoFrameTransformer;
class AudioFrameTransformer;

class ActivateNodeManager
{
private:
    VideoTransformNode *mpCurrentVideoNode;
    AudioTransformNode *mpCurrentAudioNode;

    VideoFrameTransformer *const mpVideoFrameTransformer;
    AudioFrameTransformer *const mpAudioFrameTransformer;

    /*回收 node*/
    void recyle_video_node(VideoTransformNode *node);
    void recyle_audio_node(AudioTransformNode *node);

public:
    ActivateNodeManager(VideoFrameTransformer *video_frame_transformer,
                    AudioFrameTransformer *audio_frame_transformer);
    ~ActivateNodeManager();

    /*将当前queue的peek node变为current node*/
    VideoTransformNode *obtain_current_video_node();
    /*获得queue的peek node*/
    VideoTransformNode *peek_video_queue_node();
    /*获得当前展示的node*/
    VideoTransformNode *get_current_video_node();

    /*将当前queue的peek node变为current node*/
    AudioTransformNode *obtain_current_audio_node();
    /*获得queue的peek node*/
    AudioTransformNode *peek_audio_queue_node();
    /*获得当前展示的node*/
    AudioTransformNode *get_current_audio_node();

    /**
     * @return true 回收成功， false peek 为空 没有回收
    */
    bool recyle_peek_video_node();
    /**
     * @return true 回收成功， false peek 为空 没有回收
    */
    bool recyle_peek_audio_node();
    void recyle_current_audio_node();
    void recyle_current_video_node();
};
#endif