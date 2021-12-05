#ifndef H_SENDMSGSTATE_H
#define H_SENDMSGSTATE_H


//  app
#include "State.h"

namespace MessageService {

//  forwards, enums, aliases
class StateMachine;
class FrontEnd;

class SendMsgState : public State
{
    //  no move/copy
    SendMsgState& operator=(SendMsgState&&) = delete;
    SendMsgState& operator=(const SendMsgState&) = delete;
    SendMsgState(SendMsgState&&) = delete;
    SendMsgState(const SendMsgState&) = delete;

public:
    SendMsgState(StateMachine& sm, FrontEnd& frontEnd)
        : State(STATE_ID::eSEND_MSG_STATE_ID, sm)
        , m_frontEnd(frontEnd) {}

    ~SendMsgState() {}

    bool update() override;

private:
    FrontEnd& m_frontEnd;
};

}
#endif