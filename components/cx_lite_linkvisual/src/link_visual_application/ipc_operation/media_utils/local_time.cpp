/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#include "local_time.h"
#include "posix/timer.h"
#include <time.h>

uint64_t GetLocalTime() {
    struct timespec     ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);


    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
