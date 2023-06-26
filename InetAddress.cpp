//
// Created by Meng on 2023/6/11.
//

#include "InetAddress.h"

#include <cstring>

InetAddress::InetAddress(uint16_t port, std::string ip){
    memset(&addr_,0,sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}
std::string InetAddress::toIp() const{
    char buf[64] ={0};
    ::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof(buf));
    return buf;
}
std::string InetAddress::toPort()   const{
    uint16_t port = ntohs(addr_.sin_port);
    return std::to_string(port);
}
std::string InetAddress::toIpPort() const{
    //ip:port
    std::string ret = toIp() + ":" + toPort();
    return ret;
}
uint16_t InetAddress::port() const {
    return ntohs(addr_.sin_port);
}


//int main(){
//    InetAddress addr(8080);
//    std::string ret = addr.toIpPort();
//    return 0;
//}