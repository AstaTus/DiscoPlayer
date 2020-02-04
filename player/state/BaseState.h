#ifndef __BASE_STATE_H__
#define __BASE_STATE_H__

class BaseState {
public:
    BaseState() = default;
    virtual ~BaseState() = default;

    /**
     * 不能在该函数内跳转 state
    */
    virtual void on_state_enter(...) = 0;
    /**
     * 不能在该函数内跳转 state
    */
    virtual void on_state_exit() = 0;
};
#endif