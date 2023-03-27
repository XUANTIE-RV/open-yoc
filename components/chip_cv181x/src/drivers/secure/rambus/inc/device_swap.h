/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* device_swap.h
 *
 * Driver Framework, Device API, Swap-Endianness function
 *
 * The document "Driver Framework Porting Guide" contains the detailed
 * specification of this API. The information contained in this header file
 * is for reference only.
 */



#ifndef INCLUDE_GUARD_DEVICE_SWAP_H
#define INCLUDE_GUARD_DEVICE_SWAP_H

// Driver Framework Basic Defs API
#include "basic_defs.h"     // uint32_t, inline

/*----------------------------------------------------------------------------
 * Device_SwapEndian32
 *
 * This function can be used to swap the byte order of a 32bit integer. The
 * implementation could use custom CPU instructions, if available.
 */
static inline uint32_t
Device_SwapEndian32(
        const uint32_t Value)
{
#ifdef DEVICE_SWAP_SAFE
    return (((Value & 0x000000FFU) << 24) |
            ((Value & 0x0000FF00U) <<  8) |
            ((Value & 0x00FF0000U) >>  8) |
            ((Value & 0xFF000000U) >> 24));
#else
    // reduces typically unneeded AND operations
    return ((Value << 24) |
            ((Value & 0x0000FF00U) <<  8) |
            ((Value & 0x00FF0000U) >>  8) |
            (Value >> 24));
#endif
}

#endif /* Include Guard */

/* end of file device_swap.h */
