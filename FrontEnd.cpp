#include "FrontEnd.h"



//  app
#include "Message.h"
#include "StateMachine.h"
#include "MenuState.h"
#include "AddUserState.h"
#include "GetAllMsgState.h"
#include "SendMsgState.h"
#include "WaitState.h"
#include "ResultState.h"
#include "CommsLayer.h"

namespace MessageService {

FrontEnd::FrontEnd(CommsLayer& comms, const uint32_t numWorkers)
    : BaseQueue(numWorkers)
    , m_commsLayer(comms)
{
    m_machine.addState(std::make_unique<MenuState>(m_machine, *this));
    m_machine.addState(std::make_unique<AddUserState>(m_machine, *this));
    m_machine.addState(std::make_unique<GetAllMsgState>(m_machine, *this));
    m_machine.addState(std::make_unique<SendMsgState>(m_machine, *this));
    m_machine.addState(std::make_unique<WaitState>(m_machine));
    m_machine.addState(std::make_unique<ResultState>(m_machine));
}

void FrontEnd::run()
{
    m_machine.changeState(STATE_ID::eMENU_STATE_ID);

    while (m_machine.run())
    {
        //  check for any result data and paas to the state
        if (!m_resultData.empty())
        {
            //  waiting for a result? move to result state
            if (m_machine.getCurrentStateID() == STATE_ID::eWAIT_STATE_ID)
            {
                m_machine.changeState(STATE_ID::eRESULT_STATE_ID);
            }
            else if (m_machine.getCurrentStateID() == STATE_ID::eRESULT_STATE_ID)
            {
                //  a thread could be returning data so acquire a lock
                std::unique_lock<std::mutex> lock{ m_resultDataMutex };
                while (!m_resultData.empty())
                {
                    auto result = m_resultData.front();
                    m_machine.pushToState(result);
                    m_resultData.pop();
                }
            }
            else
            {
                //  discard any results, we have timed out
                std::unique_lock<std::mutex> lock{ m_resultDataMutex };
                std::queue<BaseMessageRef>().swap(m_resultData);
            }
        }
    }
}

void FrontEnd::shutdown()
{
    BaseQueue<BaseMessageRef>::shutdown();
}

void FrontEnd::work(BaseMessageRef msg)
{
    //  check if a shutdown has been intitated
    if (m_shutdown) return;
    if (!msg) return;

    switch (msg->getId())
    {
    case MessageID::AddUserMessageID:
    case MessageID::SendMessageID:
    case MessageID::GetMessagesID:
    case MessageID::GetReportID:
    {
        m_commsLayer.push(msg);
    }
    break;
    case MessageID::AddUserReplyMessageID:
    case MessageID::SendMessageReplyID:
    case MessageID::GetMessagesReplyID:
    case MessageID::GetReportReplyID:
    {
        //  store the result data
        std::unique_lock<std::mutex> lock{ m_resultDataMutex };
        m_resultData.push(msg);
    }
    break;
    }
}

}