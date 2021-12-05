#ifndef H_RESULTSTATE_H
#define H_RESULTSTATE_H


//  app
#include "State.h"

//  stl
#include <queue>

namespace MessageService {

//  forwards, enums, aliases
class StateMachine;


class ResultState : public State
{
    //  no move/copy
    ResultState& operator=(ResultState&&) = delete;
    ResultState& operator=(const ResultState&) = delete;
    ResultState(ResultState&&) = delete;
    ResultState(const ResultState&) = delete;

public:
    ResultState(StateMachine& sm)
        : State(STATE_ID::eRESULT_STATE_ID, sm) {}

    ~ResultState() {}

    void recieve(BaseMessageRef msg) override;
    bool update() override;

private:
    std::queue<std::string> m_output;
    bool m_stop = false;
    bool m_outputFinished = false;
};

}

#endif
