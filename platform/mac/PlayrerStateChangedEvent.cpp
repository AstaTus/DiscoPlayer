#include "PlayrerStateChangedEvent.h"


const QEvent::Type PlayrerStateChangedEvent::sType = static_cast<QEvent::Type>(60000);

PlayrerStateChangedEvent::PlayrerStateChangedEvent(PlayerStateEnum state)
    : QEvent( (QEvent::Type)sType), 
    mState(state) 
{

}

PlayrerStateChangedEvent::~PlayrerStateChangedEvent()
{

}

PlayerStateEnum PlayrerStateChangedEvent::get_state()
{
    return mState;
}