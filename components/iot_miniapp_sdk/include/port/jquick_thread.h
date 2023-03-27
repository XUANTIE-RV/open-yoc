/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_BASE_THREAD_H__
#define ___JQUICK_BASE_THREAD_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* JQuick_Thread;
typedef void* (*Runner)(void* arg);

JQuick_Thread jquick_thread_create(
        const char* name,
        Runner runner,
        void* args);

JQuick_Thread jquick_thread_create_with_stack_size(
        const char* name,
        Runner runner,
        void* args,
        int stackSize);

int jquick_thread_destroy(
        JQuick_Thread thread);

void jquick_thread_set_current_name(
        const char* name);

void jquick_thread_set_name(
        JQuick_Thread thread,
        const char* name);

JQuick_Thread jquick_thread_get_current();

int jquick_thread_set_priority(
        JQuick_Thread thread,
        int priority);

#ifdef __cplusplus
}
#endif

#endif  // ___JQUICK_BASE_THREAD_H__
