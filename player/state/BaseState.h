#ifndef __BASE_STATE_H__
#define __BASE_STATE_H__
class BaseState {
public:
    BaseState() = default;
    virtual ~BaseState() = default;

    virtual void on_state_enter() = 0;
    virtual void on_state_exit() = 0;
};
#endif