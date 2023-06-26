//
// Created by Meng on 2023/6/18.
//

#include "EventLoopThread.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb ,
                const std::string& name):callback_(std::move(cb)),
                loop_(nullptr),
                exiting_(false),
                thread_(std::bind(&EventLoopThread::threadFunc,this),name),
                mutex_(),
                cond_()
                {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if(loop_ != nullptr){
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    thread_.start();
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_ == nullptr){
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}
//下面程序单独在一个线程中运行
void EventLoopThread::threadFunc() {
    EventLoop loop; //创建一个独立的eventloop，和上面的线程是一一对应的， one loop per thread
    if(callback_){
        callback_(&loop);
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop(); //eventloop loop -> poller.poll
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}