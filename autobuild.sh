#!/bin/bash
#
#set -e
#
#if [ ! -d `pwd`/build ];then
#  mkdir `pwd`/build
#fi
#
#rm -rf `pwd`/build/*
#
#cd `pwd`/build && cmake .. && make
## 回到项目根目录
#cd ..
#
##把头文件拷贝到/usr/include/muduo_1 so库拷贝到 /usr/lib
#
## shellcheck disable=SC1069
#if [ ! -d /usr/include/muduo_test ]; then
#    mkdir /usr/include/muduo_test
#fi
#
## shellcheck disable=SC2045
## shellcheck disable=SC2006
#for header in `ls *.h`
#do
#  cp "$header" /usr/include/muduo_test
#done
#
## shellcheck disable=SC2046
## shellcheck disable=SC2006
#cp `pwd`/lib/libmuduo_test.so /usr/lib
#
#ldconfig



set -e

# 如果没有build目录，创建该目录
# shellcheck disable=SC2046
# shellcheck disable=SC2006
if [ ! -d `pwd`/build ]; then
    # shellcheck disable=SC2046
    # shellcheck disable=SC2006
    mkdir `pwd`/build
fi

# shellcheck disable=SC2046
rm -rf `pwd`/build/*

# shellcheck disable=SC2046
cd `pwd`/build &&
    cmake .. &&
    make

# 回到项目根目录
cd ..

# 把头文件拷贝到 /usr/include/mymuduo  so库拷贝到 /usr/lib    PATH
if [ ! -d /usr/include/mymuduo ]; then
    mkdir /usr/include/mymuduo
fi

# shellcheck disable=SC2045
# shellcheck disable=SC2006
for header in `ls *.h`
do
    cp "$header" /usr/include/mymuduo
done

# shellcheck disable=SC2046
# shellcheck disable=SC2006
cp `pwd`/lib/libmymuduo.so /usr/lib

ldconfig