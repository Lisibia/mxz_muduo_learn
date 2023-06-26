//
// Created by Meng on 2023/6/11.
//

#include "EventLoop.h"
#include "logger.h"
#include"Poller.h"
#include"EPollPoller.h"
#include"Channel.h"
#include<sys/eventfd.h>
#include<fcntl.h>

//防止一个线程创建多个EventLoop
__thread EventLoop *t_loopInThisThread = nullptr;
//默认的poller IO复用的超时时间
const int kPollTimes = 10000;
//创建wakeupfd，用nodify的方式来唤醒subReactor处理新来的channel
int createEventfd(){
    int evtfd = ::eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0){
        LOG_FATAL("failed in eventfd");
    }
    return evtfd;
}
EventLoop::EventLoop():looping_(false),quit_(false), callingPendingFunctors_(false),threadId_(CurrentThread::tid()),
poller_(Poller::newDefaultPoller(this)),wakeupFd_(createEventfd()),wakeupChannel_(new Channel(this, wakeupFd_)),
currentActiveChannel_(nullptr)
{
    LOG_DEBUG("EventLoop Created %p in thread %d", this, threadId_);
    if(t_loopInThisThread){
        LOG_FATAL("Another EventLoop %p exists in this thread %d \n",t_loopInThisThread,threadId_);
    }
    else{
        t_loopInThisThread = this;
    }
    //检测到wakeupFd_上的监听事件后,就调用回调函数。
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead,this));
    //每一个EventLoop都将监听wakeupchannel的EPOLLIN
    wakeupChannel_->enableReading();
}
EventLoop::~EventLoop() {
    LOG_DEBUG("~EventLoop");
    //将监听事件置为空
    wakeupChannel_->disableAll();
    //将channel从poller中删除
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t  nn = read(wakeupFd_,&one, sizeof one);
    if(nn != one){
        LOG_ERROR("EventLoop::handleRead() reads %d bytes instead of 8." ,nn);
    }
}
//开启事件循环
void EventLoop::loop(){
    looping_ = true;
    quit_ = false;
    LOG_INFO("EventLoop %p start looping \n" ,this);
    while(!quit_){
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimes,&activeChannels_);
        for(Channel *channel : activeChannels_){
            //Poller监听哪些channel发生事件了，然后上报给EventLoop，通知channel处理相应的事件
            channel->handlerEvent(pollReturnTime_);
        }
        //执行当前EventLoop事件循环需要处理的回调操作。
        doPendingFunctors();
    }
    looping_ = false;
}
//退出事件循环 1.loop在自己的线程中调用quit 2.在非loop的线程中，调用loop的quit.
void EventLoop::quit(){
    quit_ = true;
    if(!isInLoopThread()){ //如果是在其他线程中调用quit，在一个subloop中，调用了mainloop的quit
        wakeup();
    }
}


//在当前loop中执行cb
void EventLoop::runInLoop(Functor cb){
    if(isInLoopThread()){//在loop线程中，执行cb
        cb();
    }
    else{//在非当前loop线程中，就需要唤醒loop所在线程，执行cb。
        queueInLoop(std::move(cb));
    }
}
//把cb放入到队列中，唤醒loop所在的线程，执行cb
void EventLoop::queueInLoop(Functor cb){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    if(!isInLoopThread() || callingPendingFunctors_) {
        //callingPendingFunctors_当前loop正在执行回调，现在又添加了新的回调，loop有了新的回调
        //需要将主线程唤醒，执行新添加进去的回调
        wakeup(); //唤醒loop所在线程执行cb
    }
}


//用来唤醒loop所在的线程,wakeupFd_发生读事件，就从poller中被唤醒开始做事情。
void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_,&one, sizeof one);
    if(n != sizeof one){
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8.\n",n);
    }
}
//EventLoop的方法->Poller的方法。
void EventLoop::updateChannel(Channel* channel){
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel* channel){
    poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel* channel){
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(const auto& functor:functors){
        functor();//执行当前loop需要执行的回调操作
    }
    callingPendingFunctors_ = false;

}