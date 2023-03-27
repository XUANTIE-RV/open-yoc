/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_BASE_MUTEX_H__
#define ___JQUICK_BASE_MUTEX_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* JQuick_Mutex;

JQuick_Mutex jquick_mutex_create();

int jquick_mutex_lock(
        JQuick_Mutex m);

int jquick_mutex_unlock(
        JQuick_Mutex m);

int jquick_mutex_destroy(
        JQuick_Mutex m);

#ifdef __cplusplus
}
#endif

#endif  // ___JQUICK_BASE_MUTEX_H__
