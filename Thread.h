//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_THREAD_H
#define MUDUO_1_THREAD_H

#include <functional>
#include<thread>
#include<memory>
#include "noncopyable.h"
#include<atomic>
class Thread : noncopyable {
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc, const std::string& name = std::string());
    ~Thread();

    void start();
    void join(); //return pthread_join();

    bool started() const {return started_;}
    pid_t tid() const {return tid_;}
    const std::string& name() const {return name_;}
    static int numCreated(){return numCreated_;}
private:
    void setDefaultName();

    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> pthread_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    static std::atomic<int> numCreated_;
};


#endif //MUDUO_1_THREAD_H
