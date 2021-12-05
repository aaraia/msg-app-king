#ifndef H_MESSAGEIDS_H
#define H_MESSAGEIDS_H


namespace MessageService {

enum class MessageID 
{ 
    AddUserMessageID,
    SendMessageID,
    GetMessagesID,
    GetReportID,

    AddUserReplyMessageID,
    SendMessageReplyID,
    GetMessagesReplyID,
    GetReportReplyID
};

}

#endif // !H_MESSAGEIDS_H
