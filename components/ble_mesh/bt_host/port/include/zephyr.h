/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ZEPHYR_H
#define ZEPHYR_H
#include  <stdint.h>
#include  <stddef.h>
#include  <misc/slist.h>
#include  <misc/__assert.h>
//#include  "bt_config_check.h"
#include <ble_mesh_config.h>
#include "kport.h"
#include "mbox.h"
#include "work.h"
#include <stdlib.h>


#if defined(__cplusplus)
extern "C"
{
#endif

#ifndef ASSERT
#define ASSERT __ASSERT
#endif

#define _STRINGIFY(x) #x

#define ___in_section(a, b, c) \
    __attribute__((section("." _STRINGIFY(a)            \
                           "." _STRINGIFY(b)           \
                           "." _STRINGIFY(c))))

#define __in_section(a, b, c) ___in_section(a, b, c)

#define __in_section_unique(seg) ___in_section(seg, _FILE_PATH_HASH, \
        __COUNTER__)

#define __in_btstack_section() __in_section(btstack, static, func)

#ifndef ARG_UNUSED
#define ARG_UNUSED(x) (void)(x)
#endif

#ifndef __packed
#define __packed              __attribute__((__packed__))
#endif

#ifndef __printf_like
#define __printf_like(f, a)   __attribute__((format (printf, f, a)))
#endif
#define  STACK_ALIGN 4
#define __stack __aligned(STACK_ALIGN)

#define K_FOREVER -1
#define K_NO_WAIT 0

/* Unaligned access */
#define UNALIGNED_GET(p)                        \
    __extension__ ({                            \
        struct  __attribute__((__packed__)) {               \
            __typeof__(*(p)) __v;                   \
        } *__p = (__typeof__(__p)) (p);                 \
        __p->__v;                           \
    })

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

enum _poll_types_bits {
    _POLL_TYPE_IGNORE,
    _POLL_TYPE_SIGNAL,
    _POLL_TYPE_SEM_AVAILABLE,
    _POLL_TYPE_DATA_AVAILABLE,
    _POLL_NUM_TYPES
};

#define _POLL_TYPE_BIT(type) (1 << ((type)-1))

enum _poll_states_bits {
    _POLL_STATE_NOT_READY,
    _POLL_STATE_SIGNALED,
    _POLL_STATE_SEM_AVAILABLE,
    _POLL_STATE_DATA_AVAILABLE,
    _POLL_NUM_STATES
};

#define _POLL_STATE_BIT(state) (1 << ((state)-1))

#define _POLL_EVENT_NUM_UNUSED_BITS                             \
    (32 - (0 + 8                                    /* tag */   \
           + _POLL_NUM_TYPES + _POLL_NUM_STATES + 1 /* modes */ \
          ))

#define K_POLL_SIGNAL_INITIALIZER(obj)                                         \
    {                                                                          \
        .poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events), .signaled = 0, \
                       .result = 0,                                                           \
    }

struct k_poll_event {
    sys_dnode_t     _node;
    struct _poller *poller;
    uint32_t           tag : 8;
uint32_t           type :
    _POLL_NUM_TYPES;
uint32_t           state :
    _POLL_NUM_STATES;
    uint32_t           mode : 1;
uint32_t           unused :
    _POLL_EVENT_NUM_UNUSED_BITS;
    union {
        void                 *obj;
        struct k_poll_signal *signal;
        struct k_sem         *sem;
        struct kfifo        *fifo;
        struct k_queue       *queue;
    };
};

struct k_poll_signal {
    sys_dlist_t  poll_events;
    unsigned int signaled;
    int          result;
};

#define K_POLL_STATE_NOT_READY 0
#define K_POLL_STATE_EADDRINUSE 1
#define K_POLL_STATE_SIGNALED 2
#define K_POLL_STATE_SEM_AVAILABLE 3
#define K_POLL_STATE_DATA_AVAILABLE 4
#define K_POLL_STATE_FIFO_DATA_AVAILABLE K_POLL_STATE_DATA_AVAILABLE

#define K_POLL_TYPE_IGNORE 0
#define K_POLL_TYPE_SIGNAL 1
#define K_POLL_TYPE_SEM_AVAILABLE 2
#define K_POLL_TYPE_DATA_AVAILABLE 3
#define K_POLL_TYPE_FIFO_DATA_AVAILABLE K_POLL_TYPE_DATA_AVAILABLE

#define K_POLL_EVENT_STATIC_INITIALIZER(event_type, event_mode, event_obj,     \
                                        event_tag)                             \
{                                                                          \
    .type = event_type, .tag = event_tag, .state = K_POLL_STATE_NOT_READY, \
                               .mode = event_mode, .unused = 0, { .obj = event_obj },                 \
}

extern int  k_poll_signal(struct k_poll_signal *signal, int result);
extern int  k_poll(struct k_poll_event *events, int num_events,
                   int32_t timeout);
extern void k_poll_event_init(struct k_poll_event *event, uint32_t type,
                              int mode, void *obj);
extern void _handle_obj_poll_events(sys_dlist_t *events, uint32_t state);

/* public - polling modes */
enum k_poll_modes {
    /* polling thread does not take ownership of objects when available */
    K_POLL_MODE_NOTIFY_ONLY = 0,

    K_POLL_NUM_MODES
};

#define BT_STACK(name, size) \
    k_thread_stack_t name[(size) / sizeof(k_thread_stack_t)];

#define BT_STACK_NOINIT(name, size) \
    k_thread_stack_t name[(size) / sizeof(k_thread_stack_t)]  __attribute__((section(".data")));

static inline  void k_call_stacks_analyze()
{
    return;
}

static inline char *K_THREAD_STACK_BUFFER(uint32_t *sym)
{
    return NULL;
}

#define k_oops()    while(1)

void *k_current_get(void);
uint32_t k_get_tick(void);
uint32_t k_tick2ms(uint32_t tick);
void k_sleep(int32_t ms);

uint32_t k_uptime_get_32();


#define _DO_CONCAT(x, y) x ## y
#define _CONCAT(x, y) _DO_CONCAT(x, y)

#ifndef BUILD_ASSERT
/* compile-time assertion that makes the build fail */
#define BUILD_ASSERT(EXPR) \
    enum _CONCAT(__build_assert_enum, __COUNTER__) { \
        _CONCAT(__build_assert, __COUNTER__) = 1 / !!(EXPR) \
    }
#endif
#ifndef BUILD_ASSERT_MSG
/* build assertion with message -- common implementation swallows message. */
#define BUILD_ASSERT_MSG(EXPR, MSG) BUILD_ASSERT(EXPR)
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC 1000
#endif
/**
 * @brief Generate timeout delay from milliseconds.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a ms milliseconds to perform the requested operation.
 *
 * @param ms Duration in milliseconds.
 *
 * @return Timeout delay value.
 */
#define K_MSEC(ms)     (ms)

/**
 * @brief Generate timeout delay from seconds.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a s seconds to perform the requested operation.
 *
 * @param s Duration in seconds.
 *
 * @return Timeout delay value.
 */
#define K_SECONDS(s)   K_MSEC((s) * MSEC_PER_SEC)

/**
 * @brief Generate timeout delay from minutes.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a m minutes to perform the requested operation.
 *
 * @param m Duration in minutes.
 *
 * @return Timeout delay value.
 */
#define K_MINUTES(m)   K_SECONDS((m) * 60)

/**
 * @brief Generate timeout delay from hours.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a h hours to perform the requested operation.
 *
 * @param h Duration in hours.
 *
 * @return Timeout delay value.
 */
#define K_HOURS(h)     K_MINUTES((h) * 60)

#define popcount(x) __builtin_popcount(x)

extern int ffs32_lsb(uint32_t i);
extern int ffs32_msb(uint32_t i);

#define find_msb_set(x) ffs32_msb(x)
#define find_lsb_set(x) ffs32_lsb(x)

#define k_thread_foreach(...)

#define k_free free

#define snprintk snprintf
#if defined(__cplusplus)
}
#endif

#endif /* ZEPHYR_H */

