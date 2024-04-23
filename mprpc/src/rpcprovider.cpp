#include"rpcprovider.h"
#include"mprpcapplication.h"
#include"zookeeperutil.h"

using namespace std;
    // 发布rpc方法
void RpcProvider::NotifyService(google::protobuf::Service* service){
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    string serviceName = pserviceDesc->name();
    cout << serviceName << " ";
    ServiceInfo serviceInfo;
    // 获取服务对象的方法的数量
    int methodCnt = pserviceDesc->method_count();
    for(int i = 0; i < methodCnt; ++i){
        // 获取服务里所有方法的描述
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        serviceInfo.methodMap.insert({pmethodDesc->name(), pmethodDesc});
        cout << pmethodDesc->name() << endl;
    }
    serviceInfo.service = service;
    serviceMap.insert({serviceName, serviceInfo});
}
    // 启动rpc服务节点
void RpcProvider::Run(){
    
    string ip = MprpcApplication::GetInstance().getConfig().Load("rpcServerIp");
    uint16_t port = atoi(MprpcApplication::GetInstance().getConfig().Load("rpcServerPort").c_str());
    InetAddress address(ip, port);

    // 创建TCPserver对象
    TcpServer server(&eventloop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调方法
    server.setConnectionCallback(bind(&RpcProvider::onConnection, this, placeholders::_1));
    server.setMessageCallback(bind(&RpcProvider::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));
    // 设置线程数量 
    server.setThreadNum(2);
    // 将服务注册到zookeeper
    ZkClient zkCli;
    zkCli.Start();
    for(auto& sp: serviceMap){
        string servicePath = "/" + sp.first;
        zkCli.Create(servicePath.c_str(), nullptr, 0);
        for(auto& mp: sp.second.methodMap){
            // /UserServiceRpc/Login
            string methodPath = servicePath + "/" + mp.first;
            char data[128] = {0};
            sprintf(data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERL表示临时节点
            zkCli.Create(methodPath.c_str(), data, strlen(data), ZOO_EPHEMERAL);
        }
    }
    // 启动网络服务
    cout << "RpcProvider service start at ip: " << ip << " port: " << port << endl;
    server.start();
    eventloop.loop();
     
}

void RpcProvider::onConnection(const TcpConnectionPtr& conn){
    if(!conn->connected()){
        conn->shutdown();
    }
}

/*
在框架内部，provider和consumer协商好之间通信的protobuf数据类型 
需要定义proto的message类型，进行数据头的序列化反序列化:
headerSize(4B) + header + argS;  header = serviceName + methodName + argsSize
*/
void RpcProvider::onMessage(const TcpConnectionPtr& conn, Buffer* buff, muduo::Timestamp timestamp){
    // 网络接收到的远程rpc调用请求的字符流
    string recvBuff = buff->retrieveAllAsString();
    // 从字符流中读取前4个字节的内容
    uint32_t headerSize = 0;
    recvBuff.copy((char*)&headerSize, 4, 0);
    // 根据headerSize读取数据头的原始字符流
    string recvHeader = recvBuff.substr(4, headerSize);
    // 从字符流反序列化出：服务名字，方法名字，参数长度
    mprpc::RpcHeader rpcHeader;
    string serviceName;
    string methodName;
    uint32_t argsSize;
    if(rpcHeader.ParseFromString(recvHeader)){
        serviceName = rpcHeader.servicename();
        methodName = rpcHeader.methodname();
        argsSize = rpcHeader.argssize();
    }
    else{
        cout << "rpcHeader parse error" << endl;
        exit(EXIT_FAILURE);
    }
    // 根据argsSize获取参数的原始字符流
    string recvArgs = recvBuff.substr(4 + headerSize, argsSize);
    cout << "=================================" << endl;
    cout << "headerSize: " << headerSize << endl;
    cout << "recvHeader: " << recvHeader << endl;
    cout << "serviceName: " << serviceName << endl;
    cout << "methodName: " << methodName << endl;
    cout << "recvArgs: " << recvArgs << endl;
    cout << "=================================" << endl;

    // 获取service对象和method对象
    auto it = serviceMap.find(serviceName);
    if(it == serviceMap.end()){
        cout << serviceName << " is not exit" << endl;
        return;
    } 
    auto mit = it->second.methodMap.find(methodName);
    if(mit == it->second.methodMap.end()){
        cout << serviceName << ": "<< methodName << " is not exit" << endl;
        return; 
    }
    google::protobuf::Service *service = it->second.service;
    const google::protobuf::MethodDescriptor* method = mit->second;

    // 生成rpc方法调用的request和response参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(recvArgs)){
        cout << "request parse error" << endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // 给下面的callMethod最有一个参数
    google::protobuf::Closure* done =  google::protobuf::NewCallback<RpcProvider, const TcpConnectionPtr&, google::protobuf::Message*>
    (this, &RpcProvider::SendRpcResponse, conn, response);
    // 在框架上根据远程rpc请求，调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}

// closure的回调，用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const TcpConnectionPtr& conn, google::protobuf::Message* response){
    string responseStr;
    // 序列化
    if(response->SerializeToString(&responseStr)){
        // 将响应结果发送回去
        conn->send(responseStr);
    }
    else{
        cout << "serialize response error" << endl;
    }
    conn->shutdown(); //和http短连接一样，主动断开连接

}