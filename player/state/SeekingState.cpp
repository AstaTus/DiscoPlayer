#include "SeekingState.h"
#include "../../render/audio/AudioDevice.h"
#include "../../codec/MediaDecoder.h"
#include "../../stream/MediaInputStream.h"
#include "../../render/video/VideoFrameTransformer.h"
#include "../../render/audio/AudioFrameTransformer.h"
#include "../ActivateNodeManager.h"
#include "../../common/log/Log.h"

SeekingState::SeekingState(AudioDevice * audio_device,
                           MediaInputStream * input_stream,
                           MediaDecoder * media_decoder,
                           VideoFrameTransformer * video_frame_transformer,
                           AudioFrameTransformer * audio_frame_transformer,
                           ActivateNodeManager * activate_node_manager)
    : BaseState(),
      mpAudioDevice(audio_device),
      mpInputStream(input_stream),
      mpMediaDecoder(media_decoder),
      mpVideoFrameTransformer(video_frame_transformer),
      mpAudioFrameTransformer(audio_frame_transformer),
      mpActivateNodeManager(activate_node_manager)
{
}

SeekingState::~SeekingState()
{
}

void SeekingState::on_state_enter()
{
    //暂停音频播放
    //TODO 清除音频设备缓存
    mpAudioDevice->pause();
    //暂停视频播放，停留在最后一帧，在CorePlayer中暂停

    //TODO 暂停音视频Transform

    //暂停音视频解码
    mpMediaDecoder->pause();
    //清除音视频Transform缓存
    bool ret = true;
    while(ret) {
        ret = mpActivateNodeManager->recyle_peek_audio_node();
    }
    while(ret) {
        ret = mpActivateNodeManager->recyle_peek_video_node();
    }
    //清除当前的播放的音视频node
    mpActivateNodeManager->recyle_current_audio_node();
    mpActivateNodeManager->recyle_current_video_node();

    //seek
    //TODO POS
    mpInputStream->seek(0);

    //恢复音视频解码
    mpMediaDecoder->resume();
    //恢复transform

    
    Log::get_instance().log_debug("[Disco][CorePlayer::seek start]");
}

void SeekingState::on_state_exit()
{
    
    //恢复音视频Transform 在CorePlayer中暂停

    //恢复音视频播放-起播第一帧是否需要同步？
    mpAudioDevice->resume();

    Log::get_instance().log_debug("[Disco][CorePlayer::seek end]");
}