#ifndef __PLAYER_STATE_CHANGED_EVENT_H__
#define __PLAYER_STATE_CHANGED_EVENT_H__
#include <qevent.h>
#include "../../player/state/PlayerStateEnum.h"
class CorePlayer;
class PlayrerStateChangedEvent : public QEvent
{
private:
    CorePlayer * mpCorePlayer;
    PlayerStateEnum mState;
public:
    static const QEvent::Type sType;
    
    PlayrerStateChangedEvent(PlayerStateEnum state, CorePlayer * player);
    ~PlayrerStateChangedEvent();

    PlayerStateEnum get_state();
    CorePlayer * get_core_player();
};
#endif