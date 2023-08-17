/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

static void foo()
{
    printf("===prepare sleep 2secs: %s\r\n", __FUNCTION__);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    printf("===finish sleep 2secs: %s\r\n", __FUNCTION__);
}

static void bar()
{
    printf("===prepare sleep 4secs: %s\r\n", __FUNCTION__);
    std::this_thread::sleep_for(std::chrono::seconds(4));
    printf("===finish sleep 4secs: %s\r\n", __FUNCTION__);
}

extern "C" int run_case_chrono();
int run_case_chrono()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    std::thread helper1(foo);

    std::thread helper2(bar);

    helper1.join();
    helper2.join();
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}

