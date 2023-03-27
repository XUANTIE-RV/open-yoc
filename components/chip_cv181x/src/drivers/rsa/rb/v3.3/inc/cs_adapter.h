/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_adapter.h
 *
 * Configuration Settings for the SLAD Adapter module.
 */



#ifndef INCLUDE_GUARD_CS_ADAPTER_H
#define INCLUDE_GUARD_CS_ADAPTER_H

// we accept a few settings from the top-level configuration file
#include "cs_driver.h"

// Adapter extensions
//#include "cs_adapter_ext.h"


/*----------------------------------------------------------------------------
 * Adapter EIP-28 Configuration
 */

// Define PKA Type to build: EIP-28
#define ADAPTER_PKA_EIP28

// Logical interrupt number for the EIP-28 DATA READY interrupt event
#ifdef DRIVER_EIP28_READY_IRQ
#define ADAPTER_EIP28_READY_IRQ             DRIVER_EIP28_READY_IRQ
#endif

// Logical interrupt number for the EIP-28 DATA READY interrupt event
#ifdef DRIVER_PHY_EIP28_READY_IRQ
#define ADAPTER_PHY_EIP28_READY_IRQ         DRIVER_PHY_EIP28_READY_IRQ
#endif

#ifdef DRIVER_EIP28_REMOVE_FIRMWARE_DOWNLOAD
#define ADAPTER_EIP28_REMOVE_FIRMWARE_DOWNLOAD
#endif

#ifdef DRIVER_INTERRUPTS
#define ADAPTER_EIP28_INTERRUPT_ENABLE
#endif // DRIVER_INTERRUPTS


/*----------------------------------------------------------------------------
 * Common Adapter PKA Configuration
 */

#ifdef DRIVER_PKA_VECTOR_MAX_WORDS
#define ADAPTER_PKA_VECTOR_MAX_WORDS        DRIVER_PKA_VECTOR_MAX_WORDS
#endif

#ifdef DRIVER_OVERLAP_INPUT_RESULT
#define ADAPTER_PKA_OVERLAP_INPUT_RESULT
#endif

// Enable strict argument checking
#ifndef DRIVER_PERFORMANCE
#define ADAPTER_PKA_STRICT_ARGS
#endif

// _REMOVE configuration options
#define ADAPTER_PKA_REMOVE_ADD

#define ADAPTER_PKA_REMOVE_SUBTRACT

#define ADAPTER_PKA_REMOVE_SUBADD

#define ADAPTER_PKA_REMOVE_SHIFTRIGHT

#define ADAPTER_PKA_REMOVE_SHIFTLEFT

#define ADAPTER_PKA_REMOVE_MULTIPLY

#define ADAPTER_PKA_REMOVE_MOD

#define ADAPTER_PKA_REMOVE_DIVIDE

#define ADAPTER_PKA_REMOVE_COPY

#define ADAPTER_PKA_REMOVE_COMPARE

#define ADAPTER_PKA_REMOVE_MODINV

//#define ADAPTER_PKA_REMOVE_MODEXP

#define ADAPTER_PKA_REMOVE_MODEXPCRT

#define ADAPTER_PKA_REMOVE_ECCADD

#define ADAPTER_PKA_REMOVE_ECCMUL

#define ADAPTER_PKA_REMOVE_ECCMULMONT

#define ADAPTER_PKA_REMOVE_DSASIGN

#define ADAPTER_PKA_REMOVE_DSAVERIFY

#define ADAPTER_PKA_REMOVE_ECCDSASIGN

#define ADAPTER_PKA_REMOVE_ECCDSAVERIFY

/* NOTE add */
#define PKA_ADAPTER_REMOVE_SCAP


/*----------------------------------------------------------------------------
 * Global Adapter Configuration
 */

#ifdef DRIVER_NAME
#define ADAPTER_DRIVER_NAME                 DRIVER_NAME
#endif

#ifdef DRIVER_LICENSE
#define ADAPTER_LICENSE                     DRIVER_LICENSE
#endif


// Filter for tracing interrupts: 0 - no traces, 0xFFFFFFFF - all interrupts
#ifdef DRIVER_INTERRUPTS
#define ADAPTER_INTERRUPTS_TRACEFILTER      0x0
#endif // DRIVER_INTERRUPTS

#ifdef DRIVER_PKA_FSM_ERROR_CHECK_ENABLE
#define ADAPTER_PKA_FSM_ERROR_CHECK_ENABLE
#endif

#define ADAPTER_REMOVE_BOUNCEBUFFERS

#define ADAPTER_BOUNCE_FACTOR               1

#define ADAPTER_MAX_DMARESOURCE_HANDLES     (ADAPTER_BOUNCE_FACTOR * 30)


#endif /* Include Guard */


/* end of file cs_adapter.h */
