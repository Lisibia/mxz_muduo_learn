//
// Created by Meng on 2023/6/18.
//

#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg):
baseLoop_(baseLoop),name_(nameArg), started_(false), numThreads_(0), next_(0)
{
}
EventLoopThreadPool::~EventLoopThreadPool(){
    //啥也不用做
     }
void EventLoopThreadPool::start(const ThreadInitCallback& cb){
    started_ = true;
    for(int i = 0; i < numThreads_; ++i){
        char buf[name_.size() + 32];
        snprintf(buf,sizeof buf, "%s%d", name_.c_str(),i);
        EventLoopThread* t = new EventLoopThread(cb,buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
    if(numThreads_ == 0 && cb){
        cb(baseLoop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    EventLoop* loop = baseLoop_;
    if(!loops_.empty()){
    loop = loops_[next_];
        if(next_ >= loops_.size()){
            next_ = 0;
        }
    }
    return loop;
}

//对于相同的hash代码，这个函数返回相同的Event Loop
EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode){
    EventLoop* loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[hashCode % loops_.size()];
    }
    return loop;
}
std::vector<EventLoop*> EventLoopThreadPool::getAllLoops(){
    if(loops_.empty()){
        return std::vector<EventLoop*>(1,baseLoop_);
    }
    else{
        return loops_;
    }
}