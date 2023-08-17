/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <functional>
#include <future>
#include <thread>

using namespace std;

extern "C" int run_case_async();
int func(int in)
{
    return in + 1;
}

int run_case_async()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    auto res = std::async(func, 5);
    // res.wait();
    cout << res.get() << endl; // 阻塞直到函数返回
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}

