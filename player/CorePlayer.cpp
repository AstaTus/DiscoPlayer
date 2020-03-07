#include "CorePlayer.h"

#include "../render/video/VideoFrameTransformer.h"
#include "../render/video/VideoTransformNode.h"
#include "../codec/FrameWrapper.h"
#include "../common/log/Log.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// #include "libswscale/swscale.h"
#include "libavutil/time.h"
}

#include "./state/PlayingState.h"
#include "./state/SeekingState.h"
#include "./state/InitState.h"
#include "./state/PreparedState.h"
#include "./state/PauseState.h"
#include "./state/StopState.h"
#include "./state/ReleaseState.h"
#include "./state/EndState.h"
#include "./state/StateChangedListener.h"

#include "./CorePlayerStateChangedListener.h"

#include "DebugInfo.h"

class AudioDevice;
CorePlayer::CorePlayer()
    : pMediaDecoder(nullptr),
      pVideoRender(nullptr),
      pRenderView(nullptr),
      pInputStream(nullptr),
      pCurrentPlayItem(nullptr),
      mpSyncClockManager(nullptr),
      mStateManager(),
      mpDebugInfo(nullptr),
      mpCorePlayerStateChangedListener(nullptr)
{
    mStateManager.set_state_changed_listener(this);
}

CorePlayer::~CorePlayer()
{
    mInitFuture.wait();

    pRenderView = nullptr;
    pAudioDevice = nullptr;
    pVideoRender = nullptr;
    pAudioRender = nullptr;
}

void CorePlayer::set_play_item(PlayItem *play_item)
{
    if (pCurrentPlayItem == nullptr && play_item->get_data_source().size() > 0)
    {
        pCurrentPlayItem = play_item;
    }
    else
    {
    }
}

void CorePlayer::replace_play_item(const PlayItem *const play_item)
{

}

void CorePlayer::set_render_surface(RenderView *render_view)
{
    pRenderView = render_view;
    pVideoRender = new VideoRender(pRenderView);
}

void CorePlayer::set_audio_device(AudioDevice *audio_device, AudioDeviceLastBufferProcessor * audio_last_buffer_processor)
{
    pAudioDevice = audio_device;
    pAudioRender = new AudioRender(pAudioDevice);
    pAudioRender->set_last_buffer_processor(audio_last_buffer_processor);
}

void CorePlayer::init_task()
{
    mpSyncClockManager = new SyncClockManager(SyncClockManager::SYNC_STRATEGY_AUDIO),
    pInputStream = new MediaInputStream();
    pMediaDecoder = new MediaDecoder(pInputStream->get_stream_iterator(),
                                     pInputStream->get_packet_reader());
    mpVideoFrameTransformer = new VideoFrameTransformer(pMediaDecoder->get_video_frame_reader());
    mpAudioFrameTransformer = new AudioFrameTransformer(pMediaDecoder->get_audio_frame_reader());
    mpActivateNodeManager = new ActivateNodeManager(mpVideoFrameTransformer, mpAudioFrameTransformer);
    pRenderView->set_video_frame_transformer(mpVideoFrameTransformer);
    
    
    init_states();
    //传递初始化
    mStateManager.on_prepare(false, 0, pCurrentPlayItem->get_data_source());

}

void CorePlayer::start()
{
    if (pVideoRender == NULL || pCurrentPlayItem == NULL)
    {
        //TODO throw exception
    }
    else
    {
        mInitFuture = std::async(std::launch::async, &CorePlayer::init_task, this);
    }
}

void CorePlayer::pause()
{
    mStateManager.on_pause_by_user();
}

void CorePlayer::resume()
{
    mStateManager.on_resume_by_user();
}

void CorePlayer::stop()
{
    if (pAudioDevice != nullptr)
    {
        pAudioDevice->stop();
    }
    
    mStateManager.on_stop_by_user();
}

void CorePlayer::init_states()
{
    PlayingState * playing_state = new PlayingState(pVideoRender, pAudioRender, pAudioDevice,
        mpActivateNodeManager, mpSyncClockManager, pInputStream, &mStateManager);
    mStateManager.add_state(PlayerStateEnum::PLAYING, playing_state);
    SeekingState * seeking_state = new SeekingState(&mStateManager, pAudioDevice, pInputStream, 
        pMediaDecoder, mpVideoFrameTransformer, mpAudioFrameTransformer, 
        mpActivateNodeManager, mpSyncClockManager);
    mStateManager.add_state(PlayerStateEnum::SEEKING, seeking_state);
    InitState * init_state = new InitState(pInputStream, pMediaDecoder, mpVideoFrameTransformer, 
        mpAudioFrameTransformer, pAudioDevice, &mStateManager);
    mStateManager.add_state(PlayerStateEnum::PREPARING, init_state);
    PreparedState * prepared_state = new PreparedState(pAudioRender, &mStateManager, mpAudioFrameTransformer);
    mStateManager.add_state(PlayerStateEnum::PREPARED, prepared_state);
    PauseState * pause_state = new PauseState(pAudioRender);
    mStateManager.add_state(PlayerStateEnum::PAUSED, pause_state);
    StopState * stop_state = new StopState(pInputStream, pMediaDecoder, mpVideoFrameTransformer, 
        mpAudioFrameTransformer, pAudioRender, &mStateManager);
    mStateManager.add_state(PlayerStateEnum::STOPPED, stop_state);
    ReleaseState * release_state = new ReleaseState(pInputStream, pMediaDecoder, mpVideoFrameTransformer, 
        mpAudioFrameTransformer, mpActivateNodeManager, pAudioRender, &mStateManager, mpSyncClockManager);
    mStateManager.add_state(PlayerStateEnum::RELEASING, release_state);
    EndState * end_state = new EndState();
    mStateManager.add_state(PlayerStateEnum::END, end_state);
}

void CorePlayer::set_volume(int volume)
{
    if (pAudioRender != nullptr)
    {
        pAudioRender->set_volume(volume);
    }
}

int CorePlayer::get_volume()
{
    if (pAudioRender != nullptr)
    {
        return pAudioRender->get_volume();
    }
    return 0;
}

int CorePlayer::get_max_volume()
{
    return AudioRender::MAX_AUDIO_VOLUME;
}

PlayerStateEnum CorePlayer::get_current_play_state()
{
    return mStateManager.get_play_state();
}

void CorePlayer::set_speed(float speed)
{
    if (pAudioRender != nullptr)
    {
        pAudioRender->set_speed(speed);
    }

    if (mpSyncClockManager != nullptr)
    {
        mpSyncClockManager->set_speed(speed);
    }
}

float CorePlayer::get_speed()
{
    if (mpSyncClockManager != nullptr)
    {
        return mpSyncClockManager->get_speed();
    }

    return 1.0f;
}

int64_t CorePlayer::get_duration()
{
    if (mCurrentState == PlayerStateEnum::PLAYING || mCurrentState == PlayerStateEnum::PAUSED || mCurrentState == PlayerStateEnum::PREPARED)
    {
        if (pInputStream != nullptr)
        {
            return pInputStream->get_duration();
        }
    }
    return 0L;
}

int64_t CorePlayer::get_current_position()
{
    if (mCurrentState == PlayerStateEnum::PLAYING || mCurrentState == PlayerStateEnum::PAUSED)
    {
        return mpSyncClockManager->get_current_position();
    } else {
        return 0L;
    }
} 

void CorePlayer::seek(int64_t position)
{
    mStateManager.on_seek_start(position);
}

void CorePlayer::set_player_state_change_listener(CorePlayerStateChangedListener * listener)
{
    mpCorePlayerStateChangedListener = listener;
}

void CorePlayer::on_state_changed(PlayerStateEnum state)
{
    mCurrentState = state;
    if (mpCorePlayerStateChangedListener != nullptr)
    {
        mpCorePlayerStateChangedListener->on_state_changed(state, this);
    }
}

const DebugInfo * CorePlayer::get_debug_info() {
    if (mpDebugInfo == nullptr)
    {
        mpDebugInfo = new DebugInfo();
    }

    if (mCurrentState == PlayerStateEnum::PLAYING)
    {
        mpDebugInfo->video_pts = mpSyncClockManager->get_video_pts();
        mpDebugInfo->audio_pts = mpSyncClockManager->get_video_pts();
        mpDebugInfo->video_time = mpSyncClockManager->get_video_position();
        mpDebugInfo->audio_time = mpSyncClockManager->get_audio_position();
    }
    
    return mpDebugInfo;
}