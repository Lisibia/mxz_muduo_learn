//
// Created by Meng on 2023/6/10.
//

#ifndef MUDUO_1_LOGGER_H
#define MUDUO_1_LOGGER_H
#include<string>
#include"noncopyable.h"

#define LOG_INFO(logMsgFormat, ...) \
do{                                 \
   Logger &logger = Logger::instance(); \
   logger.setLogLevel(INFO);        \
   char buf[1024] ={0};             \
   snprintf(buf,1024,logMsgFormat,##__VA_ARGS__); \
   logger.log(buf);\
}while(0)

#define LOG_ERROR(logMsgFormat, ...) \
do{                                 \
   Logger &logger = Logger::instance(); \
   logger.setLogLevel(ERROR);        \
   char buf[1024] ={0};             \
   snprintf(buf,1024,logMsgFormat,##__VA_ARGS__); \
   logger.log(buf);\
}while(0)

#define LOG_FATAL(logMsgFormat, ...) \
do{                                 \
   Logger &logger = Logger::instance(); \
   logger.setLogLevel(INFO);        \
   char buf[1024] ={0};             \
   snprintf(buf,1024,logMsgFormat,##__VA_ARGS__); \
   logger.log(buf);                  \
    exit(-1);                         \
}while(0)
#ifdef MUDEBUG
#define LOG_DEBUG(logMsgFormat, ...) \
do{                                 \
   Logger &logger = Logger::instance(); \
   logger.setLogLevel(DEBUG);        \
   char buf[1024] ={0};             \
   snprintf(buf,1024,logMsgFormat,##__VA_ARGS__); \
   logger.log(buf);\
}while(0)
#else
#define LOG_DEBUG(logMsgFormat, ...)
#endif
enum LogLevel{
    INFO, //普通信息
    ERROR,//错误信息
    FATAL,//core信息
    DEBUG,//调试信息
};
class Logger:noncopyable{
public:
    //获取日志唯一的实例对象
    static Logger& instance();
    //设置日志级别
    void setLogLevel(int Level);
    //写日志
    void log(std::string msg);
private:
    int logLevel_;
    Logger(){}
};
#endif //MUDUO_1_LOGGER_H
