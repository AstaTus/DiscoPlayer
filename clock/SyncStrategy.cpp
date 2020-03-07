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

float SyncStrategy::get_speed()
{
    return mSpeed;
}

void SyncStrategy::set_speed(float speed)
{
    mSpeed = speed;
}
