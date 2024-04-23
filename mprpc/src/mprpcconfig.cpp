#include"mprpcconfig.h"
#include<iostream>
using namespace std;
// 负责解析配置文件
void MprpcConfig::LoadConfigFile(const char* configFile){
    FILE *pf = fopen(configFile, "r");
    if(nullptr == pf){
        cout << "config file is not exist!" << endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(pf)){
        char buf[512] = {};
        // 按行读
        fgets(buf, 512, pf);
        string line(buf);
        // 去掉这行前后多余的空格
        Trim(line);
        // 判断空行或注释
        if(line[0] == '#' || line.empty()){
            continue;
        }
        // 解析配置项
        int idx = line.find('=');
        if(idx == -1){
            // 配置项不合法
            continue;
        }
        // 保存配置项
        string key, value;
        key = line.substr(0, idx);
        Trim(key);
        // 去掉换行符
        int endIdx = line.find_last_of('\n'); 
        value = line.substr(idx+1, endIdx-idx-1);
        Trim(value);

        configMap.insert({key, value});
    }

}
// 查询配置项信息
string MprpcConfig::Load(const string& key){
    if(configMap.count(key)){
        return configMap[key];
    }
    return "";

}

// 去掉字符串两边的空格
void MprpcConfig::Trim(string& line){
    int idx = line.find_first_not_of(' ');
    if(idx != -1){
        line = line.substr(idx, line.size()-idx);
    }
    // 去掉后面多余空格
    idx = line.find_last_not_of(' ');
    if(idx != -1){
        line = line.substr(0, idx+1);
    }

}