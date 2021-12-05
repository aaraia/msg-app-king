#include "MessageApp.h"


//  app
#include "Logger.h"

namespace MessageService {

MessageApp::MessageApp()
    : m_commsLayer{ 4 }
    , m_backEnd{ m_commsLayer, Limits{}, 4 }
    , m_frontEnd{ m_commsLayer, 4 }
{
    //  get id's from comms layer
    m_frontEnd.setID(m_commsLayer.getID());
    m_backEnd.setID(m_commsLayer.getID());

    //  frontend needs to know the backend id
    //  ideally the frontend would connect to a API gateway
    //  and then any messages would be sent to a cluster of servers that do the processing
    m_frontEnd.setBackEndID(m_backEnd.getID());

    //  connect to messages of interest for each id
    bool success = true;
    success &= m_commsLayer.connect(m_frontEnd.getID(), MessageID::AddUserReplyMessageID, std::bind(&FrontEnd::push, &m_frontEnd, std::placeholders::_1));
    success &= m_commsLayer.connect(m_frontEnd.getID(), MessageID::SendMessageReplyID, std::bind(&FrontEnd::push, &m_frontEnd, std::placeholders::_1));
    success &= m_commsLayer.connect(m_frontEnd.getID(), MessageID::GetMessagesReplyID, std::bind(&FrontEnd::push, &m_frontEnd, std::placeholders::_1));
    success &= m_commsLayer.connect(m_frontEnd.getID(), MessageID::GetReportReplyID, std::bind(&FrontEnd::push, &m_frontEnd, std::placeholders::_1));

    success &= m_commsLayer.connect(m_backEnd.getID(), MessageID::AddUserMessageID, std::bind(&BackEnd::push, &m_backEnd, std::placeholders::_1));
    success &= m_commsLayer.connect(m_backEnd.getID(), MessageID::SendMessageID, std::bind(&BackEnd::push, &m_backEnd, std::placeholders::_1));
    success &= m_commsLayer.connect(m_backEnd.getID(), MessageID::GetMessagesID, std::bind(&BackEnd::push, &m_backEnd, std::placeholders::_1));
    success &= m_commsLayer.connect(m_backEnd.getID(), MessageID::GetReportID, std::bind(&BackEnd::push, &m_backEnd, std::placeholders::_1));

    if (!success)
    {
        Utilities::Logger::get().log("failed successful connection");
    }
}

MessageApp::~MessageApp()
{
    m_commsLayer.shutdown();
    m_backEnd.shutdown();
    m_frontEnd.shutdown();
}

void MessageApp::run()
{
    m_frontEnd.run();
}

}





