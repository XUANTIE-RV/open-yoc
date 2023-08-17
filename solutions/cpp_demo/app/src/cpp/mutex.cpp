/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;
static std::mutex mutex_;

extern "C" int run_case_mutex();
int run_case_mutex()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    auto func1 = [](int k) {
        mutex_.lock();
        for (int i = 0; i < k; ++i) {
            cout << i << " ";
        }
        cout << endl;
        mutex_.unlock();
    };
    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(func1, 100);
    }
    for (auto& th : threads) {
        th.join();
    }
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}

