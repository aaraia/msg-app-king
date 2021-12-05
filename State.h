#ifndef H_STATE_H
#define H_STATE_H


//  stl
#include <iostream>

//  app
#include "StateIDs.h"


namespace MessageService {

//  forwards, enums, aliases
class StateMachine;
struct BaseMessage;
using BaseMessageRef = std::shared_ptr<BaseMessage>;

class State
{
    //  no move/copy
    State& operator=(State&&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) = delete;
    State(const State&) = delete;

public:
    State(STATE_ID id, StateMachine& sm)
        : m_statemachine(sm)
        , m_id(id) {}

    virtual ~State() {}

    STATE_ID id() const { return m_id; }

    virtual void recieve(BaseMessageRef msg) {}
    virtual bool update() = 0;

protected:
    StateMachine& m_statemachine;  //  statemachine owns the states so life times are tied together
    STATE_ID m_id;
};

}

#endif