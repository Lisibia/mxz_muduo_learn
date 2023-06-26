//
// Created by Meng on 2023/6/13.
//
#include "Poller.h"
#include "EPollPoller.h"
#include <stdlib.h>
//不放在 poller.cpp的原因是基类最好不包含派生类的实现
Poller* Poller::newDefaultPoller(EventLoop *loop) {
    if(::getenv("MUDUO_USE_POLL")){
        return nullptr; //生成poll的实例
    }
    else{
        return new EPollPoller(loop); //生成epoll的实例
    }
}
