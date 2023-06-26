//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_SOCKET_H
#define MUDUO_1_SOCKET_H

#include <cstdint>
#include"noncopyable.h"
class InetAddress;
//Socket就是将与socket有关的操作进行了一个封装，比如创建socket，绑定本地地址端口，listen，accept，设置对端地址端口
class Socket : noncopyable{
public:
    explicit Socket(int sockfd):sockfd_(sockfd){}
    ~Socket();
    int fd() const {return sockfd_;}
    //如果地址在使用就abort
    //void fd();
    void listen();
    void bindAddress(const InetAddress& localaddr);
    //如果成功，返回一个非负int
    int accept(InetAddress* peeraddr);

    void shutdownWrite();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
private:
    int sockfd_;
};


#endif //MUDUO_1_SOCKET_H
