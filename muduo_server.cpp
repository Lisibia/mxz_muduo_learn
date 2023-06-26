////
//// Created by Meng on 2023/6/8.
////
///*
// *muduo网络库给用户提供了两个主要的类
// * TcpServer：用于编写服务器程序
// * TcpClient：用于编写客户端程序
// *
// * epoll + 线程池
// * 好处：能够把网络IO的代码和业务代码区分开
// *                        业务代码主要包括 用户的连接和断开   用户的可读写事件
// */
//
//#include <muduo/net/TcpServer.h>
//#include <iostream>
//#include <muduo/net/EventLoop.h>
//using namespace std;
//using namespace muduo;
//using namespace muduo::net;
//using namespace placeholders;
//
///*
// * 1.组合Tcpserver对象
// * 2.创建EventLoop事件循环对象的指针
// * 3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
// * 4.在当前服务器类的构造函数中，注册处理连接的回调函数和处理写事件的回调函数
// * 5、设置合适的服务线程数量，muduo库会自己分配IO线程和工作线程
// */
//class ChatServer{
//public:
//    ChatServer(EventLoop* loop,
//               const InetAddress& listenAddr,
//               const string& nameArg): _server(loop,listenAddr,nameArg),_loop(loop){
//        //给服务器注册用户连接的创建和断开回调
//        _server.setConnectionCallback( std::bind(&ChatServer::onConnection, this, _1));
//        //给服务器注册用户写事件回调
//        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
//    }
//    void start(){
//        _server.start();
//    }
//
//private:
//    void onConnection(const TcpConnectionPtr &conn){
//        if(conn->connected()){
//            cout<<conn->peerAddress().toIpPort() <<"->"<<conn->localAddress().toIpPort()<<endl;
//        }
//        else{
//            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<endl;
//        }
//
//    }
//    void onMessage(const TcpConnectionPtr & conn, //连接
//                   Buffer *buffer,                //缓冲区
//                   Timestamp time                 //接收到数据的时间信息
//                   ){
//        string buf = buffer->retrieveAllAsString();
//        cout<<"recv data:"<< buf <<" time:"<<time.toString()<<endl;
//        conn->send(buf);
//    }
//    TcpServer _server;//#1
//    EventLoop *_loop; //#2
//};
//
//
//
//int main() {
//    EventLoop loop; //epoll
//    InetAddress addr("127.0.0.1",6000);
//    ChatServer server(&loop, addr,"ChatServer");
//    server.start();
//    loop.loop();
//}