//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_ACCEPTOR_H
#define MUDUO_1_ACCEPTOR_H

#include"noncopyable.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#include<functional>
class EventLoop;
class InetAddress;
class Acceptor {
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();
    void setNewConnectionCallback(const NewConnectionCallback& cb){
        newConnectionCallback_ = cb;
    }
    void listen();

    bool listening() const{return listening_;}

private:
    void handleRead();
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
    int idleFd_;
};


#endif //MUDUO_1_ACCEPTOR_H
