#ifndef H_BACKEND_H
#define H_BACKEND_H


//  app
#include "BaseQueue.h"

//  stl
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace MessageService {

//  forwards, enums, aliases
class CommsLayer;
struct BaseMessage;

using BaseMessageRef = std::shared_ptr<BaseMessage>;

struct Limits
{
    uint32_t MAX_USERS = 20;
    uint32_t MAX_TOTAL_MESSAGES = 200;
    uint32_t MAX_MESSAGES_PER_USER = 10;
    uint32_t MAX_MESSAGES_RETRIEVED_PER_REPLY = 10;
    uint32_t MAX_TOTAL_MESSAGES_RETRIEVED = 200;
};

//  Back end
class BackEnd : public BaseQueue<BaseMessageRef>
{
    using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
    using TimeStampVec = std::vector<TimeStamp>;
    using ReportEntryMap = std::unordered_map<std::string, TimeStampVec>;

    struct Report
    {
        ReportEntryMap m_sentList;
        std::string m_sender;
        uint32_t m_count;
    };

    struct Email
    {
        std::string m_sender;
        std::string m_content;
    };

    using EmailVec = std::vector<Email>;

    //  no move/copy for now
    BackEnd& operator=(BackEnd&&) = delete;
    BackEnd& operator=(const BackEnd&) = delete;
    BackEnd(BackEnd&&) = delete;
    BackEnd(const BackEnd&) = delete;

public:
    explicit BackEnd(CommsLayer& comms, const Limits& limits, const uint32_t numWorkers = 1)
        : BaseQueue(numWorkers)
        , m_limits(limits)
        , m_commsLayer(comms) {}

    void shutdown() override;

    void setID(const uint32_t id) { m_id = id; }
    uint32_t getID() const { return m_id; }

private:
    void work(BaseMessageRef msg) override;

    void AddUserHandler(BaseMessageRef msg);
    void SendMessageHandler(BaseMessageRef msg);
    void GetMessagesHandler(BaseMessageRef msg);
    void GetReportHandler(BaseMessageRef msg);

private:
    std::unordered_set<std::string> m_users;               //  users
    std::unordered_map<std::string, Report> m_reports;     //  sender / report
    std::unordered_map<std::string, EmailVec> m_messages;  //  recipient / email
    const Limits m_limits;
    std::mutex m_usersMutex;
    std::mutex m_messagesMutex;
    std::mutex m_reportsMutex;
    CommsLayer& m_commsLayer;   //  lifetime of CommsLayer and BackEnd match are tied to each other
    uint32_t m_id = 0;
    uint32_t m_totalMessagesStored = 0;
};

}

#endif