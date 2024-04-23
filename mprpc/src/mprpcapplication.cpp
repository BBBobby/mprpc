#include"mprpcapplication.h"
#include<iostream>
#include<unistd.h>
using namespace std;

void ShowArgsHelp(){
    cout << "need format: command -i <configfile>" << endl;
}

MprpcConfig MprpcApplication::config;

void MprpcApplication::Init(int argc, char **argv){
    // 参数少于两个，错误
    if(argc < 2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    string configFile;
    while((c = getopt(argc, argv, "i:")) != -1){
        switch (c)
        {
            // 如果option = i，才是正确的
        case 'i':
            configFile = optarg;
            break;
            // option != i，错误option
        case '?':
            cout << "invalid args!" << endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
            // -i后面没有参数（即配置文件）
        case ':':
            cout << "need configfile" << endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 加载配置文件
    config.LoadConfigFile(configFile.c_str());
    // cout << "rpcServerIp = " << config.Load("rpcServerIp") << endl;
    // cout << "rpcServerPort = " << config.Load("rpcServerPort") << endl;
    // cout << "zookeeperIp = " << config.Load("zookeeperIp") << endl;
    // cout << "zookeeperPort = " << config.Load("zookeeperPort") << endl;
}

MprpcApplication& MprpcApplication::GetInstance(){
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::getConfig(){
    return config;
}