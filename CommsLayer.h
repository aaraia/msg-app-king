#ifndef H_COMMSLAYER_H
#define H_COMMSLAYER_H


//  app
#include "BaseQueue.h"
#include "MessageIDs.h"

//  stl
#include <unordered_map>
#include <functional>

namespace MessageService {

//  forwards, enums, aliases
struct BaseMessage;
using BaseMessageRef = std::shared_ptr<BaseMessage>;
using CallBack = std::function<void(BaseMessageRef)>;
using CallBackVec = std::vector<CallBack>;
using MessageCallBackMap = std::unordered_map<MessageID, CallBackVec>;
using ConnectionMap = std::unordered_map<uint32_t, MessageCallBackMap>;

//  communication layer
class CommsLayer : public BaseQueue<BaseMessageRef>
{
    static const uint32_t MAX_CONNECTIONS = 15;
    static const uint32_t MAX_CALLBACKS_PER_MESSAGE_ID = 5;

    //  no move/copy
    CommsLayer& operator=(CommsLayer&&) = delete;
    BaseQueue& operator=(const CommsLayer&) = delete;
    CommsLayer(CommsLayer&&) = delete;
    CommsLayer(const CommsLayer&) = delete;

public:
    explicit CommsLayer(const uint32_t numWorkers = 1)
        : BaseQueue(numWorkers) {}

    bool connect(const uint32_t connectionID, const MessageID id, CallBack callback);
    void shutdown() override;
    uint32_t getID() { return ++m_nextID; }

private:
    void work(BaseMessageRef msg) override;

private:
    ConnectionMap m_connections;
    std::mutex m_connectionMutex;
    uint32_t m_nextID = 0;
};

}


#endif
