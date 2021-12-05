#ifndef H_GETALLMSGSTATE_H
#define H_GETALLMSGSTATE_H


//  app
#include "State.h"

namespace MessageService {

//  forwards, enums, aliases
class StateMachine;
class FrontEnd;

class GetAllMsgState : public State
{
    //  no move/copy
    GetAllMsgState& operator=(GetAllMsgState&&) = delete;
    GetAllMsgState& operator=(const GetAllMsgState&) = delete;
    GetAllMsgState(GetAllMsgState&&) = delete;
    GetAllMsgState(const GetAllMsgState&) = delete;

public:
    GetAllMsgState(StateMachine& sm, FrontEnd& frontEnd)
        : State(STATE_ID::eGET_ALL_MESSAGES_STATE_ID, sm)
        , m_frontEnd(frontEnd) {}

    ~GetAllMsgState() {}

    bool update() override;

private:
    FrontEnd& m_frontEnd;
};

}

#endif
