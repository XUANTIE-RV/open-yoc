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

static int func(int in)
{
    return in + 1;
}

extern "C" int run_case_packaged_task_future();
int run_case_packaged_task_future()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    std::packaged_task<int(int)> task(func);
    std::future<int> fut = task.get_future();
    std::thread(std::move(task), 5).detach();
    cout << "result " << fut.get() << endl;
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}

