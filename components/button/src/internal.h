/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef BUTTON_INTERNAL_H
#define BUTTON_INTERNAL_H

#include <aos/aos.h>
#include <aos/list.h>

typedef struct button_ops {
    int (*init)(button_t *b);
    int (*deinit)(button_t *b);
    int (*read)(button_t *b);
    int (*irq_disable)(button_t *b);
    int (*irq_enable)(button_t *b);
} button_ops_t;

struct button {
    slist_t next;
    long long t_ms;
    long long st_ms;    //enter irq time
    int pin_id;
    gpio_pin_handle_t *pin_hdl;
    int repeat;
    int state;
    int evt_id;
    int old_evt_id;
    int active;
    int irq_flag;
    int evt_flag;
    evt_cb cb;
    void *priv;
    int bc_flag;
    button_ops_t *ops;
    button_param_t param;
    char name[MAX_BUTTON_NAME];
};

#endif