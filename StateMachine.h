#ifndef H_STATEMACHINE_H
#define H_STATEMACHINE_H



//  app
#include "StateIDs.h"
#include "State.h"

//  stl
#include <unordered_map>
#include <memory>
#include <mutex>

namespace MessageService {

//  forwards, enums, aliases
class State;
struct BaseMessage;
using StateURef = std::unique_ptr<State>;
using BaseMessageRef = std::shared_ptr<BaseMessage>;

class StateMachine
{
    //  no move/copy
    StateMachine& operator=(StateMachine&&) = delete;
    StateMachine& operator=(const StateMachine&) = delete;
    StateMachine(StateMachine&&) = delete;
    StateMachine(const StateMachine&) = delete;

public:
    StateMachine() = default;

    void addState(StateURef state);
    void changeState(STATE_ID newState);
    STATE_ID getCurrentStateID() const { return m_currentState; }
    void pushToState(BaseMessageRef msg);

    bool run();

private:
    std::unordered_map<STATE_ID, StateURef> m_states;
    std::mutex m_stateMutex;
    STATE_ID m_currentState = STATE_ID::eMENU_STATE_ID;
};

}
#endif
