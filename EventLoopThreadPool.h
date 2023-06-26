//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_EVENTLOOPTHREADPOOL_H
#define MUDUO_1_EVENTLOOPTHREADPOOL_H

#include"noncopyable.h"
#include"EventLoopThread.h"

#include <functional>
#include<vector>
#include<string>
#include<memory>
class EventLoop;
class EventLoopThreadPool : noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads){numThreads_ = numThreads;}
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    //如果工作在多线程模式下，baseloop_默认以轮询的方式分配channel给subloop
    EventLoop* getNextLoop();

    //对于相同的hash代码，这个函数返回相同的Event Loop
    EventLoop* getLoopForHash(size_t hashCode);
    std::vector<EventLoop*> getAllLoops();

    bool started() const{return started_;}
    const std::string& name() const{return name_;}
private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};


#endif //MUDUO_1_EVENTLOOPTHREADPOOL_H
