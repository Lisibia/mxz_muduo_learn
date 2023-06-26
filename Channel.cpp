//
// Created by Meng on 2023/6/11.
//

#include "Channel.h"
#include<sys/epoll.h>
#include "logger.h"
#include "EventLoop.h"
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN|EPOLLPRI;//EPOLLPRI这个是啥
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd):loop_(loop),
                                          fd_(fd),
                                          events_(0),
                                          revents_(0),
                                          index_(-1), //为啥是-1
                                          tied_(false){}
Channel::~Channel() {
    //缺东西
}
//一个TcpConnection新连接创建的时候  TcpConnection->channel
void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}
void Channel::update(){
    //当改变channel所表示fd的events事件后，update负责在poller里面更改fd相应的事件
    //要用到eventloop.
    loop_->updateChannel(this);
}

void Channel::remove() {
    //将channel 从eventloop中删除
    loop_->removeChannel(this);
}

void Channel::handlerEvent(Timestamp receiveTime) {
    //先判断弱智能指针指向的内存还是否存在，如果存在那么就处理事件，如果内存不存在就不处理事件
    //如何判断弱智能指针指向的内存是否还存在？ 将其转换成强智能指针，看强智能指针是否为空
    std::shared_ptr<void> guard;
    if(tied_){
        guard = tie_.lock(); //如果内存还存在
        if(guard){
            handleEventWithGuard(receiveTime);
        }
    }
    else{
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
    LOG_INFO("channel handleEvent revents:%d\n", revents_);
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        //如果通信一方关闭了连接或者不是可读事件
        if(closeCallback_){closeCallback_();}
    }
    if(revents_ & (EPOLLERR)){
        if(errorCallback_) errorCallback_();
    }
    if(revents_ & (EPOLLIN|EPOLLPRI)){
        if(readEventCallback_) readEventCallback_(receiveTime);
    }
    if(revents_ & EPOLLOUT){
        if(writeCallback_) writeCallback_();
    }
}