#include "SyncStrategy.h"

SyncStrategy::SyncStrategy()
:mVideoClock(),
mAudioClock(),
mNaturalClock(),
mSystemTime(0.0),
mSpeed(1.0)
{

}
SyncStrategy::~SyncStrategy()
{

}

