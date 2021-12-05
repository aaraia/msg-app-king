#include "StateMachine.h"


//  app
#include "State.h"
#include "Message.h"

namespace MessageService {

void StateMachine::addState(StateURef state)
{
    if (!state) return;

    STATE_ID id = state->id();   
    m_states.emplace(id, std::move(state));
}

void StateMachine::changeState(STATE_ID newState)
{
    auto fnd = m_states.find(newState);
    if (fnd != m_states.end())
    {
        m_currentState = newState;
    }
}

void StateMachine::pushToState(BaseMessageRef msg)
{
    m_states[m_currentState]->recieve(msg);
}

bool StateMachine::run()
{
    return m_states[m_currentState]->update();
}

}