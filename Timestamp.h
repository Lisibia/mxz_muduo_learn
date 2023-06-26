//
// Created by Meng on 2023/6/10.
//

#ifndef MUDUO_1_TIMESTAMP_H
#define MUDUO_1_TIMESTAMP_H
#include<iostream>
#include<string>
//时间类，用来打印时间，主要有返回现在时间，将时间格式化输出
class Timestamp {
public:
    Timestamp():microSecondsSinceEpoch_(0){}
    explicit Timestamp(int64_t microSecondsSinceEpoch):microSecondsSinceEpoch_(microSecondsSinceEpoch){}
    static Timestamp now();
    std::string toString()const;
private:
    int64_t microSecondsSinceEpoch_;
};


#endif //MUDUO_1_TIMESTAMP_H
