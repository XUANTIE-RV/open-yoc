/*
 * Copyright (c) 2018, Pinecone Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	nuttx/sleep.h
 * @brief	NuttX sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#error "Include metal/sleep.h instead of metal/nuttx/sleep.h"
#endif

#ifndef __METAL_NUTTX_SLEEP__H__
#define __METAL_NUTTX_SLEEP__H__

#include <aos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int __metal_sleep_usec(unsigned int usec)
{
	aos_msleep(usec/1000);
	return 0;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_NUTTX_SLEEP__H__ */
