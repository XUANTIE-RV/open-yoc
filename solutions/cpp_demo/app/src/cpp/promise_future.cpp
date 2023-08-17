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

static void func(std::future<int>& fut)
{
    int x = fut.get();
    cout << "value: " << x << endl;
}

extern "C" int run_case_promise_future();
int run_case_promise_future()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();
    std::thread t(func, std::ref(fut));
    prom.set_value(144);
    t.join();
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}
