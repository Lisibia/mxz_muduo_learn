//
// Created by Meng on 2023/6/10.
//
#include "logger.h"
//获取日志唯一的实例对象
#include<iostream>
#include "Timestamp.h"
using std::cout;
using std::endl;
Logger& Logger::instance(){
    static Logger logger;
    return logger;
}
//设置日志级别
void Logger::setLogLevel(int Level){
    this->logLevel_ = Level;
}
//写日志 级别信息 time ： msg
void Logger::log(std::string msg){
    switch (logLevel_) {
        case INFO:
            cout<<"[INFO]";break;
        case ERROR:
            cout<<"[ERROR]";break;
        case FATAL:
            cout<<"[FATAL]";break;
        case DEBUG:
            cout<<"[DEBUG]";break;
    }
    cout<<Timestamp::now().toString() << "msg:"<<msg;
}
//int main(){
//    std::string t = "test";
//    LOG_ERROR("%s\n","t");
//    return 0;
//}