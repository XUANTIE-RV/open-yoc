/*
 * Copyright (c) 2018, Pinecone Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	nuttx/time.c
 * @brief	NuttX libmetal time handling.
 */

#include <metal/time.h>
#include <aos/kernel.h>

unsigned long long metal_get_timestamp(void)
{
	unsigned long long t = aos_now();

	return t;
}
