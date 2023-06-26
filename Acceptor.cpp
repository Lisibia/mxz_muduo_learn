//
// Created by Meng on 2023/6/18.
//

#include <unistd.h>
#include "Acceptor.h"
#include "logger.h"

static int createNonbloking(){
    int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
    if(sockfd < 0){
        LOG_FATAL("%s:%s:%d listen socket createNonblocking failed:%ds.\n",__FILE__, __FUNCTION__,__LINE__,errno);
    }
    return sockfd;
}
Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
:loop_(loop), acceptSocket_(createNonbloking()), acceptChannel_(loop,acceptSocket_.fd()),
 listening_(false),idleFd_()
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);
    //TcpServer::start() Acceptor.listen 有新用户连接，要执行一个回调connfd->channel->subloop
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}


Acceptor::~Acceptor(){
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen(){
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading(); //acceptChannel_->Poller.
}

void Acceptor::handleRead(){
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >= 0){
        if(newConnectionCallback_){
            newConnectionCallback_(connfd,peerAddr); //轮询找到subloop唤醒，分发当前的新客户端的Channel
        }
        else{
            ::close(connfd);
        }
    }
    else{
        LOG_ERROR("%s:%s:%d accept err:%d.\n",__FILE__, __FUNCTION__,__LINE__,errno);
        if(errno == EMFILE){ //文件描述符已经打开到上线
            LOG_ERROR("%s:%s:%d accept reached limit :%d.\n",__FILE__, __FUNCTION__,__LINE__,errno);
        }

    }
}