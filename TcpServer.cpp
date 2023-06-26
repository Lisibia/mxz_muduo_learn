//
// Created by Meng on 2023/6/11.
//

#include <cstring>
#include "TcpServer.h"
#include "logger.h"
#include"TcpConnection.h"
#include<iostream>
static EventLoop* CheckLoopNotNull(EventLoop* loop){
    if(loop == nullptr){
        LOG_FATAL("%s:%s:%d mainLoop is null! \n", __FILE__,__FUNCTION__,__LINE__);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop* loop,
const InetAddress &listenaddr,
const std::string& nameArg,
        Option option)
        :loop_(CheckLoopNotNull(loop)),
        ipPort_(listenaddr.toIpPort()),
        name_(nameArg),
        acceptor_(new Acceptor(loop_,listenaddr, option == kReusePort)),
        threadPool_(new EventLoopThreadPool(loop, name_)),
        connectionCallback_(),
        messageCallback_(),
        nextConnId_(1),
        started_(0)
        {
    //当有新用户连接时，会执行TcpServer::newConnection回调
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this,std::placeholders::_1,std::placeholders::_2));
}
//有新客户端用户连接时，acceptor会执行这个回调操作
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr){

    EventLoop* ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf,sizeof buf,"-%s#%d",ipPort_.c_str(),nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    LOG_INFO("TcpServer::newConnection[%s]-new connection [%s] from %s", name_.c_str(), connName.c_str(),ipPort_.c_str());
    struct sockaddr_in localAddr;
    memset(&localAddr,0,sizeof localAddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localAddr);

    if(::getsockname(sockfd, reinterpret_cast<sockaddr *>(&localAddr), &addrlen) < 0){
        LOG_ERROR("sockets::getLocalAddr.\n");
    }
    InetAddress localAddress(localAddr);
    //根据四元组来创建连接
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddress, peerAddr));
    connections_[connName] = conn;
    //下面回调都是用户设置给TcpServer->TcpConnection->Channel->Poll->Poller->notify channel调用回调
    conn->setMessageCallback(messageCallback_);
    conn->setConnectionCallback(connectionCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    //设置了如何关闭连接的回调
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));
    //直接调用TcpConnection::connectEstablished
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));
}

void TcpServer::setThreadNum(int numThreads) { //设置subloop的个数
    threadPool_->setThreadNum(numThreads);
}
void TcpServer::start() {//开启服务器监听
    if(started_++ == 0){
        threadPool_->start(threadInitCallback_);
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn){
    LOG_INFO("TcpServer::RemoveConnectionInLoop[%s]-connection %s", name_.c_str(), conn->name().c_str());
    size_t n = connections_.erase(conn->name());
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
}
TcpServer::~TcpServer(){
    LOG_INFO("TcpServer::~TcpServer[%s] destructing\n" ,name_.c_str());
    for(auto& item : connections_){
        TcpConnectionPtr conn(item.second); //新建立的conn智能指针出右括号可以自动释放new出来的TcpConnection资源了。
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

