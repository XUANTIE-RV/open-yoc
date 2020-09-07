/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#ifndef DEVICE_U1_H
#define DEVICE_U1_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char  *device_name;
} u1_nbiot_param_t;

extern void nbiot_u1_register(utask_t *task, u1_nbiot_param_t *param);

#ifdef __cplusplus
}
#endif

#endif
