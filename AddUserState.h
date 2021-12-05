#ifndef H_ADDUSERSTATE_H
#define H_ADDUSERSTATE_H


//  app
#include "State.h"

namespace MessageService {

//  forwards, enums, aliases
class StateMachine;
class FrontEnd;

class AddUserState : public State
{
    //  no move/copy for now
    AddUserState& operator=(AddUserState&&) = delete;
    AddUserState& operator=(const AddUserState&) = delete;
    AddUserState(AddUserState&&) = delete;
    AddUserState(const AddUserState&) = delete;

public:
    AddUserState(StateMachine& sm, FrontEnd& frontEnd)
        : State(STATE_ID::eADD_USER_STATE_ID, sm)
        , m_frontEnd(frontEnd) {}

    ~AddUserState() {}

    bool update() override;

private:
    FrontEnd& m_frontEnd; //  both FrontEnd and AddUserState lifetimes are tied
};

}

#endif
