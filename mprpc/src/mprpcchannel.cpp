#include"mprpcchannel.h"
#include"mprpcapplication.h"
#include"zookeeperutil.h"
#include"rpcheader.pb.h"
#include<string>
#include<sys/types.h>         
#include<sys/socket.h>
#include<err.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>

using namespace std;
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                        google::protobuf::RpcController* controller, 
                        const google::protobuf::Message* request,
                        google:: protobuf::Message* response, 
                        google::protobuf::Closure* done)
{
    // 获取服务名和方法名
    const google::protobuf::ServiceDescriptor* sd = method->service();
    string serviceName = sd->name();
    string methodName = method->name();

    // 获取参数的序列化字符串长度argsSize
    int argsSize = 0;
    string argsStr;
    if(request->SerializeToString(&argsStr)){
        argsSize = argsStr.size();
    }
    else{
        controller->SetFailed("Serialize request error");
        return;
    }

    // headerSize(4B) + header + argS;  header = serviceName + methodName + argsSize
    // 定义请求的rpcHeader
    mprpc::RpcHeader header;
    header.set_servicename(serviceName);
    header.set_methodname(methodName);
    header.set_argssize(argsSize);
    // 序列化header
    string headerStr;
    uint32_t headerSize = 0;
    if(header.SerializeToString(&headerStr)){
        headerSize = headerStr.size();
    }
    else{
        controller->SetFailed("Serialize header error");
        return;
    }
    // 组织待发送的rpc请求的字符串
    // headerSize(4B) + header + argS;  header = serviceName + methodName + argsSize
    string sendRpc;
    sendRpc.insert(0, string((char*)&headerSize, 4));
    sendRpc += headerStr;
    sendRpc += argsStr;

    cout << "=================================" << endl;
    cout << "headerSize: " << headerSize << endl;
    cout << "headerStr: " << headerStr << endl;
    cout << "serviceName: " << serviceName << endl;
    cout << "methodName: " << methodName << endl;
    cout << "argsStr: " << argsStr << endl;
    cout << "=================================" << endl;

    // TCP编程，完成rpc方法的远程调用
    /*
    AF_INET 参数指定了套接字的地址族，表示使用 IPv4 地址。
    SOCK_STREAM 参数指定了套接字的类型，表示创建一个流式套接字，用于面向连接的 TCP 通信。
    0 参数是一个协议标志，用于选择默认协议，对于 TCP 通信来说，通常设置为 0。
    */
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == clientFd){
        string txt = "create socket error, errno: ";
        txt += to_string(errno);
        controller->SetFailed(txt);
        close(clientFd);
        return;
    }

    // 读本地配置文件找rpc服务方的ip/port
    // string ip = MprpcApplication::GetInstance().getConfig().Load("rpcServerIp");
    // uint16_t port = atoi(MprpcApplication::GetInstance().getConfig().Load("rpcServerPort").c_str());
    // 通过zookeeper查找rpc服务方的ip/port
    ZkClient zkCli;
    zkCli.Start();
    string methodPath = "/" + serviceName + "/" + methodName;
    string hostPath = zkCli.GetData(methodPath.c_str());
    if(hostPath == ""){
        controller->SetFailed(methodPath + " is not exit");
        return;
    }
    int idx = hostPath.find(":");
    if(idx == -1){
        controller->SetFailed(methodPath + " address is invalid");
        return;
    }
    string ip = hostPath.substr(0, idx);
    uint16_t port = atoi(hostPath.substr(idx+1, hostPath.size()-idx).c_str());
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    // 建立TCP连接
    if(-1 == connect(clientFd, (sockaddr*)&serverAddr, sizeof(serverAddr))){
        string txt = "TCP connect error, errno: ";
        txt += to_string(errno);
        controller->SetFailed(txt);
        close(clientFd);
        return;
    }
    // 发送rpc请求
    if(-1 == send(clientFd, sendRpc.c_str(), sendRpc.size(), 0)){
        string txt = "send error, errno: ";
        txt += to_string(errno);
        controller->SetFailed(txt);
        close(clientFd);
        return; 
    }
    // 接收响应
    char recvBuff[1024] = {0};
    int recvSize;
    if(-1 == (recvSize = recv(clientFd, recvBuff, 1024, 0))){
        string txt = "recv error, errno: ";
        txt += to_string(errno);
        controller->SetFailed(txt);
        close(clientFd);
        return;
    }

    if(!response->ParseFromArray(recvBuff, recvSize)){
        string txt = "parse response error";
        txt += to_string(errno);
        controller->SetFailed(txt);
    }
    close(clientFd);
    return;
}