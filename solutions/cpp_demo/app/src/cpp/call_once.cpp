/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

static std::once_flag onceflag;

extern "C" int run_case_call_once();
void CallOnce()
{
    std::call_once(onceflag, []() {
        cout << "call once" << endl;
    });
}

int run_case_call_once()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    std::thread threads[5];
    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(CallOnce);
    }
    for (auto& th : threads) {
        th.join();
    }
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}
