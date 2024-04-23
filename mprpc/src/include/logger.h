#ifndef LOGGER_H
#define LOGGER_H
#include"lockqueue.h"


// 
class Logger{
public:
    static Logger& GetInstance();
    void Log(string msg, string logInfo);
private:
    LockQueue<string> lque; //  缓冲队列

    // 单例模式
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};

// 定义日志宏
#define LOG_INFO(logmsgformat, ...)\
do{ \
    Logger &logger = Logger::GetInstance(); \
    char c[1024] = {0}; \
    snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
    logger.Log(c, "[Info]: "); \
}while(0);

#define LOG_ERROR(logmsgformat, ...)\
do{ \
    Logger &logger = Logger::GetInstance(); \
    char c[1024] = {0}; \
    snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
    logger.Log(c, "[Error]: "); \
}while(0);

#endif