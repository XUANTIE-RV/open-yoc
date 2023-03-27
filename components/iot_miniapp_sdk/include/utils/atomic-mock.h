/*
* Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef __APPX_ATOMIC_MOCK_H__
#define __APPX_ATOMIC_MOCK_H__

#include <stdint.h>
#include "port/jquick_mutex.h"

extern JQuick_Mutex atomic_mutex;

#ifndef JQUICK_ATOMIC_INLINE
#define JQUICK_ATOMIC_INLINE inline __attribute__((always_inline))
#endif

extern JQUICK_ATOMIC_INLINE void jquick_compiler_barrier()
{
    __asm__ __volatile__(""
                         :
                         :
                         : "memory");
}

extern JQUICK_ATOMIC_INLINE void jquick_memory_barrier()
{
    jquick_compiler_barrier();
}

extern JQUICK_ATOMIC_INLINE int32_t jquick_atomic_acquire_load(
        volatile const int32_t* ptr)
{
    int32_t value = *ptr;
    jquick_memory_barrier();
    return value;
}

extern JQUICK_ATOMIC_INLINE int32_t jquick_atomic_release_load(
        volatile const int32_t* ptr)
{
    jquick_memory_barrier();
    return *ptr;
}

extern JQUICK_ATOMIC_INLINE void jquick_atomic_acquire_store(
        int32_t value,
        volatile int32_t* ptr)
{
    *ptr = value;
    jquick_memory_barrier();
}

extern JQUICK_ATOMIC_INLINE void jquick_atomic_release_store(
        int32_t value,
        volatile int32_t* ptr)
{
    jquick_memory_barrier();
    *ptr = value;
}

extern JQUICK_ATOMIC_INLINE int jquick_atomic_cas(
        int32_t old_value,
        int32_t new_value,
        volatile int32_t* ptr)
{
    jquick_mutex_lock(atomic_mutex);
    if (*ptr == old_value) {
        *ptr = new_value;
    }

    jquick_mutex_unlock(atomic_mutex);

    return 0;
}

extern JQUICK_ATOMIC_INLINE int jquick_atomic_acquire_cas(
        int32_t old_value,
        int32_t new_value,
        volatile int32_t* ptr)
{
    int status = jquick_atomic_cas(old_value, new_value, ptr);
    jquick_memory_barrier();
    return status;
}

extern JQUICK_ATOMIC_INLINE int jquick_atomic_release_cas(
        int32_t old_value,
        int32_t new_value,
        volatile int32_t* ptr)
{
    jquick_memory_barrier();
    return jquick_atomic_cas(old_value, new_value, ptr);
}

extern JQUICK_ATOMIC_INLINE int32_t jquick_atomic_add(
        int32_t increment,
        volatile int32_t* ptr)
{
    jquick_mutex_lock(atomic_mutex);
    int32_t prev = *ptr;
    *ptr = prev + increment;
    jquick_mutex_unlock(atomic_mutex);

    return prev;
}

extern JQUICK_ATOMIC_INLINE int32_t jquick_atomic_inc(volatile int32_t* addr)
{
    return jquick_atomic_add(1, addr);
}

extern JQUICK_ATOMIC_INLINE int32_t jquick_atomic_dec(volatile int32_t* addr)
{
    return jquick_atomic_add(-1, addr);
}

extern JQUICK_ATOMIC_INLINE int32_t jquick_atomic_and(
        int32_t value,
        volatile int32_t* ptr)
{
    jquick_mutex_lock(atomic_mutex);
    int32_t prev = *ptr;
    *ptr = prev & value;
    jquick_mutex_unlock(atomic_mutex);

    return prev;
}

extern JQUICK_ATOMIC_INLINE int32_t jquick_atomic_or(
        int32_t value,
        volatile int32_t* ptr)
{
    jquick_mutex_lock(atomic_mutex);
    int32_t prev = *ptr;
    *ptr = prev | value;
    jquick_mutex_unlock(atomic_mutex);

    return prev;
}

#endif  // __APPX_ATOMIC_MOCK_H__
