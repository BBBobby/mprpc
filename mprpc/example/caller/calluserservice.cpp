#include<iostream>
#include"user.pb.h"
#include"mprpcapplication.h"

int main(int argc, char** argv){
    // 程序启动后，先初始化框架
    MprpcApplication::Init(argc, argv);
    // rpc远程调用的参数,和接受response
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    // 调用远程发布的rpc方法
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    MprpcController controller;
    // 本质上调用的是RpcChannel->callMethod, 集中来做所有rpc方法参数的序列化和网络发送
    stub.Login(&controller, &request, &response, nullptr);

    // 调用完成，读结果
    if(controller.Failed()){
        cout << controller.ErrorText() << endl;
    }
    else{
        if(0 == response.result().errcode()){
            cout << "rpc login response success: " << response.success() << endl;
        }
        else{
            cout << "rpc login response error: " << response.result().errmsg() << endl;
        }
        return 0;
    }

}