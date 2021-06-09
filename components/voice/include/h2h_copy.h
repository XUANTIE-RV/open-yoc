/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _VOICE_H2H_COPY_AI_H_
#define _VOICE_H2H_COPY_AI_H_

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void start_h2h_copy(void *src, void *dst, size_t size);
void wait_h2h_copy_done(void);
void h2h_copy_ai_init(void);
void h2h_copy_shm_addr_set(void *addr);

void h2h_copy_init(void);
void *h2h_copy_shm_addr_get(void);

#ifdef __cplusplus
}
#endif

#endif