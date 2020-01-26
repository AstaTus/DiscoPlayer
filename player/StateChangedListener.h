#ifndef __STATE_CHANGED_LISTENER_H__
#define __STATE_CHANGED_LISTENER_H__
class StateChangedListener
{
private:
    /* data */
public:
    StateChangedListener(/* args */) = default;
    virtual ~StateChangedListener() = default;

    virtual void on_state_changed(StateManager::PlayState state) = 0;
};

#endif