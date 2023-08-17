/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#include "thread_entry.h"
#include "posix/timer.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

ThreadEntry::ThreadEntry() {
    function_ = NULL;
    arg_ = NULL;
    running_ = false;
    thread_not_quit_ = false;
}

ThreadEntry::~ThreadEntry() {

}

void ThreadEntry::Start(ThreadFunction function, void *arg, const char *name, int prio) {
    if (!function) {
        return;
    }

    function_ = function;
    arg_ = arg;
    running_ = true;
    thread_not_quit_ = true;

    pthread_t       tid;
    pthread_attr_t  attr;
    int ret = pthread_attr_init(&attr);
    if (ret < 0) {
        printf("Create thread attr failed, ret = %d\n", ret);
        return;
    }
    attr.sched_priority = prio;
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret < 0) {
        printf("Set thread attr failed, ret = %d\n", ret);
        return;
    }

    ret = pthread_create(&tid, &attr, thread, this);
    if (ret != 0) {
        running_ = false;
        printf("Create thread failed, ret = %d\n", ret);
        return;
    }
    if(name) {
        pthread_setname_np(tid, name);
    }
}

void ThreadEntry::Stop() {
    running_ = false;
    while (thread_not_quit_) {
        usleep(20000);
    }
}

void *ThreadEntry::thread(void *parse) {
    if (!parse) {
        return NULL;
    }

    ThreadEntry *life = static_cast<ThreadEntry *>(parse);
    while (life->running_) {
        if (!life->function_(life->arg_)) {
            life->running_ = false;
        }
    }

    life->thread_not_quit_ = false;
    return NULL;
}
