#ifndef __PLAYER_STATE_CHANGED_EVENT_H__
#define __PLAYER_STATE_CHANGED_EVENT_H__
#include <qevent.h>
#include "../../player/state/PlayerStateEnum.h"
class PlayrerStateChangedEvent : public QEvent
{
private:
    
    PlayerStateEnum mState;
public:
    static const QEvent::Type sType;
    
    PlayrerStateChangedEvent(PlayerStateEnum state);
    ~PlayrerStateChangedEvent();

    PlayerStateEnum get_state();
};
#endif