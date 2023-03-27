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
/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef JQUICK_UTILS_ATOMIC_STD_H
#define JQUICK_UTILS_ATOMIC_STD_H

#include <stdint.h>
#include <sys/types.h>
#include <stdatomic.h>

#ifndef JQUICK_ATOMIC_INLINE
#define JQUICK_ATOMIC_INLINE inline __attribute__((always_inline))
#endif

JQUICK_ATOMIC_INLINE
volatile atomic_int_least32_t* to_atomic_int_least32_t(volatile const int32_t* addr) {
#ifdef __cplusplus
    return reinterpret_cast<volatile atomic_int_least32_t*>(const_cast<volatile int32_t*>(addr));
#else
    return (volatile atomic_int_least32_t*)addr;
#endif
}

/*
 * Basic arithmetic and bitwise operations.  These all provide a
 * barrier with "release" ordering, and return the previous value.
 *
 * These have the same characteristics (e.g. what happens on overflow)
 * as the equivalent non-atomic C operations.
 */
extern JQUICK_ATOMIC_INLINE
int32_t jquick_atomic_inc(volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    /* Int32_t, if it exists, is the same as int_least32_t. */
    return atomic_fetch_add_explicit(a, 1, memory_order_release);
}

extern JQUICK_ATOMIC_INLINE
int32_t jquick_atomic_dec(volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    return atomic_fetch_sub_explicit(a, 1, memory_order_release);
}

extern JQUICK_ATOMIC_INLINE
int32_t jquick_atomic_add(int32_t value, volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    return atomic_fetch_add_explicit(a, value, memory_order_release);
}

extern JQUICK_ATOMIC_INLINE
int32_t jquick_atomic_and(int32_t value, volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    return atomic_fetch_and_explicit(a, value, memory_order_release);
}

extern JQUICK_ATOMIC_INLINE
int32_t jquick_atomic_or(int32_t value, volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    return atomic_fetch_or_explicit(a, value, memory_order_release);
}

/*
 * Perform an atomic load with "acquire" or "release" ordering.
 *
 * Note that the notion of a "release" ordering for a load does not
 * really fit into the C11 or C++11 memory model.  The extra ordering
 * is normally observable only by code using memory_order_relaxed
 * atomics, or data races.  In the rare cases in which such ordering
 * is called for, use memory_order_relaxed atomics and a leading
 * atomic_thread_fence (typically with memory_order_acquire,
 * not memory_order_release!) instead.  If you do not understand
 * this comment, you are in the vast majority, and should not be
 * using release loads or replacing them with anything other than
 * locks or default sequentially consistent atomics.
 */
extern JQUICK_ATOMIC_INLINE
int32_t jquick_atomic_acquire_load(volatile const int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    return atomic_load_explicit(a, memory_order_acquire);
}

extern JQUICK_ATOMIC_INLINE
int32_t jquick_atomic_release_load(volatile const int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    atomic_thread_fence(memory_order_seq_cst);
    /* Any reasonable clients of this interface would probably prefer   */
    /* something weaker.  But some remaining clients seem to be         */
    /* abusing this API in strange ways, e.g. by using it as a fence.   */
    /* Thus we are conservative until we can get rid of remaining       */
    /* clients (and this function).                                     */
    return atomic_load_explicit(a, memory_order_relaxed);
}

/*
 * Perform an atomic store with "acquire" or "release" ordering.
 *
 * Note that the notion of an "acquire" ordering for a store does not
 * really fit into the C11 or C++11 memory model.  The extra ordering
 * is normally observable only by code using memory_order_relaxed
 * atomics, or data races.  In the rare cases in which such ordering
 * is called for, use memory_order_relaxed atomics and a trailing
 * atomic_thread_fence (typically with memory_order_release,
 * not memory_order_acquire!) instead.
 */
extern JQUICK_ATOMIC_INLINE
void jquick_atomic_acquire_store(int32_t value, volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    atomic_store_explicit(a, value, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);
    /* Again overly conservative to accomodate weird clients.   */
}

extern JQUICK_ATOMIC_INLINE
void jquick_atomic_release_store(int32_t value, volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    atomic_store_explicit(a, value, memory_order_release);
}

/*
 * Compare-and-set operation with "acquire" or "release" ordering.
 *
 * This returns zero if the new value was successfully stored, which will
 * only happen when *addr == oldvalue.
 *
 * (The return value is inverted from implementations on other platforms,
 * but matches the ARM ldrex/strex result.)
 *
 * Implementations that use the release CAS in a loop may be less efficient
 * than possible, because we re-issue the memory barrier on each iteration.
 */
extern JQUICK_ATOMIC_INLINE
int jquick_atomic_acquire_cas(int32_t oldvalue, int32_t newvalue,
                               volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    return !atomic_compare_exchange_strong_explicit(
            a, &oldvalue, newvalue,
            memory_order_acquire,
            memory_order_acquire);
}

extern JQUICK_ATOMIC_INLINE
int jquick_atomic_release_cas(int32_t oldvalue, int32_t newvalue,
                               volatile int32_t* addr)
{
    volatile atomic_int_least32_t* a = to_atomic_int_least32_t(addr);
    return !atomic_compare_exchange_strong_explicit(
            a, &oldvalue, newvalue,
            memory_order_release,
            memory_order_relaxed);
}

/*
 * Fence primitives.
 */
extern JQUICK_ATOMIC_INLINE
void jquick_compiler_barrier(void)
{
    __asm__ __volatile__ ("" : : : "memory");
    /* Could probably also be:                          */
    /* atomic_signal_fence(memory_order_seq_cst);       */
}

extern JQUICK_ATOMIC_INLINE
void jquick_memory_barrier(void)
{
    atomic_thread_fence(memory_order_seq_cst);
}

#endif  // __APPX_ATOMIC_MOCK_H__
