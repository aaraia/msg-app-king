#include <iostream>


//  app
#include "MessageApp.h"
#include "Logger.h"

int main(int, const char* [])
{
    try {
        MessageService::MessageApp messageApp;
        messageApp.run();
    } 
    catch (std::exception& e)
    {
        Utilities::Logger::get().log("Exception: " + std::string(e.what()));
    }

    return 0;
}
