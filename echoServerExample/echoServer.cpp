//
// Created by Meng on 2023/6/26.
//
#include<muduo_test/TcpServer.h>
#include<muduo_test/TcpConnection.h>
#include<string>
#include<functional>
#include<iostream>
class EchoServer{
public:
    EchoServer(EventLoop* loop, const InetAddress&addr, std::string name):
            server_(loop,addr, name),
            loop_(loop) {
        //注册回调函数
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2,
                                             std::placeholders::_3));

        //设置合适的loop线程
        server_.setThreadNum(3);

    }
    void start(){
        server_.staDrt();
    }
private:
    //连接建立或者断开的回调
    void onConnection(const TcpConnectionPtr &conn){
        if(conn->connected()){
            std::cout<<"Connection UP:"<<conn->peerAddress().toIpPort()<<std::endl;
        }
        else{
            std::cout<<"Connection DOWN:"<<conn->peerAddress().toIpPort()<<std::endl;
        }
    }

    //可读写事件回调
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time){
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown(); //关闭写端， 就会执行EPOLLHUP-》closeCallback
    }
    EventLoop *loop_;
    TcpServer server_;
};
int main(){
    EventLoop loop;
    InetAddress addr(8000);
    std::string name = "echoServer";
    EchoServer echoServer(&loop,addr,name);
    echoServer.start();
    loop.loop();  //启动mainLoop的底层Poller
    return 0;
}