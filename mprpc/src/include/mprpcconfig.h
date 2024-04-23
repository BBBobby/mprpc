#ifndef MPRPCCONFIG_H
#define MPRPCCONFIG_H

#include<unordered_map>
#include<string>

using namespace std;

class MprpcConfig{
public:
    // 负责解析配置文件
    void LoadConfigFile(const char* configFile);
    // 查询配置项信息
    string Load(const string& key);
private:
    unordered_map<string, string> configMap;
    void Trim(string& line);
};

#endif