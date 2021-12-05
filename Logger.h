#ifndef H_LOGGER_H
#define H_LOGGER_H


//  stl
#include <iostream>
#include <mutex>

namespace Utilities {

//  used to help debugging
class Logger
{
    //  no need for move/copy
    Logger& operator=(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger(const Logger&) = delete;

    Logger() = default;  //  declare private so cant be created

public:
    static Logger& get()
    {
        //  create local static instance
        //  thread safe in standard C++11 (magic statics)
        static Logger logger;
        return logger;
    }

    ~Logger() = default;

    void log(const std::string& msg)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        std::cout << msg;
    }
    
private:
    std::mutex m_mutex;
};

}


#endif
