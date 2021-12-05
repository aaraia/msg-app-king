#ifndef H_MESSAGE_H
#define H_MESSAGE_H


//  app
#include "MessageIDs.h"

//  stl
#include <string>
#include <memory>

namespace MessageService {

//  forwards, enums, aliases
struct AddUserMessage;
struct AddUserReply;
struct SendMessage;
struct SendMessageReply;
struct GetMessages;
struct GetMessagesReply;
struct GetReport;
struct GetReportReply;

using AddUserMessageRef = std::shared_ptr<AddUserMessage>;
using AddUserReplyRef = std::shared_ptr<AddUserReply>;
using SendMessageRef = std::shared_ptr<SendMessage>;
using SendMessageReplyRef = std::shared_ptr<SendMessageReply>;
using GetMessagesRef = std::shared_ptr<GetMessages>;
using GetMessagesReplyRef = std::shared_ptr<GetMessagesReply>;
using GetReportRef = std::shared_ptr<GetReport>;
using GetReportReplyRef = std::shared_ptr<GetReportReply>;
using MessagesVec = std::vector<std::pair<std::string, std::string>>;
using ReportVec = std::vector<std::string>;

struct BaseMessage
{
    BaseMessage(MessageID id, const uint32_t toID, const uint32_t fromID)
        : m_id(id)
        , m_toID(toID)
        , m_fromID(fromID)
    {}

    virtual ~BaseMessage() = 0 {}

    MessageID getId() const { return m_id; };
    uint32_t getToID() const { return m_toID; }
    uint32_t getFromID() const { return m_fromID; }

private:
    MessageID m_id;
    uint32_t m_toID = 0;  //  who did this message come from/going to
    uint32_t m_fromID = 0;
};

//  add user
struct AddUserMessage : public BaseMessage
{
    static AddUserMessageRef create(const uint32_t toID, const uint32_t fromID, const std::string& userName)
    {
        return std::make_shared<AddUserMessage>(toID, fromID, userName);
    }

    std::string getUserName() const { return m_userName; }

    AddUserMessage(const uint32_t toID, const uint32_t fromID, const std::string& userName)
        : BaseMessage(MessageID::AddUserMessageID, toID, fromID)
        , m_userName(userName) {}

private:
    std::string m_userName;
};

struct AddUserReply : public BaseMessage
{
    static AddUserReplyRef create(const uint32_t toID, const uint32_t fromID, const std::string& reply)
    {
        return std::make_shared<AddUserReply>(toID, fromID, reply);
    }

    AddUserReply(const uint32_t toID, const uint32_t fromID, const std::string& reply)
        : BaseMessage(MessageID::AddUserReplyMessageID, toID, fromID)
        , m_reply(reply) {}

    std::string geReply() const { return m_reply; }

private:
    std::string m_reply;
};


//  send message
struct SendMessage : public BaseMessage
{
    static SendMessageRef create(const uint32_t toID, 
        const uint32_t fromID,
        const std::string& sender,
        const std::string& recipient,
        const std::string& message)
    {
        return std::make_shared<SendMessage>(toID, fromID, sender, recipient, message);
    }

    SendMessage(const uint32_t toID, 
        const uint32_t fromID,
        const std::string& sender,
        const std::string& recipient,
        const std::string& message)
        : BaseMessage(MessageID::SendMessageID, toID, fromID)
        , m_sender(sender)
        , m_recipient(recipient)
        , m_message(message) {}

    std::string getSender() const { return m_sender; }
    std::string getRecipient() const { return m_recipient; }
    std::string getMessage() const { return m_message; }

private:
    std::string m_sender;
    std::string m_recipient;
    std::string m_message;
};

struct SendMessageReply : public BaseMessage
{
    static SendMessageReplyRef create(const uint32_t toID, const uint32_t fromID, const std::string& reply)
    {
        return std::make_shared<SendMessageReply>(toID, fromID, reply);
    }

    SendMessageReply(const uint32_t toID, const uint32_t fromID, const std::string& reply)
        : BaseMessage(MessageID::SendMessageReplyID, toID, fromID)
        , m_reply(reply) {}

    std::string getReply() const { return m_reply; }

private:
    std::string m_reply;
};


//  get messages
struct GetMessages : public BaseMessage
{
    static GetMessagesRef create(const uint32_t toID, const uint32_t fromID, const std::string& user)
    {
        return std::make_shared<GetMessages>(toID, fromID, user);
    }

    GetMessages(const uint32_t toID, const uint32_t fromID, const std::string& user)
        : BaseMessage(MessageID::GetMessagesID, toID, fromID)
        , m_userName(user) {}

    std::string getUserName() { return m_userName; }

private:
    std::string m_userName;
};

struct GetMessagesReply : public BaseMessage
{
    static GetMessagesReplyRef create(const uint32_t toID, 
        const uint32_t fromID,
        const MessagesVec& messages,
        const std::string& reply,
        const uint32_t sequenceID,
        const bool flag)
    {
        return std::make_shared<GetMessagesReply>(toID, fromID, messages, reply, sequenceID, flag);
    }

    GetMessagesReply(const uint32_t toID, 
        const uint32_t fromID, 
        const MessagesVec& messages,
        const std::string& reply,
        const uint32_t sequenceID,
        const bool flag)
        : BaseMessage(MessageID::GetMessagesReplyID, toID, fromID)
        , m_messages(messages)
        , m_reply(reply)
        , m_sequenceID(sequenceID)
        , m_flag(flag)
    {}

    MessagesVec getMessages() const { return m_messages; }
    std::string getReply() const { return m_reply; }
    bool getFlag() const { return m_flag; }
    uint32_t getSequenceID() const { return m_sequenceID; }

private:
    MessagesVec m_messages;
    std::string m_reply;
    uint32_t m_sequenceID; //  used to work out order of the stream of messages returned by the backend
    bool m_flag = false;  //  marks end of transmission when set to false
};


//  get report
struct GetReport : public BaseMessage
{
    static GetReportRef create(const uint32_t toID, const uint32_t fromID)
    {
        return std::make_shared<GetReport>(toID, fromID);
    }

    GetReport(const uint32_t toID, const uint32_t fromID) 
        : BaseMessage(MessageID::GetReportID, toID, fromID) {}
};

struct GetReportReply : public BaseMessage
{
    static GetReportReplyRef create(const uint32_t toID, 
        const uint32_t fromID,
        const ReportVec& report,
        const std::string& reply,
        const uint32_t sequenceID,
        const bool flag)
    {
        return std::make_shared<GetReportReply>(toID, fromID, report, reply, sequenceID, flag);
    }

    GetReportReply(const uint32_t toID, 
        const uint32_t fromID,
        const ReportVec& report,
        const std::string& reply,
        const uint32_t sequenceID,
        const bool flag)
        : BaseMessage(MessageID::GetReportReplyID, toID, fromID)
        , m_report(report)
        , m_reply(reply)
        , m_sequenceID(sequenceID)
        , m_flag(flag)
    {}

    ReportVec getReport() const { return m_report; }
    std::string getReply() const { return m_reply; }
    bool getFlag() const { return m_flag; }
    uint32_t getSequenceID() const { return m_sequenceID; }

private:
    ReportVec m_report;
    std::string m_reply;
    uint32_t m_sequenceID; //  used to work out order of the stream of messages returned by the backend
    bool m_flag = false;  //  marks end of transmission when set to false
};

}

#endif
