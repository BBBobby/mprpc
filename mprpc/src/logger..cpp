#include"logger.h"
#include<iostream>

Logger& Logger::GetInstance(){
    static Logger logger;
    return logger;
}

Logger::Logger(){
    // 启动写日志线程;
    thread t([&](){
        for(;;){
            // 获取当天的日期，然后取日志信息，写入相应的文件中（"a+"）
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);
            char fileName[128];
            // 将格式化字符串写入fileName
            sprintf(fileName, "../log/%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);
            FILE* pf = fopen(fileName, "a+");
            if(pf == nullptr){
                cout << "logger file open error: " << fileName << endl;
                exit(EXIT_FAILURE); 
            }
            string msg = lque.Pop();
            char timeBuf[128] = {0};
            sprintf(timeBuf, "%d:%d:%d=> ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
            msg.insert(0, timeBuf);
            fputs(msg.c_str(), pf);
            fclose(pf);
        }   
    });
    t.detach();

}


void Logger::Log(string msg, string logInfo){
    lque.Push(logInfo + msg + "\n");

}