/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_hwpal_umdevxs.h
 *
 * Configuration for Driver Framework Device API implementation for
 * Linux user-space.
 */



#ifndef INCLUDE_GUARD_CS_HWPAL_UMDEVXS_H
#define INCLUDE_GUARD_CS_HWPAL_UMDEVXS_H

// we accept a few settings from the top-level configuration file
#include "cs_hwpal.h"

#define HWPAL_DEVICE0_UMDEVXS  HWPAL_DEVICE_TO_FIND

// Check if an endian conversion must be performed
#ifdef DRIVER_SWAPENDIAN
#define HWPAL_DEVICE_ENABLE_SWAP
#endif // DRIVER_SWAPENDIAN

#define HWPAL_DMARESOURCE_UMDEVXS_DCACHE_CTRL

#endif // INCLUDE_GUARD_CS_HWPAL_UMDEVXS_H


/* end of file cs_hwpal_umdevxs.h */
