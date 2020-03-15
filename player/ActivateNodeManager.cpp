#include "ActivateNodeManager.h"
#include "../render/video/VideoFrameTransformer.h"
#include "../render/video/VideoTransformNode.h"
#include "../render/audio/AudioFrameTransformer.h"
#include "../render/audio/AudioTransformNode.h"

ActivateNodeManager::ActivateNodeManager(VideoFrameTransformer *video_frame_transformer,
                                     AudioFrameTransformer *audio_frame_transformer)
    : mpCurrentVideoNode(nullptr),
      mpCurrentAudioNode(nullptr),
      mpVideoFrameTransformer(video_frame_transformer),
      mpAudioFrameTransformer(audio_frame_transformer)
{
}

ActivateNodeManager::~ActivateNodeManager()
{
    recyle_video_node(mpCurrentVideoNode);
    recyle_audio_node(mpCurrentAudioNode);

    mpCurrentVideoNode = nullptr;
    mpCurrentAudioNode = nullptr;

    // mpVideoFrameTransformer = nullptr;
    // mpAudioFrameTransformer = nullptr;
}

VideoTransformNode *ActivateNodeManager::obtain_current_video_node()
{
    recyle_video_node(mpCurrentVideoNode);
    mpCurrentVideoNode = mpVideoFrameTransformer->non_block_pop_transformed_node();
    return mpCurrentVideoNode;
}

VideoTransformNode *ActivateNodeManager::peek_video_queue_node()
{
    return mpVideoFrameTransformer->block_peek_transformed_node();
}

VideoTransformNode *ActivateNodeManager::get_current_video_node()
{
    return mpCurrentVideoNode;
}

AudioTransformNode *ActivateNodeManager::obtain_current_audio_node()
{
    recyle_audio_node(mpCurrentAudioNode);
    mpCurrentAudioNode = mpAudioFrameTransformer->block_pop_transformed_node();
    return mpCurrentAudioNode;
}

AudioTransformNode *ActivateNodeManager::peek_audio_queue_node()
{
    return mpAudioFrameTransformer->non_block_peek_transformed_node();
}

AudioTransformNode *ActivateNodeManager::get_current_audio_node()
{
    return mpCurrentAudioNode;
}

void ActivateNodeManager::recyle_video_node(VideoTransformNode *node)
{
    if (node != nullptr)
    {
        mpVideoFrameTransformer->recycle(node);
    }
}

void ActivateNodeManager::recyle_audio_node(AudioTransformNode *node)
{
    if (node != nullptr)
    {
        mpAudioFrameTransformer->recycle(node);
    }
}

bool ActivateNodeManager::recyle_peek_video_node()
{
    VideoTransformNode *node = mpVideoFrameTransformer->non_block_pop_transformed_node();
    recyle_video_node(node);
    if (node == nullptr)
    {
        return false;
    }
    return true;
    
}

bool ActivateNodeManager::recyle_peek_audio_node()
{
    AudioTransformNode *node = mpAudioFrameTransformer->non_block_pop_transformed_node();
    recyle_audio_node(node);
    if (node == nullptr)
    {
        return false;
    }
    return true;
}

void ActivateNodeManager::recyle_current_audio_node()
{
    recyle_audio_node(mpCurrentAudioNode);
    mpCurrentAudioNode = nullptr;
}

void ActivateNodeManager::recyle_current_video_node()
{
    recyle_video_node(mpCurrentVideoNode);
    mpCurrentVideoNode = nullptr;
}
