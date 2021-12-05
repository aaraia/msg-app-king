#include "ResultState.h"


//  app
#include "StateMachine.h"
#include "Message.h"

namespace MessageService {

void ResultState::recieve(BaseMessageRef msg)
{
    switch (msg->getId())
    {
    case MessageID::AddUserReplyMessageID:
    {
        auto reply = std::dynamic_pointer_cast<AddUserReply>(msg);
        m_output.push(reply->geReply());
        m_outputFinished = true;
    }
    break;
    case MessageID::SendMessageReplyID:
    {
        auto reply = std::dynamic_pointer_cast<SendMessageReply>(msg);
        m_output.push(reply->getReply());
        m_outputFinished = true;
    }
    break;
    case MessageID::GetMessagesReplyID:
    {
        auto reply = std::dynamic_pointer_cast<GetMessagesReply>(msg);
        auto messages = reply->getMessages();

        // TODO use sequence id to ensure the messages are processed in the order they were sent
        //reply->getSequenceID()

        if (messages.empty())
        {
            m_output.push(reply->getReply());
        }
        else
        {
            for (const auto& m : messages)
            {
                m_output.push("Message \n");
                m_output.push("From: " + m.first + "\n");
                m_output.push("Content: " + m.second + "\n");
                m_output.push("-----");
            }
        }

        if (reply->getFlag())
        {
            m_outputFinished = true;
        }
    }
    break;
    case MessageID::GetReportReplyID:
    {
        auto reply = std::dynamic_pointer_cast<GetReportReply>(msg);
        auto report = reply->getReport();

        // TODO use sequence id to ensure the messages are processed in the order they were sent
        // maybe a priority queue
        //reply->getSequenceID()

        if (report.empty())
        {
            m_output.push(reply->getReply());
        }
        else
        {
            for (const auto& reportStr : report)
            {
                m_output.push(reportStr);
            }
        }

        if (reply->getFlag())
        {
            m_outputFinished = true;
        }
    }
    break;
    }
}

bool ResultState::update()
{
    if (!m_stop)
    {
        if (!m_output.empty())
        {
            std::cout << m_output.front();
            m_output.pop();
        }
        else
        {
            m_stop = m_outputFinished;
        }
    }
    else
    {
        m_stop = false;
        m_outputFinished = false;

        std::cout << std::endl << "Enter any key and press return to continue.....";
        std::string str;
        std::getline(std::cin, str);

        m_statemachine.changeState(STATE_ID::eMENU_STATE_ID);
    }

    return true;
}

}