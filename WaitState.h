#ifndef H_WAITSTATE_H
#define H_WAITSTATE_H


//  app
#include "State.h"

//  stl
#include <chrono>

namespace MessageService {

//  forwards, enums, aliases
class StateMachine;


class WaitState : public State
{
    //  no move/copy
    WaitState& operator=(WaitState&&) = delete;
    WaitState& operator=(const WaitState&) = delete;
    WaitState(WaitState&&) = delete;
    WaitState(const WaitState&) = delete;

public:
    WaitState(StateMachine& sm)
        : State(STATE_ID::eWAIT_STATE_ID, sm) {}

    ~WaitState() {}

    bool update() override;

private:
    std::chrono::steady_clock::time_point m_start;
    bool m_firstPass = true;
};

}

#endif
