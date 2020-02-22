#include "PlayrerStateChangedEvent.h"
#include "../../player/CorePlayer.h"

const QEvent::Type PlayrerStateChangedEvent::sType = static_cast<QEvent::Type>(60000);

PlayrerStateChangedEvent::PlayrerStateChangedEvent(PlayerStateEnum state, CorePlayer * player)
    : QEvent((QEvent::Type)sType), 
    mState(state),
    mpCorePlayer(player)
{

}

PlayrerStateChangedEvent::~PlayrerStateChangedEvent()
{

}

PlayerStateEnum PlayrerStateChangedEvent::get_state()
{
    return mState;
}

CorePlayer * PlayrerStateChangedEvent::get_core_player()
{
    return mpCorePlayer;
}