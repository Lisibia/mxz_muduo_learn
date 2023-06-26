//
// Created by Meng on 2023/6/10.
//

#ifndef MUDUO_1_NONCOPYABLE_H
#define MUDUO_1_NONCOPYABLE_H
/*
 * noncopyable 的派生类拷贝构造和赋值操作就被删除了，只能进行可以使用默认构造和析构
 */
class noncopyable{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
public:
    noncopyable() = default;
    ~noncopyable() = default;
};
#endif //MUDUO_1_NONCOPYABLE_H
