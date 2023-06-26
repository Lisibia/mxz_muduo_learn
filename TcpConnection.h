//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_TCPCONNECTION_H
#define MUDUO_1_TCPCONNECTION_H

#include "noncopyable.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Buffer.h"
#include<memory>
#include <string>
#include<atomic>
/*
 *TcpServer->Acceptor->有一个新用户连接，通过accept函数拿到connfd
 * ->TcpConnection设置回调->Channel->poller->Channel的回调操作
 */
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>{
public:
    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();
    EventLoop* getLoop() const{return loop_;}
    const std::string& name() const {return name_;}
    const InetAddress& localAddress()const{return localAddr_;}
    const InetAddress& peerAddress() const{return peerAddr_; }

    bool connected() const {return state_ == kConnected;}
    bool disconnected() const {return state_ == kDisconnected;}
    //发送数据
//    void send(const void* message, int len);
    void send(const std::string &buf);
    void sendInLoop(const void* message, size_t len);
    //关闭连接
    void shutdown();

    void setConnectionCallback(const ConnectionCallback& cb){ connectionCallback_ = cb;}
    void setMessageCallback(const MessageCallback& cb){messageCallback_ = cb;}
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompleteCallback_ = cb;}
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb){ highWaterMarkCallback_ = cb;}
    void setCloseCallback(const CloseCallback& cb){ closeCallback_ = cb;}

    Buffer* inputBuffer(){return &inputBuffer_;}
    Buffer* outputBuffer(){return &outputBuffer_;}

    void connectEstablished(); //建立连接
    void connectDestroyed();   //关闭连接

private:
    enum StateE{kDisconnected, kConnecting, kConnected, kDisconnecting};
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void senInLoop(const void* message, size_t len);
    void shutdownInLoop();

    void forceCloseInLoop();
    void setState(StateE s){state_ = s;}
    const char* stateToString() const;
    void startReadInLoop();
    void stopReadInLoop();

    EventLoop* loop_;      //这里不是baseLoop，因为tcpConnection都是在subLoop中建立的。
    const std::string name_;
    std::atomic<StateE> state_;
    bool reading_;
    //这里和acceptor类似 Acceptor->mainLoop  TcpConnection->subLoop
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    ConnectionCallback connectionCallback_; //有新连接来时的回调
    MessageCallback    messageCallback_;    //有读写事件的回调
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t highWaterMark_;


    Buffer inputBuffer_;
    Buffer outputBuffer_;
};


#endif //MUDUO_1_TCPCONNECTION_H
