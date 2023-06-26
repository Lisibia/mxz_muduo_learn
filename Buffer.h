//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_BUFFER_H
#define MUDUO_1_BUFFER_H

#include <cstdio>
#include "noncopyable.h"
#include<vector>
#include <string>

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
//该类就是个接收发送缓冲区，并有读写操作，对粘包问题进行了处理。采用的
class Buffer:public  noncopyable {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
    :buffer_(kCheapPrepend + initialSize),
    readerIndex_(kCheapPrepend),
    writeIndex_(kCheapPrepend){}

    size_t readableBytes()    const {return writeIndex_ - readerIndex_;}
    size_t writableBytes()    const {return buffer_.size() - writeIndex_;}
    size_t prependableBytes() const {return readerIndex_;}

    //返回可读事件的起始地址
    const char* peek() const{
        return begin() + readerIndex_;
    }
    //判断是否复位
    void retrieve(size_t len){
        if(len < readableBytes()){
            readerIndex_ += len;
        }
        else{
            retrieveAll();
        }
    }
    //将onMessage函数上报的Buffer数据，转成string类型的数据返回。
    std::string retrieveAllAsString(){
        return retrieveAsString(readableBytes()); //可读取数据的长度
    }
    std::string retrieveAsString(size_t len){
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }
    void retrieveAll(){
        readerIndex_ = kCheapPrepend;
        writeIndex_  = kCheapPrepend;
    }

    void ensureWritableBytes(size_t len){
        if(writableBytes() < len){
            buffer_.resize(buffer_.size() * 2);
        }

    }
    //将数据添加到缓冲区中
    void append(const char* data, size_t len){
        ensureWritableBytes(len);
        std::copy(data,data + len,beginWrite());
        writeIndex_ += len;
    }
    char* beginWrite(){return begin() + writeIndex_;}
    const char* beginWrite()const{return begin() + writeIndex_;}
    //从Fd上读取数据。
    ssize_t readFd(int fd, int* saveErrno);
    //从Fd上写数据。
    ssize_t writeFd(int fd, int* saveErrno);
private:
    char* begin() {return &*buffer_.begin();}   //vector底层数组首元素的地址，也就是数组的起始地址。
    const char* begin() const {return &*buffer_.begin();}
    void makeSpace(size_t len){
        if(writableBytes() + prependableBytes() < len + kCheapPrepend){
            buffer_.resize(writeIndex_ + len);
        }
        else{
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writeIndex_, begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writeIndex_  = readerIndex_ + readable;
        }
    }
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writeIndex_;

};


#endif //MUDUO_1_BUFFER_H
