//
// Created by Meng on 2023/6/18.
//

#ifndef MUDUO_1_CALLBACKS_H
#define MUDUO_1_CALLBACKS_H
#include<memory>
#include <functional>
#include "Timestamp.h"

class Buffer;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(TcpConnectionPtr&)>;
using HighWaterMarkCallback = std::function<void(TcpConnectionPtr&, size_t)>;

using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)>;
void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);




#endif //MUDUO_1_CALLBACKS_H
