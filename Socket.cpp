//
// Created by Meng on 2023/6/18.
//

#include <unistd.h>
#include <sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cstring>
#include "Socket.h"
#include "logger.h"
#include"InetAddress.h"
Socket::~Socket() {
    ::close(sockfd_);
}
void Socket::bindAddress(const InetAddress& localaddr){
    int ret = ::bind(sockfd_,(sockaddr*)localaddr.getSockAddr(),sizeof *localaddr.getSockAddr());
    if(ret != 0){
        LOG_FATAL("Socket::bindAddress:%d.\n", sockfd_);
    }
}
void Socket::listen(){
    int ret = ::listen(sockfd_,1024);
    if(ret < 0){
        LOG_FATAL("sockets_listen failed\n");
    }
}

int Socket::accept(InetAddress* peeraddr){
    sockaddr_in addr;
    socklen_t len;
    memset(&addr,0, sizeof addr);
    int connfd = ::accept(sockfd_, (sockaddr*)&addr, &len);
    if(connfd >= 0){
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}
void Socket::shutdownWrite(){
    int ret = ::shutdown(sockfd_,SHUT_WR);
    if(ret < 0){
        LOG_ERROR("shutdown failed.\n");
    }
}
void Socket::setTcpNoDelay(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_,IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}
void Socket::setReuseAddr(bool on){
    int optval = on ? 1:0;
    ::setsockopt(sockfd_, SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
}
void Socket::setReusePort(bool on){
    int optval = on ? 1:0;
    ::setsockopt(sockfd_, SOL_SOCKET,SO_REUSEPORT,&optval,sizeof optval);
}
void Socket::setKeepAlive(bool on){
    int optval = on ? 1:0;
    ::setsockopt(sockfd_, SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof optval);
}