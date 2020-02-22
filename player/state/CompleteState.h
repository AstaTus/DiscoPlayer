#pragma once

#include "BaseState.h"

class CompleteState: public BaseState
{
private:
    /* data */
public:
    CompleteState(/* args */);
    ~CompleteState();

    virtual void on_state_enter(...) override;

    virtual void on_state_exit() override;
};
