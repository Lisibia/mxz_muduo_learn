//
// Created by Meng on 2023/6/12.
//
#ifndef MUDUO_1_POLLER_H
#define MUDUO_1_POLLER_H
#include "noncopyable.h"
#include <vector>
#include<unordered_map>
#include "Timestamp.h"
class Channel;
class EventLoop;
//muduo库中多路事件分发器的核心IO复用模块
class Poller:noncopyable {
public:
    using ChannelLIst = std::vector<Channel*>;
    Poller(EventLoop *loop);
    virtual ~Poller();
    //给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelLIst* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    //判断channel是否在当前Poller当中
    virtual bool hasChannel(Channel* channel) const;
    //EventLoop可以通过该接口获取默认的IO复用的具体实现
    //为啥是static
    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    EventLoop *ownerLoop_;
};


#endif //MUDUO_1_POLLER_H
