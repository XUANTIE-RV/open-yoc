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

#ifndef WORK_H
#define WORK_H

#if defined(__cplusplus)
extern "C"
{
#endif

struct k_work_q {
    struct kfifo fifo;
};

int k_work_q_start();

enum {
    K_WORK_STATE_PENDING,
};

struct k_work;

/* work define*/
typedef void (*k_work_handler_t)(struct k_work *work);
struct k_work {
    sys_snode_t node;
    k_work_handler_t handler;
    atomic_t flags[1];
    int      index;
};

int k_work_init(struct k_work *work, k_work_handler_t handler);

void k_work_submit(struct k_work *work);

/*delay work define*/
struct k_delayed_work {
    struct k_work work;
    struct k_work_q *work_q;
    k_timer_t timer;
    uint64_t tick_end;
};

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler);
int k_delayed_work_submit(struct k_delayed_work *work, uint32_t delay);
int k_delayed_work_cancel(struct k_delayed_work *work);
int k_delayed_work_remaining_get(struct k_delayed_work *work);

#ifdef __cplusplus
}
#endif

#endif /* WORK_H */

