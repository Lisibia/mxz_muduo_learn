//
// Created by Meng on 2023/6/18.
//

#include "TcpConnection.h"
#include "logger.h"
#include "Channel.h"
#include<errno.h>
#include <cstring>

static EventLoop* CheckLoopNotNull(EventLoop* loop){
    if(loop == nullptr){
        LOG_FATAL("%s:%s:%d mainLoop is null! \n", __FILE__,__FUNCTION__,__LINE__);
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop* loop,
const std::string& name,
int sockfd,
const InetAddress& localAddr,
const InetAddress& peerAddr)
:loop_(CheckLoopNotNull(loop)),
name_(name),
state_(kConnecting),
reading_(true),
socket_(new Socket(sockfd)),
channel_(new Channel(loop, sockfd)),
localAddr_(localAddr),
 peerAddr_(peerAddr),
 highWaterMark_(64*1024*1024)
{
    //给channel设置相应的回调函数，poller给channel通知感兴趣的事件发生了，channel就会执行响应的回调操作
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead,this,std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError,this));

    LOG_INFO("TcpConnection::ctor[ %s ] at %p fd= %d", name_.c_str(), this, sockfd);
    socket_->setKeepAlive(true);

}
TcpConnection::~TcpConnection(){
    LOG_INFO("TcpConnection::dtor[ %s ] at %p fd= %d" , name_.c_str(), this, channel_->fd());
}

void TcpConnection::handleRead(Timestamp receiveTime){
 int savedErrno = 0;
 ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
 if(n > 0){
     //已建立连接的用户有可读事件发生了
     messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
 }
 else if(n == 0){
     handleClose();
 }
 else{
     errno = savedErrno;
     LOG_ERROR("TcpConnection::handleRead\n");
     handleError();
 }
}
void TcpConnection::handleWrite(){
    if(channel_->isWriting())
    {
        int savedErrno = 0;
        ssize_t n = outputBuffer_.writeFd(channel_->fd(),&savedErrno);
        if(n > 0){
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0) {
                channel_->disenableWriting();

                if (writeCompleteCallback_) {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if(state_ == kDisconnected){
                    shutdownInLoop();
                }
            }
        }
        else{
            LOG_ERROR("TcpConnection::handleWrite\n");
        }
    }
    else{
        LOG_INFO("Connection fd = %d is down, no more writing.\n",channel_->fd());
    }
}
void TcpConnection::handleClose(){
    LOG_INFO("fd = %d, state = \n",channel_->fd());
    setState(kDisconnected);
    channel_->disableAll();
    //这是在做什么
    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis); //执行关闭连接的回调
    closeCallback_(guardThis);        //关闭连接的回调
}
void TcpConnection::handleError(){
    int err;
    int optval;
    socklen_t optlen = sizeof optval;
    if(::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0){
        err = errno;
    }{
        err = optval;
    }
    char t_errnobuf[512];
    LOG_ERROR("TcpConnection::handleError[%s]- SO_ERROR = %d ", name_.c_str(), err);
}
void TcpConnection::send(const std::string &buf) {
    if(state_ == kConnected){
        if(loop_->isInLoopThread()){
            sendInLoop(buf.c_str(),buf.size());
        }
        else{
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop,this,buf.c_str(), buf.size()));
        }
    }
}
/*
 * 发送数据 应用写的快，而内核发送数据慢，需要把发送数据写入缓冲区，而且设置了水位回调
 */
void TcpConnection::sendInLoop(const void* message, size_t len){
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if(state_ == kDisconnected){
        LOG_INFO("disconnected, give up writing.\n");
        return;
    }
    //表示channel_第一次开始写数据，而且缓冲区没有待发送数据
    if(!channel_->isWriting()&&outputBuffer_.readableBytes() == 0){
        nwrote = ::write(channel_->fd(),message, len);
        if(nwrote >= 0){ //发送成功
            remaining = len - nwrote;
            if(remaining == 0 && writeCompleteCallback_){
                //数据一次性发送完成，就不用再给channel设置EPOLLOUT事件
                loop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
            }
        }
        else{ //nwrote < 0
            nwrote = 0;
            if(errno != EWOULDBLOCK){ // EWOULDBLOCK正常返回
                LOG_ERROR("TcpConnection::sendInLoop\n");
                if(errno == EPIPE || errno == ECONNRESET){//EPIPE   ECONNRESET 接收对端的一个重置
                    faultError = true;
                }
            }
        }
    }
    //说明当前这一次write，并没有把数据全部发送出去，剩余数据需要保存到缓冲区中，然后给channel
    //注册EPOLLOUT事件，poller发现tcp的发送缓冲区有空间，会通知相应的sock-channel,调用writeCallback_回调方法
    //最终调用TcpConnection::handleWrite方法，把发送缓冲区中的数据全部发送完成
    if(!faultError && remaining > 0){
        // 目前发送缓冲区剩余的待发送数据的长度
        size_t oldLen = outputBuffer_.readableBytes();
        if(oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ && highWaterMarkCallback_){
            loop_->queueInLoop(std::bind(highWaterMarkCallback_,shared_from_this(), oldLen + remaining));
        }
        outputBuffer_.append(static_cast<const char*>(message)+nwrote, remaining);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
}
//创建连接的时候 使用
void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading(); //向poller注册channel的epollin事件

    connectionCallback_(shared_from_this());
}
//销毁连接的时候 使用
void TcpConnection::connectDestroyed() {
    if(state_ == kConnected){
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}
void TcpConnection::shutdown() {
    if(state_ == kConnected){
        setState(kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop,this));
    }
}
void TcpConnection::shutdownInLoop() {
    if(!channel_->isWriting()){
        socket_->shutdownWrite();
    }
}