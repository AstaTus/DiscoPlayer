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
#include "./state/StateChangedListener.h"
#include "DebugInfo.h"

class AudioDevice;
CorePlayer::CorePlayer()
    : pMediaDecoder(nullptr),
      pVideoRender(nullptr),
      pRenderView(nullptr),
      pInputStream(nullptr),
      pCurrentPlayItem(nullptr),
      mSyncClockManager(SyncClockManager::SYNC_STRATEGY_AUDIO),
      mStateManager(),
      mpDebugInfo(nullptr)
{
}

CorePlayer::~CorePlayer()
{
    mVideoRenderFuture.wait();
    mpVideoFrameTransformer->stop();
    mpAudioFrameTransformer->stop();

    delete mpVideoFrameTransformer;
    delete mpAudioFrameTransformer;
    delete mpActivateNodeManager;

    pRenderView = nullptr;
    pAudioDevice = nullptr;
    delete pVideoRender;
    delete pAudioRender;
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

void CorePlayer::set_audio_device(AudioDevice *audio_device)
{
    pAudioDevice = audio_device;
    pAudioRender = new AudioRender(pAudioDevice);
}

void CorePlayer::init_task()
{
    pInputStream = new MediaInputStream();
    pMediaDecoder = new MediaDecoder(pInputStream->get_stream_iterator(),
                                     pInputStream->get_packet_reader());
    mpVideoFrameTransformer = new VideoFrameTransformer(pMediaDecoder->get_video_frame_reader());
    mpAudioFrameTransformer = new AudioFrameTransformer(pMediaDecoder->get_audio_frame_reader());
    mpActivateNodeManager = new ActivateNodeManager(mpVideoFrameTransformer, mpAudioFrameTransformer);
    mpVideoFrameTransformer->on_resize_render_view(
        pRenderView->get_width(), pRenderView->get_height());
    
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
    mStateManager.onPauseByUser();
}

void CorePlayer::resume()
{
    mStateManager.onResumeByUser();
}

void CorePlayer::stop()
{
    mStateManager.onPauseByUser();
}

void CorePlayer::init_states()
{
    PlayingState * playing_state = new PlayingState(pVideoRender, pAudioDevice,
        mpActivateNodeManager, &mSyncClockManager, pInputStream, &mStateManager);
    mStateManager.add_state(PlayerStateEnum::PLAYING, playing_state);
    SeekingState * seeking_state = new SeekingState(&mStateManager, pAudioDevice, pInputStream, 
        pMediaDecoder, mpVideoFrameTransformer, mpAudioFrameTransformer, 
        mpActivateNodeManager, &mSyncClockManager);
    mStateManager.add_state(PlayerStateEnum::SEEKING, seeking_state);
    InitState * init_state = new InitState(pInputStream, pMediaDecoder, mpVideoFrameTransformer, 
        mpAudioFrameTransformer, pAudioDevice, &mStateManager);
    mStateManager.add_state(PlayerStateEnum::PREPARING, init_state);
}

PlayerStateEnum CorePlayer::get_current_play_state()
{
    return mStateManager.get_play_state();
}

int64_t CorePlayer::get_duration()
{
    if (pInputStream != nullptr)
    {
        return pInputStream->get_duration();
    }

    return 0;
}

int64_t CorePlayer::get_current_position()
{
    return mSyncClockManager.get_current_position();
} 

void CorePlayer::seek(int64_t position)
{
    mStateManager.onSeekStart(position);
}

void CorePlayer::set_player_state_change_listener(StateChangedListener * listener)
{
    mStateManager.set_state_changed_listener(listener);
}

const DebugInfo * CorePlayer::get_debug_info() {
    if (mpDebugInfo == nullptr)
    {
        mpDebugInfo = new DebugInfo();
    }

    mpDebugInfo->video_pts = mSyncClockManager.get_video_pts();
    mpDebugInfo->audio_pts = mSyncClockManager.get_video_pts();
    mpDebugInfo->video_time = mSyncClockManager.get_video_position();
    mpDebugInfo->audio_time = mSyncClockManager.get_audio_position();
    return mpDebugInfo;
}