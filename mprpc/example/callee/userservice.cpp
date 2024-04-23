#include<iostream>
#include"user.pb.h"
#include"mprpcapplication.h"
#include"rpcprovider.h"
#include"logger.h"
using namespace std;

/*
UserService原来是一个本地服务，提供两个本地方法：Login, GetFriendLists
*/
class UserService: public fixbug::UserServiceRpc{
public:
    bool Login(string name, string pwd){
        cout << "doing local service: Login" << endl;
        cout << "name: " << name << ",pwd: " << pwd << endl;
        return true;
    }
    /*
        重写基类UserServiceRpc的虚函数 下面的方法是框架直接调用的
    */
    void Login(::google::protobuf::RpcController* controller,
                    const ::fixbug::LoginRequest* request,
                    ::fixbug::LoginResponse* response,
                    ::google::protobuf::Closure* done)
    {   
        // 框架给业务上报请求参数，应用获取相应数据做本地业务
        string name = request->name();
        string pwd = request->pwd();
        // 做本地业务
        bool loginResult = Login(name, pwd);
        // 根据业务结果写入响应response
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(loginResult);
        // 执行响应函数, 执行响应数据的序列化和网络发送（由框架完成）
        done->Run();
    }

private:

};

int main(int argc, char **argv){
    LOG_INFO("TEST LOG: %s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    
    // provider是一个rpc网络服务对象，把UserService发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());
    // 启动一个rpc服务发布节点，run后阻塞，等待远程rpc调用
    provider.Run();
    
    return 0;
}