//
// Created by Meng on 2023/6/13.
//

#include "EPollPoller.h"
#include <sys/epoll.h>
#include<errno.h>
#include <unistd.h>
#include <cstring>
#include "logger.h"

//channel未添加到poller中
const int kNew = -1; //channel的成员index_ = -1
//channel已添加到poller中
const int kAdded = 1;
//channel从poller中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop): Poller(loop),
                                           epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
                                           events_(kInitEventListSize)
                                           {
    if(epollfd_ < 0){
        LOG_FATAL("epoll_create error:%d \n", errno);
    }
}
EPollPoller::~EPollPoller() noexcept {
    ::close(epollfd_);
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelLIst *activeChannels) const {
    for(int i = 0; i < numEvents; ++i){
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);//EventLoop就拿到了它的poller给它返回的所有发生事件的channel列表
    }
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelLIst* activeChannels) {
    LOG_INFO("func=%s=>fd total count:%lu\n",__FUNCTION__ ,channels_.size());
    int numEvents = ::epoll_wait(epollfd_,&*events_.begin(),static_cast<int>(events_.size()),timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if(numEvents > 0){
        LOG_DEBUG("%d events happened \n", numEvents);
        fillActiveChannels(numEvents,activeChannels);
        if(numEvents == events_.size()){
            events_.resize(2*events_.size());
        }
    }
    else if(numEvents == 0){
        LOG_DEBUG("%s timeout \n", __FUNCTION__ );
    }
    else {
        if(saveErrno != EINTR){ //中断
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() err!\n");
        }
    }
    return now;
}
//从poller中删除channel，删掉channels上的channel，如果channel状态是已添加到poller上，就将其从poller上删除
void EPollPoller::removeChannel(Channel* channel) {
    int fd = channel->fd();
    channels_.erase(fd);
    if(channel->index() == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}
/*
 *                  eventloop
 *           channellist     poller
 *                             ChannelMap<fd, channel>    epollfd
 */

void EPollPoller::updateChannel(Channel *channel) {
    const int index = channel->index();
    LOG_INFO("fd=%d, events=%d, index=%d\n", channel->fd(), channel->events(), index);
    if(index == kNew || index == kDeleted){
        int fd = channel->fd();
        if(index == kNew){
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else{
        int fd = channel->fd();
        if(channel->isNoneEvent()){
            update(EPOLL_CTL_DEL ,channel);
            channel->set_index(kDeleted);
        }
        else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}
//更新操作：改变监听事件的操作，具体可以向epollfd上添加监听事件，删除事件，更改事件
void EPollPoller::update(int operation, Channel* channel){
    epoll_event event;
    memset(&event,0,sizeof event);
    int fd = channel->fd();
    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if(::epoll_ctl(epollfd_,operation,fd,&event) < 0){
        if(operation == EPOLL_CTL_DEL){
            LOG_ERROR("epoll_ctl del:%d\n",errno);
        }
        else{
            LOG_FATAL("epoll_ctl_add/mod:%d\n", errno);
        }
    }
}