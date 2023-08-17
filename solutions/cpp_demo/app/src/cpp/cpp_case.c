/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern int run_case_thread();
extern int run_case_mutex();
extern int run_case_atomic();
extern int run_case_condition();
extern int run_case_lock_guard();
extern int run_case_unique_lock();
extern int run_case_call_once();
extern int run_case_async();
extern int run_case_packaged_task_future();
extern int run_case_promise_future();
extern int run_case_chrono();
int run_cpp_case()
{
    printf("======cpp case start: %s\r\n", __FUNCTION__);
    run_case_thread();
    run_case_mutex();
    run_case_atomic();
    run_case_condition();
    run_case_lock_guard();
    run_case_unique_lock();
    run_case_call_once();
    run_case_async();
    run_case_packaged_task_future();
    run_case_promise_future();
    run_case_chrono();
    printf("======cpp case end : %s\r\n", __FUNCTION__);
    return 0;
}

