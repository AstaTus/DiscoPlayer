#pragma once
#include "BaseState.h"

class EndState : public BaseState
{
private:
    /* data */
public:
    EndState(/* args */);
    ~EndState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};

