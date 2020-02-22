#pragma once 
#include "./state/PlayerStateEnum.h"
class CorePlayer;
class CorePlayerStateChangedListener
{
private:
    /* data */
public:
    CorePlayerStateChangedListener(/* args */) = default;
    virtual ~CorePlayerStateChangedListener() = default;

    virtual void on_state_changed(PlayerStateEnum state, CorePlayer * player) = 0;
};