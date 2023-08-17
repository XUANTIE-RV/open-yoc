/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <aos/kernel.h>
#include <iostream>                // std::cout
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable

using namespace std;

static std::mutex mtx; // 全局互斥锁.
static std::condition_variable cv; // 全局条件变量.
static bool ready = false; // 全局标志位.

void do_print_id(int id)
{
    std::unique_lock <std::mutex> lck(mtx);
    while (!ready) // 如果标志位不为 true, 则等待...
        cv.wait(lck); // 当前线程被阻塞, 当全局标志位变为 true 之后,
    // 线程被唤醒, 继续往下执行打印线程编号id.
    std::cout << "thread " << id << '\n';
}

void go()
{
    std::unique_lock <std::mutex> lck(mtx);
    ready = true; // 设置全局标志位为 true.
    cv.notify_all(); // 唤醒所有线程.
}

extern "C" int run_case_condition();
int run_case_condition()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    std::thread threads[10];
    // spawn 10 threads:
    for (int i = 0; i < 10; ++i)
        threads[i] = std::thread(do_print_id, i);

    std::cout << "sleep 5s and 10 threads ready to race...\n";
    aos_msleep(5000);
    go(); // go!

    for (auto & th:threads)
        th.join();
    printf("======cpp case end : %s\r\n", __FUNCTION__);

    return 0;
}

