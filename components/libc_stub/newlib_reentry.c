/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#if defined(CONFIG_NEWLIB_REENTRY_SUPPORT) && CONFIG_NEWLIB_REENTRY_SUPPORT && defined(AOS_COMP_POSIX)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/lock.h>
#include <aos/kernel.h>
#include "pthread.h"

struct __lock {
    int             dynamic;
    pthread_mutex_t mutex;
};

struct __lock __lock___sinit_recursive_mutex;
struct __lock __lock___sfp_recursive_mutex;
struct __lock __lock___atexit_recursive_mutex;
struct __lock __lock___at_quick_exit_mutex;
struct __lock __lock___malloc_recursive_mutex;
struct __lock __lock___env_recursive_mutex;
struct __lock __lock___tz_mutex;
struct __lock __lock___dd_hash_mutex;
struct __lock __lock___arc4random_mutex;

typedef struct __lock * _LOCK_T;
void
__retarget_lock_init (_LOCK_T *lock)
{
    if (lock) {
        struct __lock *plock;

        plock = aos_malloc(sizeof(struct __lock));
        plock->dynamic = 1;
        pthread_mutex_init(&plock->mutex, NULL);
        *lock = plock;
    }
}

void
__retarget_lock_init_recursive(_LOCK_T *lock)
{
    //FIXME: mutex is recursize default for rhino
    __retarget_lock_init(lock);
}

void
__retarget_lock_close(_LOCK_T lock)
{
    if (lock) {
        pthread_mutex_destroy(&lock->mutex);
        if (lock->dynamic) {
            aos_free(lock);
        }
    }
}

void
__retarget_lock_close_recursive(_LOCK_T lock)
{
    //FIXME: mutex is recursize default for rhino
    __retarget_lock_close(lock);
}

void
__retarget_lock_acquire (_LOCK_T lock)
{
    if (lock) {
        pthread_mutex_lock(&lock->mutex);
    }
}

void
__retarget_lock_acquire_recursive (_LOCK_T lock)
{
    __retarget_lock_acquire(lock);
}

int
__retarget_lock_try_acquire(_LOCK_T lock)
{
    if (lock) {
        pthread_mutex_trylock(&lock->mutex);
    }
    return 1;
}

int
__retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
    return __retarget_lock_try_acquire(lock);
}

void
__retarget_lock_release (_LOCK_T lock)
{
    if (lock) {
        pthread_mutex_unlock(&lock->mutex);
    }
}

void
__retarget_lock_release_recursive (_LOCK_T lock)
{
    __retarget_lock_release(lock);
}
#endif

