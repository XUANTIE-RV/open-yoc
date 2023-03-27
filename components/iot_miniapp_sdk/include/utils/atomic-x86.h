/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JQUICK_UTILS_ATOMIC_X86_H
#define JQUICK_UTILS_ATOMIC_X86_H

#include <stdint.h>

#ifndef JQUICK_ATOMIC_INLINE
#define JQUICK_ATOMIC_INLINE inline __attribute__((always_inline))
#endif

extern JQUICK_ATOMIC_INLINE void jquick_compiler_barrier(void)
{
    __asm__ __volatile__(""
                         :
                         :
                         : "memory");
}

#if JQUICK_SMP == 0
extern JQUICK_ATOMIC_INLINE void jquick_memory_barrier(void)
{
    jquick_compiler_barrier();
}
#else
extern JQUICK_ATOMIC_INLINE void jquick_memory_barrier(void)
{
    __asm__ __volatile__("mfence"
                         :
                         :
                         : "memory");
}
#endif

extern JQUICK_ATOMIC_INLINE int32_t
jquick_atomic_acquire_load(volatile const int32_t* ptr)
{
    int32_t value = *ptr;
    jquick_compiler_barrier();
    return value;
}

extern JQUICK_ATOMIC_INLINE int32_t
jquick_atomic_release_load(volatile const int32_t* ptr)
{
    jquick_memory_barrier();
    return *ptr;
}

extern JQUICK_ATOMIC_INLINE void
jquick_atomic_acquire_store(int32_t value, volatile int32_t* ptr)
{
    *ptr = value;
    jquick_memory_barrier();
}

extern JQUICK_ATOMIC_INLINE void
jquick_atomic_release_store(int32_t value, volatile int32_t* ptr)
{
    jquick_compiler_barrier();
    *ptr = value;
}

extern JQUICK_ATOMIC_INLINE int
jquick_atomic_cas(int32_t old_value, int32_t new_value, volatile int32_t* ptr)
{
    int32_t prev;
    __asm__ __volatile__("lock; cmpxchgl %1, %2"
                         : "=a"(prev)
                         : "q"(new_value), "m"(*ptr), "0"(old_value)
                         : "memory");
    return prev != old_value;
}

extern JQUICK_ATOMIC_INLINE int
jquick_atomic_acquire_cas(int32_t old_value,
                          int32_t new_value,
                          volatile int32_t* ptr)
{
    /* Loads are not reordered with other loads. */
    return jquick_atomic_cas(old_value, new_value, ptr);
}

extern JQUICK_ATOMIC_INLINE int
jquick_atomic_release_cas(int32_t old_value,
                          int32_t new_value,
                          volatile int32_t* ptr)
{
    /* Stores are not reordered with other stores. */
    return jquick_atomic_cas(old_value, new_value, ptr);
}

extern JQUICK_ATOMIC_INLINE int32_t
jquick_atomic_add(int32_t increment, volatile int32_t* ptr)
{
    __asm__ __volatile__("lock; xaddl %0, %1"
                         : "+r"(increment), "+m"(*ptr)
                         :
                         : "memory");
    /* increment now holds the old value of *ptr */
    return increment;
}

extern JQUICK_ATOMIC_INLINE int32_t
jquick_atomic_inc(volatile int32_t* addr)
{
    return jquick_atomic_add(1, addr);
}

extern JQUICK_ATOMIC_INLINE int32_t
jquick_atomic_dec(volatile int32_t* addr)
{
    return jquick_atomic_add(-1, addr);
}

extern JQUICK_ATOMIC_INLINE int32_t
jquick_atomic_and(int32_t value, volatile int32_t* ptr)
{
    int32_t prev, status;
    do {
        prev = *ptr;
        status = jquick_atomic_cas(prev, prev & value, ptr);
    } while (__builtin_expect(status != 0, 0));
    return prev;
}

extern JQUICK_ATOMIC_INLINE int32_t
jquick_atomic_or(int32_t value, volatile int32_t* ptr)
{
    int32_t prev, status;
    do {
        prev = *ptr;
        status = jquick_atomic_cas(prev, prev | value, ptr);
    } while (__builtin_expect(status != 0, 0));
    return prev;
}

#endif /* JQUICK_UTILS_ATOMIC_X86_H */
