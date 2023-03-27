/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_hwpal.h
 *
 * Configuration Settings for Driver Framework Implementation
 * for the Security-IP-28 hardware.
 */



#ifndef INCLUDE_GUARD_CS_HWPAL_H
#define INCLUDE_GUARD_CS_HWPAL_H

// we accept a few settings from the top-level configuration file
#include "cs_driver.h"
#include "cs_adapter.h"

// Host hardware platform specific extensions
#include "cs_hwpal_ext.h"

// maximum allowed length for a device name
#define HWPAL_MAX_DEVICE_NAME_LENGTH 64

#define HWPAL_DEVICE_MAGIC   54333

#ifdef DRIVER_NAME
#define HWPAL_DRIVER_NAME DRIVER_NAME
#endif

// Trace memory leaks for DMAResource API implementation
#define HWPAL_TRACE_DMARESOURCE_LEAKS

// Is host platform 64-bit?
#ifdef DRIVER_64BIT_HOST
#define HWPAL_64BIT_HOST
// Is device 64-bit? Only makes sense on 64-bit host.
#endif  // DRIVER_64BIT_HOST

// only define this if the platform hardware guarantees cache coherence of
// DMA buffers, i.e. when SW does not need to do coherence management.
#ifdef ARCH_X86
#define HWPAL_ARCH_COHERENT
#else
#undef HWPAL_ARCH_COHERENT
#endif

// Logging / tracing control
#ifndef DRIVER_PERFORMANCE
#define HWPAL_STRICT_ARGS_CHECK
#ifdef ARCH_X86
// Enabled for DMAResource API implementation on x86 debugging purposes only
#undef HWPAL_ARCH_COHERENT
#endif
//#define HWPAL_TRACE_DEVICE_FIND
//#define HWPAL_TRACE_DEVICE_READ
//#define HWPAL_TRACE_DEVICE_WRITE
//#define HWPAL_TRACE_DMARESOURCE_WRITE
//#define HWPAL_TRACE_DMARESOURCE_READ
//#define HWPAL_TRACE_DMARESOURCE_PREPOSTDMA
//#define HWPAL_TRACE_DMARESOURCE_BUF
#endif

// enable code that looks at flag bit2 and performs actual endianness swap
//#define HWPAL_DEVICE_ENABLE_SWAP

// Use sleepable or non-sleepable lock ?
//#define HWPAL_LOCK_SLEEPABLE

#define HWPAL_DMA_NRESOURCES     ADAPTER_MAX_DMARESOURCE_HANDLES

// Use direct I/O bypassing the OS functions,
// I/O device must swap bytes in words
#ifdef DRIVER_ENABLE_SWAP_SLAVE
#define HWPAL_DEVICE_DIRECT_MEMIO
#endif

// Enable cache-coherent DMA buffer allocation
//#define HWPAL_DMARESOURCE_ALLOC_CACHE_COHERENT

// Use minimum required cache-control functionality for DMA-safe buffers
//#define HWPAL_DMARESOURCE_MINIMUM_CACHE_CONTROL

#endif // INCLUDE_GUARD_CS_HWPAL_H

/* end of file cs_hwpal.h */
