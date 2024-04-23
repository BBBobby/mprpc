#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "rpcheader.pb.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include<string>
#include<unordered_map>
#include<functional>
using namespace std;
using namespace muduo::net;
// 框架提供的发布rpc服务的发布类
class RpcProvider{
public:
    // 发布rpc方法
    void NotifyService(google::protobuf::Service* service);
    // 启动rpc服务节点
    void Run();
private:
    // 服务的信息
    struct ServiceInfo{
        google::protobuf::Service* service; // 服务名称
        unordered_map<string, const google::protobuf::MethodDescriptor*> methodMap;   // 方法名，方法的描述
    };
    // 存储注册的服务的信息
    unordered_map<string, ServiceInfo> serviceMap;  // 服务名，服务的信息
    EventLoop eventloop;
    // 新的socket连接回调
    void onConnection(const TcpConnectionPtr& conn);
    // 读写回调
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, muduo::Timestamp timestamp);
    // closure的回调，用于序列化rpc的响应和网络发送
    void SendRpcResponse(const TcpConnectionPtr& conn, google::protobuf::Message* response);
};
#endif