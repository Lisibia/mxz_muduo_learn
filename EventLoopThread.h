//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_EVENTLOOPTHREAD_H
#define MUDUO_1_EVENTLOOPTHREAD_H

#include "noncopyable.h"
#include"EventLoop.h"
#include <functional>
#include "Thread.h"
#include <memory>
#include<condition_variable>
class EventLoopThread : noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                    const std::string& name = std::string());
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;

};


#endif //MUDUO_1_EVENTLOOPTHREAD_H
