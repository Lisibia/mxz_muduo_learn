//
// Created by Meng on 2023/6/11.
//

#ifndef MUDUO_1_INETADDRESS_H
#define MUDUO_1_INETADDRESS_H


#include<string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include"copyable.h"
/*
 * 对sock地址addr_in的封装。主要包括对addr_in的初始化，设置ip和port，获取ip和port‘
 * 特别注意的是构造都使用了explicit关键字修饰，默认构造继承自copyable，使不能隐式调用。
 */
class InetAddress :public copyable{
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const struct sockaddr_in& addr): addr_(addr){}
    sa_family_t family() const {return addr_.sin_family;}
    std::string toIp()     const;
    std::string toIpPort() const;
    std::string toPort()   const;
    uint16_t port()        const;
    const sockaddr_in* getSockAddr() const {return &addr_;}
    void setSockAddr(const sockaddr_in& addr){addr_ = addr;}
private:
    struct sockaddr_in addr_;
};


#endif //MUDUO_1_INETADDRESS_H
