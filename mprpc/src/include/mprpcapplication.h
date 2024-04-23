#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H
#include"mprpcconfig.h"
#include"mprpcchannel.h"
#include"mprpccontroller.h"
#include<string>
using namespace std;

// 框架的初始化类
class MprpcApplication{
public:
    static void Init(int argc, char **argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig& getConfig();
private:
    static MprpcConfig config;
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};


#endif