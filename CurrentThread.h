//
// Created by Meng on 2023/6/15.
//

#ifndef MUDUO_1_CURRENTTHREAD_H
#define MUDUO_1_CURRENTTHREAD_H
#include<unistd.h>
#include<sys/syscall.h>
namespace CurrentThread {
    extern __thread int t_cachedTid;
    void cacheTid();
    inline int tid(){//获取当前线程的tid
        if(__builtin_expect(t_cachedTid == 0,0)){
            cacheTid();
        }
        return t_cachedTid;
    }

}


#endif //MUDUO_1_CURRENTTHREAD_H
