/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;

extern "C" int run_case_thread();
int run_case_thread()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    auto func1 = [](int i) {
        cout << "thread " << i << " create\r\n";
    };
    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(func1, i);
    }
    for (auto& th : threads) {
        th.join();
    }
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}

