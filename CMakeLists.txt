#cmake_minimum_required(VERSION 3.25)
#project(muduo_1)
#
#set(CMAKE_CXX_STANDARD 11)
#
#LINK_DIRECTORIES(/usr/local/lib/)
#
#add_executable(muduo_1 main.cpp muduo_server.cpp)
#
#target_link_libraries(${PROJECT_NAME} -lmuduo_net -lmuduo_base -lpthread)

cmake_minimum_required(VERSION 3.20)
project(muduo_test)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
#target_link_libraries(${PROJECT_NAME} -lpthread)
#mymuduo最终编译成so动态库，设置通道库的路径，放在根目录的lib文件夹下
set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/../lib)
#设置调试信息 以及 启动c++11语言标准
#set(CMAKE_CXX_FLAGS "CMAKE_CXX_FLAGS -g -std=c++11")
set(CMAKE_CXX_STANDARD 11)
#参加生成动态连接库的文件
aux_source_directory(. SRT_LIST)
#编译生成动态库
#add_library(muduo_1 SHARED ${PROJECT_SOURCE_DIR}/noncopyable.h ${PROJECT_SOURCE_DIR}/muduo_server.cpp)
#add_library(muduo_1 SHARED noncopyable.h logger.h logger.cpp Timestamp.cpp Timestamp.h InetAddress.cpp InetAddress.h copyable.cpp copyable.h TcpServer.cpp TcpServer.h EventLoop.cpp EventLoop.h Channel.cpp Channel.h Poller.cpp Poller.h DefaultPoller.cpp EPollPoller.cpp EPollPoller.h CurrentThread.cpp CurrentThread.h Thread.cpp Thread.h EventLoopThread.cpp EventLoopThread.h EventLoopThreadPool.cpp EventLoopThreadPool.h Socket.cpp Socket.h Acceptor.cpp Acceptor.h TcpConnection.cpp TcpConnection.h Callbacks.cpp Callbacks.h Buffer.cpp Buffer.h)
#add_library(muduo_test noncopyable.h logger.h logger.cpp Timestamp.cpp Timestamp.h InetAddress.cpp InetAddress.h copyable.cpp copyable.h EventLoop.cpp EventLoop.h TcpServer.cpp TcpServer.h Channel.cpp Channel.h Poller.cpp Poller.h DefaultPoller.cpp EPollPoller.cpp EPollPoller.h CurrentThread.cpp CurrentThread.h Thread.cpp Thread.h EventLoopThread.cpp EventLoopThread.h EventLoopThreadPool.cpp EventLoopThreadPool.h Socket.cpp Socket.h Acceptor.cpp Acceptor.h TcpConnection.cpp TcpConnection.h Callbacks.cpp Callbacks.h Buffer.cpp Buffer.h)
add_executable(muduo_test noncopyable.h logger.h logger.cpp Timestamp.cpp Timestamp.h InetAddress.cpp InetAddress.h copyable.cpp copyable.h EventLoop.cpp EventLoop.h TcpServer.cpp TcpServer.h Channel.cpp Channel.h Poller.cpp Poller.h DefaultPoller.cpp EPollPoller.cpp EPollPoller.h CurrentThread.cpp CurrentThread.h Thread.cpp Thread.h EventLoopThread.cpp EventLoopThread.h EventLoopThreadPool.cpp EventLoopThreadPool.h Socket.cpp Socket.h Acceptor.cpp Acceptor.h TcpConnection.cpp TcpConnection.h Callbacks.cpp Callbacks.h Buffer.cpp Buffer.h testServer.cpp)
target_link_libraries(${PROJECT_NAME} -lpthread)
