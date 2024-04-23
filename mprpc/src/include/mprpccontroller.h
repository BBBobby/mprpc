#ifndef MPRPCCONTROLLER_H
#define MPRPCCONTROLLER_H

#include<iostream>
#include<google/protobuf/service.h>
using namespace std;

class MprpcController: public google::protobuf::RpcController{
public:
    MprpcController();
    // 重写
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);
    // 用不到
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    bool isFailed;
    string errTest;
};
#endif