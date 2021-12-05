#ifndef H_MESSAGEAPP_H
#define H_MESSAGEAPP_H



//  app
#include "BackEnd.h"
#include "FrontEnd.h"
#include "CommsLayer.h"

namespace MessageService {

class MessageApp
{
    //  no move/copy for now
    MessageApp& operator=(MessageApp&&) = delete;
    MessageApp& operator=(const MessageApp&) = delete;
    MessageApp(MessageApp&&) = delete;
    MessageApp(const MessageApp&) = delete;

public:
    MessageApp();
    ~MessageApp();

    void run();

private:
    CommsLayer m_commsLayer;  //  order is important, comms layer must be created first
    BackEnd m_backEnd;
    FrontEnd m_frontEnd;
};

}

#endif
