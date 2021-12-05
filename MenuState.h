#ifndef H_MENUSTATE_H
#define H_MENUSTATE_H


//  app
#include "State.h"

namespace MessageService {

//  forwards, enums, aliases
class StateMachine;
class FrontEnd;

class MenuState : public State
{
    //  no move/copy
    MenuState& operator=(MenuState&&) = delete;
    MenuState& operator=(const MenuState&) = delete;
    MenuState(MenuState&&) = delete;
    MenuState(const MenuState&) = delete;

public:
    MenuState(StateMachine& sm, FrontEnd& frontEnd)
        : State(STATE_ID::eMENU_STATE_ID, sm)
        , m_frontEnd(frontEnd) {}

    ~MenuState() {}

    bool update() override;

private:
    FrontEnd& m_frontEnd;
};

}

#endif
