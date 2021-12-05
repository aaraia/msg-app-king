#ifndef H_FRONTEND_H
#define H_FRONTEND_H


//  app
#include "BaseQueue.h"
#include "StateMachine.h"

namespace MessageService {

//  forwards, enums, aliases
class CommsLayer;
struct BaseMessage;
using BaseMessageRef = std::shared_ptr<BaseMessage>;

//  front end 
class FrontEnd : public BaseQueue<BaseMessageRef>
{
    //  no move/copy for now
    FrontEnd& operator=(FrontEnd&&) = delete;
    FrontEnd& operator=(const FrontEnd&) = delete;
    FrontEnd(FrontEnd&&) = delete;
    FrontEnd(const FrontEnd&) = delete;

public:
    explicit FrontEnd(CommsLayer& comms, const uint32_t numWorkers = 1);

    void run();
    void shutdown() override;
    void setID(const uint32_t id) { m_id = id; }
    uint32_t getID() const { return m_id; }
    void setBackEndID(const uint32_t id) { m_backEndID = id; }
    uint32_t getBackEndID() const { return m_backEndID; }

private:
    void work(BaseMessageRef msg) override;

private:
    std::queue<BaseMessageRef> m_resultData;
    std::mutex m_resultDataMutex;
    StateMachine m_machine;
    CommsLayer& m_commsLayer;
    uint32_t m_id = 0;
    uint32_t m_backEndID = 0;
};

}

#endif