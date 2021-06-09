#ifndef BUILTIN_TIMER_H_
#define BUILTIN_TIMER_H_
#include <stdint.h>
#include "linked_buffer.h"
#include "co_list.h"

struct builtin_timer
{
    struct co_list_hdr hdr;
    void (*timer_cb)(void *);
    void *param;
};

#define DEF_BUILTIN_TIMER_ENV(num) \
    DEF_LINKED_BUF(builtin_timer_env,struct builtin_timer,(num))

#define INIT_BUILTIN_TIMER_ENV() \
    do{ \
        INIT_LINKED_BUF(builtin_timer_env);\
        builtin_timer_env_register(&builtin_timer_env);\
    }while(0)

void builtin_timer_env_register(linked_buffer_t *env);

void *builtin_timer_create(void (*cb)(void *));

void builtin_timer_delete(void *timer);

void builtin_timer_stop(void *timer);

void builtin_timer_start(void *timer,uint32_t timeout,void *param);

#endif

