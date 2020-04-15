/*
 * Copyright (c) 2018, Pinecone Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	nuttx/cache.h
 * @brief	NuttX cache operation primitives for libmetal.
 */

#ifndef __METAL_CACHE__H__
#error "Include metal/cache.h instead of metal/nuttx/cache.h"
#endif

#ifndef __METAL_NUTTX_CACHE__H__
#define __METAL_NUTTX_CACHE__H__

#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void __metal_cache_flush(void *addr, unsigned int len)
{
	csi_dcache_clean_range(addr, len);
}

static inline void __metal_cache_invalidate(void *addr, unsigned int len)
{
	csi_dcache_invalid_range(addr, len);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_NUTTX_CACHE__H__ */
