//
// Created by Meng on 2023/6/18.
//

#include "Thread.h"
#include "CurrentThread.h"
#include<semaphore.h>

std::atomic<int> Thread::numCreated_(0);
Thread::Thread(ThreadFunc func, const std::string& name)
:func_(std::move(func)),name_(name),started_(false),joined_(false),tid_(0){
    setDefaultName();
}

Thread::~Thread() {
    if(started_&&!joined_){
        pthread_->detach();
    }
}

void Thread::setDefaultName() {
    int num = ++numCreated_;
    if(name_.empty()){
        char buf[32] = {0};
        snprintf(buf,sizeof buf,"Thread%d", num);
        name_ = buf;
    }
}
void Thread::start() { //一个thread对象，记录的就是一个新线程的详细信息
    started_ = true;
    sem_t sem_;
    sem_init(&sem_, false, 0);

    pthread_ = std::make_shared<std::thread>([&](){
        //获取线程的tid值
        tid_ = CurrentThread::tid();
        sem_post(&sem_);
        //开启一个新线程，专门执行该线程函数
        func_();
    });
    //用信号量进行同步
    sem_wait(&sem_);
}
void Thread::join() {
    joined_ = true;
    pthread_->join();
}