
#include "ReleaseState.h"

#include "../../stream/MediaInputStream.h"
#include "../../codec/MediaDecoder.h"
#include "../../render/video/VideoFrameTransformer.h"
#include "../../render/audio/AudioFrameTransformer.h"
#include "StateManager.h"
#include "../../render/audio/AudioDevice.h"
#include "../ActivateNodeManager.h"

ReleaseState::ReleaseState(MediaInputStream *input_stream,
                           MediaDecoder *media_decoder,
                           VideoFrameTransformer *video_frame_transformer,
                           AudioFrameTransformer *audio_frame_transformer,
                           ActivateNodeManager *activate_node_manager,
                           AudioDevice *audio_device,
                           StateManager *state_manager,
                           SyncClockManager *sync_clock_manager)
    : mpMediaInputStream(input_stream),
      mpMediaDecoder(media_decoder),
      mpVideoFrameTransformer(video_frame_transformer),
      mpAudioFrameTransformer(audio_frame_transformer),
      mpActivateNodeManager(activate_node_manager),
      mpAudioDevice(audio_device),
      mpStateManager(state_manager),
      mpSyncClockManager(sync_clock_manager)
{
}

ReleaseState::~ReleaseState()
{
}

void ReleaseState::on_state_enter(...)
{
  mpReleaseThread = new std::thread(&ReleaseState::inner_release_task, this);

  
}

void ReleaseState::inner_release_task()
{
  mpMediaInputStream->close();
  mpMediaDecoder->stop();
  mpVideoFrameTransformer->stop();
  mpAudioFrameTransformer->stop();
  
  delete mpMediaInputStream;
  delete mpActivateNodeManager;
  delete mpVideoFrameTransformer;
  delete mpAudioFrameTransformer;
  delete mpMediaDecoder;
  delete mpSyncClockManager;

  // mpMediaInputStream = nullptr;
  // mpMediaDecoder = nullptr;
  // mpVideoFrameTransformer = nullptr;
  // mpAudioFrameTransformer = nullptr;
  // mpAudioDevice = nullptr;
  // mpSyncClockManager = nullptr;

  mpStateManager->on_end();
}

void ReleaseState::on_state_exit()
{
}