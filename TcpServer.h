//
// Created by Meng on 2023/6/11.
//

#ifndef MUDUO_1_TCPSERVER_H
#define MUDUO_1_TCPSERVER_H


/*
 *
 */
#include"EventLoop.h"
#include"Acceptor.h"
#include"InetAddress.h"
#include"noncopyable.h"
#include "Callbacks.h"
#include "EventLoopThreadPool.h"

#include<unordered_map>
#include<functional>

class TcpServer : noncopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    enum Option{
        kNoReusePort,
        kReusePort,
    };
    TcpServer(EventLoop* loop,
              const InetAddress &listenaddr,
              const std::string& nameArg,
              Option option = kNoReusePort);
    ~TcpServer();
    void setThreadNum(int numThreads); //设置subloop的个数

    std::shared_ptr<EventLoopThreadPool> threadPool(){
        return threadPool_;
    }
    void start();//开启服务器监听

    void setThreadInitCallback(const ThreadInitCallback& cb){threadInitCallback_ = cb;}
    void setConnectionCallback(const ConnectionCallback& cb){connectionCallback_= cb;}
    void setMessageCallback(const MessageCallback& cb){messageCallback_ = cb;}
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){writeCompleteCallback_ = cb;}


private:

    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    using ConnectionMap = std::unordered_map<std::string,TcpConnectionPtr>;
    EventLoop* loop_; //用户定义的loop;

    const std::string ipPort_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;   //运行在mainloop，用于监听新连接事件
    std::shared_ptr<EventLoopThreadPool> threadPool_; //one loop per thread 一个线程一个事件循环

    ConnectionCallback connectionCallback_;  //有新连接来的时候的回调
    MessageCallback    messageCallback_;     //有读写消息时的回调
    WriteCompleteCallback writeCompleteCallback_;//写完消息的一个回调
    ThreadInitCallback threadInitCallback_;  //线程初始化回调
    std::atomic<int> started_;

    int nextConnId_;
    ConnectionMap connections_;
};


#endif //MUDUO_1_TCPSERVER_H
