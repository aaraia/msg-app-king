#include "BackEnd.h"


//  app
#include "CommsLayer.h"
#include "Message.h"

namespace MessageService {

void BackEnd::shutdown()
{
    BaseQueue<BaseMessageRef>::shutdown();
}

void BackEnd::work(BaseMessageRef msg)
{
    //  check if a shutdown has been intitated
    if (m_shutdown) return;

    switch (msg->getId())
    {
    case MessageID::AddUserMessageID:
        AddUserHandler(msg);
        break;
    case MessageID::SendMessageID:
        SendMessageHandler(msg);
        break;
    case MessageID::GetMessagesID:
        GetMessagesHandler(msg);
        break;
    case MessageID::GetReportID:
        GetReportHandler(msg);
        break;
    default:
        break;
    }
}

void BackEnd::AddUserHandler(BaseMessageRef msg)
{
    std::unique_lock<std::mutex> lock{ m_usersMutex };

    auto addUserMsg = std::dynamic_pointer_cast<AddUserMessage>(msg);
    auto inputedUserName = addUserMsg->getUserName();

    //  max users reached
    if (m_users.size() >= m_limits.MAX_USERS)
    {
        //  send reply
        auto reply = AddUserReply::create(msg->getFromID(), m_id, "cannot add user: " + inputedUserName + " max users reached");
        m_commsLayer.push(reply); 
        return;
    }

    //  already exists?
    auto fnd = m_users.find(inputedUserName);
    if (fnd != m_users.end())
    {
        // send reply back
        auto reply = AddUserReply::create(msg->getFromID(), m_id, "cannot add user: " + inputedUserName + " already exists");
        m_commsLayer.push(reply);
        return;
    }

    //  add the user
    auto add = m_users.emplace(inputedUserName);
    if (!add.second)
    {
        // send reply back
        auto reply = AddUserReply::create(msg->getFromID(), m_id, "cannot add user: " + inputedUserName + " add failed");
        m_commsLayer.push(reply);
        return;
    }

    //  send reply back 
    auto reply = AddUserReply::create(msg->getFromID(), m_id, "User: " + inputedUserName + " added successfully");
    m_commsLayer.push(reply);
}

void BackEnd::SendMessageHandler(BaseMessageRef msg)
{
    if (!msg) return;

    //  cast down to get access to the message data
    auto sendMsgRef = std::dynamic_pointer_cast<SendMessage>(msg);
    auto inputedSender = sendMsgRef->getSender();
    auto inputedRecipent = sendMsgRef->getRecipient();
    auto inputedMessage = sendMsgRef->getMessage();

    {
        //  lock will be released at end of block
        std::unique_lock<std::mutex> lock{ m_usersMutex };

        //  first check if the sender exists
        auto fnd = m_users.find(inputedSender);
        if (fnd == m_users.end())
        {
            //  send reply back
            auto reply = SendMessageReply::create(msg->getFromID(), m_id, "cannot send message: sender does not exist");
            m_commsLayer.push(reply);

            return;
        }

        //  first check if the recipient exists
        fnd = m_users.find(inputedRecipent);
        if (fnd == m_users.end())
        {
            //  send reply back
            auto reply = SendMessageReply::create(msg->getFromID(), m_id, "cannot send message: recipient does not exist");
            m_commsLayer.push(reply);

            return;
        }
    }

    {
        //  lock will be released at end of block
        std::unique_lock<std::mutex> lock{ m_messagesMutex };

        //  max total messages reached?
        if (m_totalMessagesStored >= m_limits.MAX_TOTAL_MESSAGES)
        {
            //  send reply back
            auto reply = SendMessageReply::create(msg->getFromID(), m_id, "cannot send message: max total messages reached");
            m_commsLayer.push(reply);

            return;
        }

        //  recipient does not exist
        auto inbox = m_messages.find(inputedRecipent);
        if (inbox == m_messages.end())
        {
            auto add = m_messages.emplace(inputedRecipent, EmailVec{});
            if (!add.second)
            {
                //  send reply back
                auto reply = SendMessageReply::create(msg->getFromID(), m_id, "cannot send message: could not add recipient");
                m_commsLayer.push(reply);
                return;
            }

            inbox = add.first;
        }


        //  recipient inbox full
        if (inbox->second.size() >= m_limits.MAX_MESSAGES_PER_USER)
        {
            //  send reply back
            auto reply = SendMessageReply::create(msg->getFromID(), m_id, "cannot send message: recipient inbox full");
            m_commsLayer.push(reply);
            return;
        }

        //  add message to recipent inbox
        Email email;
        email.m_content = inputedMessage;
        email.m_sender = inputedSender;
        inbox->second.emplace_back(email);

        //  send reply that it was successfully sent
        auto reply = SendMessageReply::create(msg->getFromID(), m_id, "Message sent");
        m_commsLayer.push(std::move(reply));

        ++m_totalMessagesStored;
    }

    {
        std::unique_lock<std::mutex> lock{ m_reportsMutex };

        //  the backend generates the timestamps
        TimeStamp timeStamp = std::chrono::system_clock::now();

        //  add to our report, keep a record of this message meta data
        auto fnd = m_reports.find(inputedSender);
        if (fnd == m_reports.end())
        {
            //  create a new report
            Report report;
            report.m_count = 1;
            report.m_sender = inputedSender;

            //  add recipient
            TimeStampVec timeStamps;
            timeStamps.emplace_back(timeStamp);
            report.m_sentList.emplace(inputedRecipent, timeStamps);  //  this could fail but the message was still sent,  maybe some logging instead

            //  add report
            m_reports.emplace(inputedSender, report);  //  this could fail but the message was still sent, maybe some logging instead
        }
        else
        {
            auto& report = fnd->second;

            //  find the recipent
            auto fndRpt = report.m_sentList.find(inputedRecipent);
            if (fndRpt == report.m_sentList.end())
            {
                //  add recipient
                TimeStampVec timeStamps;
                timeStamps.emplace_back(timeStamp);

                report.m_sentList.emplace(inputedRecipent, timeStamps);  //  this could fail but the message was still sent, maybe log
            }
            else
            {
                //  add the new time stamp
                fndRpt->second.emplace_back(timeStamp);
            }

            report.m_count += 1;  // new message increment
        }
    }
}

void BackEnd::GetMessagesHandler(BaseMessageRef msg)
{
    if (!msg) return;

    auto getMessageRef = std::dynamic_pointer_cast<GetMessages>(msg);
    auto user = getMessageRef->getUserName();
    uint32_t sequenceID = 0;

    {
        //  lock will be released at end of block
        std::unique_lock<std::mutex> lock{ m_usersMutex };

        //  first check if the user exists
        auto fnd = m_users.find(user);
        if (fnd == m_users.end())
        {
            //  send reply back
            auto reply = GetMessagesReply::create(msg->getFromID(), m_id, MessagesVec{}, "cannot retrieve messages : user does not exist", ++sequenceID, true);
            m_commsLayer.push(reply);
            return;
        }
    }

    //  acquire lock on message data
    std::unique_lock<std::mutex> lock{ m_messagesMutex };

    //  double check that this user is valid?
    auto inbox = m_messages.find(user);
    if (inbox == m_messages.end())
    {
        //  no such user, send reply back
        auto reply = GetMessagesReply::create(msg->getFromID(), m_id, MessagesVec{}, "cannot retrieve message: user does not have any messages", ++sequenceID, true);
        m_commsLayer.push(reply);
        return;
    }

    //  any messages?
    if (inbox->second.empty())
    {
        //  no messages, send reply back
        auto reply = GetMessagesReply::create(msg->getFromID(), m_id, MessagesVec{}, "cannot retrieve message: no messages exist", ++sequenceID, true);
        m_commsLayer.push(reply);
        return;
    }

    //  batch the messages and send
    uint32_t count = 0;
    uint32_t total = 0;
    std::vector<std::pair<std::string, std::string>> messages;

    //  loop until we have retireved all messages or hit the limit
    auto iter = inbox->second.begin();
    while (iter != inbox->second.end() && total < m_limits.MAX_TOTAL_MESSAGES_RETRIEVED)
    {
        if (count >= m_limits.MAX_MESSAGES_RETRIEVED_PER_REPLY)
        {
            //  create and send reply
            auto reply = GetMessagesReply::create(msg->getFromID(), m_id, messages, "", ++sequenceID, false);
            m_commsLayer.push(reply);

            //  create new message reply
            messages.clear();
            count = 0;
        }

        messages.emplace_back(iter->m_sender, iter->m_content);
        ++iter;
        ++count;
        ++total;
    }

    //  any messages left?
    //  send them now
    if (!messages.empty())
    {
        //  create and send reply
        auto reply = GetMessagesReply::create(msg->getFromID(), m_id, messages, "", ++sequenceID, false);
        m_commsLayer.push(reply);
    }

    //  transmission of data ended
    auto reply = GetMessagesReply::create(msg->getFromID(), m_id, MessagesVec{}, "", ++sequenceID, true);
    m_commsLayer.push(reply);
}

void BackEnd::GetReportHandler(BaseMessageRef msg)
{
    if (!msg) return;

    std::unique_lock<std::mutex> lock{ m_reportsMutex };

    uint32_t sequenceID = 0;

    //  no reports
    if (m_reports.empty())
    {
        //  no reports, send reply back
        auto reply = GetReportReply::create(msg->getFromID(), m_id, ReportVec{}, "cannot retrieve report: report empty", ++sequenceID, true);
        m_commsLayer.push(reply);
        return;
    }

    //  compile a printable report
    uint32_t count = 0;
    uint32_t total = 0;
    ReportVec printableReport;

    auto reportIter = m_reports.begin();
    while (reportIter != m_reports.end() && total < m_limits.MAX_TOTAL_MESSAGES_RETRIEVED)
    {
        auto report = reportIter->second;
        auto num = report.m_count;

        //  create the title of each entry
        auto title = report.m_sender + "  " + "(" + std::to_string(num) + (num > 1 ? " messages" : " message") + "):\n";
        printableReport.emplace_back(title);

        //  loop through each reports sent list
        auto sentIter = report.m_sentList.begin();
        while (sentIter != report.m_sentList.end())
        {
            //  timestamps, sort them in order
            auto recipientName = sentIter->first;
            auto& timeStampList = sentIter->second;
            std::sort(timeStampList.begin(), timeStampList.end());  //  maybe a dirty flag to avoid needless sort

            auto timeStampIter = timeStampList.begin();
            while (timeStampIter != timeStampList.end() && total < m_limits.MAX_TOTAL_MESSAGES_RETRIEVED)
            {              
                if (count >= m_limits.MAX_MESSAGES_RETRIEVED_PER_REPLY)
                {
                    //  send a batch of messages
                    auto reply = GetReportReply::create(msg->getFromID(), m_id, printableReport, "", ++sequenceID, false);
                    m_commsLayer.push(reply);

                    printableReport.clear();
                    count = 0;
                }

                //  convert time to string
                auto time = std::chrono::system_clock::to_time_t(*timeStampIter);

                //  create the recipent entry in the report
                std::string recipientStr = "    " + std::to_string(time) + ", " + recipientName + "\n";
                printableReport.emplace_back(recipientStr);

                ++timeStampIter;
                ++count;
                ++total;
            }

            ++sentIter;
        }

        ++reportIter;
    }

    if (!printableReport.empty())
    {
        //  send any remaining messages in the buffer
        auto reply = GetReportReply::create(msg->getFromID(), m_id, printableReport, "", ++sequenceID, false);
        m_commsLayer.push(reply);
    }

    //  send end of transmission message
    auto reply = GetReportReply::create(msg->getFromID(), m_id, ReportVec{}, "", ++sequenceID, true);
    m_commsLayer.push(reply);
}

}