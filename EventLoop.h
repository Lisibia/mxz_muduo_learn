//
// Created by Meng on 2023/6/11.
//

#ifndef MUDUO_1_EVENTLOOP_H
#define MUDUO_1_EVENTLOOP_H
#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"
#include<functional>
#include<vector>
#include<mutex>
#include<atomic>
#include<memory>

class Channel;
class Poller;
//事件循环类 主要包含了两个大模块 channel poller（epoll）
class EventLoop : noncopyable {
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();
    //开启事件循环
    void loop();
    //退出事件循环
    void quit();

    Timestamp pollReturnTime() const{return pollReturnTime_;}
    //在当前loop中执行cb
    void runInLoop(Functor cb);
    //把cb放入到队列中，唤醒loop所在的线程，执行cb
    void queueInLoop(Functor cb);
    //用来唤醒loop所在的线程
    void wakeup();
    //EventLoop的方法-》Poller的方法。
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);
    //判断EventLoop对象是否在自己的线程里面
    bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}
private:
    void handleRead(); //waked up
    void doPendingFunctors();//执行回调操作

    using ChannelList = std::vector<Channel*>;
    std::atomic_bool looping_;//原子操作，通过CAS实现的
    std::atomic_bool quit_; //退出loop
    const pid_t threadId_;   //记录当前loop所在线程的id
    Timestamp pollReturnTime_;//poller返回发生事件的channels的时间点
    std::shared_ptr<Poller> poller_;

    int wakeupFd_;   //mainloop获取一个新用户的channel,通过轮询算法选择一个subloop,通过该成员唤醒subloop处理channel.
    std::unique_ptr<Channel> wakeupChannel_; //什么东西

    ChannelList activeChannels_;             //什么东西
    Channel* currentActiveChannel_;          //用于断言
    std::atomic_bool callingPendingFunctors_; //标识当前loop是否有需要执行的回调操作
    std::vector<Functor> pendingFunctors_;    //存储loop所需要的所有回调操作
    std::mutex mutex_;                       //互斥锁，用来保护pendingFunctors_的线程安全操作。
};


#endif //MUDUO_1_EVENTLOOP_H
