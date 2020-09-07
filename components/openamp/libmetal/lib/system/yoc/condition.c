/*
 * Copyright (c) 2018, Pinecone Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	nuttx/condition.c
 * @brief	NuttX libmetal condition variable handling.
 */

#include <metal/condition.h>
#include <metal/irq.h>
#include <errno.h>
#include <semaphore.h>
#include <metal/mutex.h>

int metal_condition_wait(struct metal_condition *cv,
			 metal_mutex_t *m)
{
	return pthread_cond_wait(&cv->cond, m);
}
