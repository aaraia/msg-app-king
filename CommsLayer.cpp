#include "CommsLayer.h"


//  app
#include "Message.h"

namespace MessageService {

bool CommsLayer::connect(const uint32_t connectionID, const MessageID id, CallBack callback)
{
    std::unique_lock<std::mutex> lock{ m_connectionMutex };

    //  must be greater than zero
    if (connectionID <= 0) return false;

    //  connect to recieve a message, if possible
    if (m_connections.size() >= MAX_CONNECTIONS)
    {
        return false;
    }

    auto connectionsMapIter = m_connections.find(connectionID);
    if (connectionsMapIter == m_connections.end())
    {
        //  no connections
        auto add = m_connections.emplace(connectionID, MessageCallBackMap{});
        if (!add.second) return false;
        connectionsMapIter = add.first;
    }

    auto& msgCallBackMap = connectionsMapIter->second;
    auto callBackVecIter = msgCallBackMap.find(id);
    if (callBackVecIter == msgCallBackMap.end())
    {
        //  no callbacks
        auto add = msgCallBackMap.emplace(id, CallBackVec{});
        if (!add.second) return false;
        callBackVecIter = add.first;
    }

    //  add callback if there is room
    auto& callBackVec = callBackVecIter->second;
    if (callBackVec.size() >= MAX_CALLBACKS_PER_MESSAGE_ID) return false;
    callBackVec.emplace_back(callback);

    return true;
}

void CommsLayer::shutdown()
{
    BaseQueue<BaseMessageRef>::shutdown();
}

void CommsLayer::work(BaseMessageRef msg)
{
    //  check if a shutdown has been intitated
    if (m_shutdown) return;

    std::unique_lock<std::mutex> lock{ m_connectionMutex };

    //  find this conenctions callbacks
    auto toConnection = m_connections.find(msg->getToID());
    if (toConnection == m_connections.end()) return;         //  TODO exception or log here?

    //  find all callbacks that have connected to this message type
    auto& messageMap = toConnection->second;
    auto callBackMap = messageMap.find(msg->getId());
    if (callBackMap == messageMap.end()) return;         //  TODO exception or log here?

    //  call the callbacks
    auto& callBackList = callBackMap->second;
    for (auto& callBack : callBackList)
    {
        callBack(msg);
    }
}
}