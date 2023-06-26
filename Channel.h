//
// Created by Meng on 2023/6/11.
//

#ifndef MUDUO_1_CHANNEL_H
#define MUDUO_1_CHANNEL_H
#include"noncopyable.h"
#include"Timestamp.h"
#include <functional>
#include<memory>
class EventLoop;
//channel 理解为通道，封装了sockfd和其感兴趣的event，如EPOLLIN、EPOLLOUT事件
//还绑定了poller返回的具体事件
class Channel : noncopyable{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;
    Channel(EventLoop* loop, int fd);
    ~Channel();
    //fd得到poller通知以后，处理事件的。调用事件的
    void handlerEvent(Timestamp receiveTime); // 干啥用的
    //设置回调函数
    //这个地方 假设回调函数占用内存比较多，并且cb是个局部变量，赋值完就不再使用了
    //所以将cb占用的资源直接给了响应的回调函数。
    void setReadCallback(ReadEventCallback cb){
        readEventCallback_ = std::move(cb);
    }
    void setWriteCallback(EventCallback cb){
        writeCallback_ = std::move(cb);
    }
    void setCloseCallback(EventCallback cb){
        closeCallback_ = std::move(cb);
    }
    void setErrorCallback(EventCallback cb){
        errorCallback_ = std::move(cb);
    }
    //防止当channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd() const {return fd_;}
    int events()const {return events_;}
    void set_revents(int revt){revents_ = revt;}
    bool isNoneEvent()const{return events_ == kNoneEvent;}

    void enableReading(){events_ |= kReadEvent; update();}
    void disenableReading(){events_ &= ~kReadEvent; update();}
    void enableWriting(){events_ |= kWriteEvent; update();}
    void disenableWriting(){events_ &= ~kWriteEvent;update();}
    void disableAll(){events_ = kNoneEvent; update();}
    bool isWriting()const{return events_&kWriteEvent;}
    bool isReading()const{return events_&kReadEvent;}
    //for Poller
    int index(){return index_;}
    void set_index(int idx){index_ = idx;}

    EventLoop* ownerLoop(){return loop_;}//当前channel属于哪个loop
    void remove();
private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);
    //这三个 static const是如何使用的
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; //事件循环
    const int fd_    ; //poller监听的对象
    int events_;      //注册fd感兴趣的事件
    int revents_;     //poller返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;
    //channel里面能够获知监听的连接fd上最终发生的具体事件revents，所以以下回调函数负责调用具体时间的回调操作
    ReadEventCallback readEventCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};


#endif //MUDUO_1_CHANNEL_H
