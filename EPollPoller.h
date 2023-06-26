//
// Created by Meng on 2023/6/13.
//

#ifndef MUDUO_1_EPOLLPOLLER_H
#define MUDUO_1_EPOLLPOLLER_H

#include "Poller.h"
#include <vector>
#include "Channel.h"
/*
 * epoll的使用
 * epoll_create
 * epoll_ctl  add/mod/del
 * epoll_wait
 */
//该类主要是封装了epoll，epoll_create, epoll_ctl, epoll_wait
class EPollPoller:public Poller {
public:

    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;
    //重写基类poller的抽象方法
    Timestamp poll(int timeoutMs, ChannelLIst* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
private:
    using EventList = std::vector<struct epoll_event>;
    //填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelLIst *activeChannels) const;
    //更新channel通道
    void update(int operation, Channel* channel);
    static const int kInitEventListSize = 16;
    int epollfd_;
    EventList events_;
};


#endif //MUDUO_1_EPOLLPOLLER_H
