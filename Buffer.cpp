//
// Created by Meng on 2023/6/18.
//

#include "Buffer.h"
#include "errno.h"
#include <sys/uio.h>
#include <unistd.h>

/*
 * 从Fd上读取数据，poller工作在LT模式
 * Buffer缓冲区是有大小的，但是从fd上读取数据的时候，不知道tcp数据最终的大小
 */
ssize_t Buffer::readFd(int fd, int* saveErrno){
    char extrabuf[65536]; //栈上内存空间
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len  = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    //一次读64K数据，如果 可写的空间大小大于64k，就只用 1，要不就1,2都用
    const int iovcnt = (writable < sizeof extrabuf ? 2 : 1);
    const ssize_t n = readv(fd,vec,iovcnt);
    if(n < 0){
        *saveErrno = errno;
    }
    else if(n <= writable){
        writeIndex_ += n;
    }
    else{
        writeIndex_ = buffer_.size();
        append(extrabuf,n - writable);
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int* saveErrno){
    int n = ::write(fd,peek(),readableBytes());
    if(n < 0){
        *saveErrno = errno;
    }
    else if(n <= readableBytes()){
        readerIndex_ += n;
    }
    return n;
}
