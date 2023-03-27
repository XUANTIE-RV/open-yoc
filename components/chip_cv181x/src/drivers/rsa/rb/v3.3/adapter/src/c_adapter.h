/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* c_adapter.h
 *
 * Default Adapter configuration
 */



#ifndef INCLUDE_GUARD_C_ADAPTER_H
#define INCLUDE_GUARD_C_ADAPTER_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Top-level Adapter configuration
#include "cs_adapter.h"

/****************************************************************************
 * Adapter general configuration parameters
 */

// Name of the driver
#ifndef ADAPTER_DRIVER_NAME
#define ADAPTER_DRIVER_NAME "rambus"
#endif

// Type of licence
#ifndef ADAPTER_LICENSE
#define ADAPTER_LICENSE "Proprietary"
#endif

// Define PKA Type to build: EIP-28
#ifndef ADAPTER_PKA_EIP28
#define ADAPTER_PKA_EIP28
#endif

// Logical interrupt number for the EIP-28 DATA READY interrupt event
#ifndef ADAPTER_EIP28_READY_IRQ
#define ADAPTER_EIP28_READY_IRQ             1
#endif

// Logical interrupt number for the EIP-28 DATA READY interrupt event
#ifndef ADAPTER_PHY_EIP28_READY_IRQ
#define ADAPTER_PHY_EIP28_READY_IRQ         1
#endif

// Name for the "Ready" interrupt
#ifndef ADAPTER_EIP28_READY_INT_NAME
#define ADAPTER_EIP28_READY_INT_NAME        "EIP28_READY"
#endif

// // Enable to remove the firmware download feature
// #ifndef ADAPTER_EIP28_REMOVE_FIRMWARE_DOWNLOAD
// #undef ADAPTER_EIP28_REMOVE_FIRMWARE_DOWNLOAD
// #endif

// Name for the Firmware file
#ifndef ADAPTER_EIP28_FIRMWARE_NAME
#define ADAPTER_EIP28_FIRMWARE_NAME      "fw_eip28"
#endif


/****************************************************************************
 * Adapter general optional configuration parameters
 */

// Enable interrupts instead of polling.
// ADAPTER_EIP28_INTERRUPT_ENABLE

// Enable Side Channel Attack Protection:
// ADAPTER_PKA_SCAP_ENABLE

// Enable fsm_error checking:
// ADAPTER_PKA_FSM_ERROR_CHECK_ENABLE

// Bounce buffers:
// ADAPTER_REMOVE_BOUNCEBUFFERS

// Remove firmware download feature.
// ADAPTER_EIP28_REMOVE_FIRMWARE_DOWNLOAD


// Enable the FSM error checking feature
// ADAPTER_PKA_FSM_ERROR_CHECK_ENABLE

// Remove the bounce buffers feature
// ADAPTER_REMOVE_BOUNCEBUFFERS

// Enable the check to use overlapping input and result data
// ADAPTER_PKA_OVERLAP_INPUT_RESULT

// Enable strict argument checking
// ADAPTER_PKA_STRICT_ARGS

// _REMOVE configuration options for commands
// ADAPTER_PKA_REMOVE_ADD
// ADAPTER_PKA_REMOVE_SUBTRACT
// ADAPTER_PKA_REMOVE_SUBADD
// ADAPTER_PKA_REMOVE_SHIFTRIGHT
// ADAPTER_PKA_REMOVE_SHIFTLEFT
// ADAPTER_PKA_REMOVE_MULTIPLY
// ADAPTER_PKA_REMOVE_MOD
// ADAPTER_PKA_REMOVE_DIVIDE
// ADAPTER_PKA_REMOVE_COPY
// ADAPTER_PKA_REMOVE_COMPARE
// ADAPTER_PKA_REMOVE_MODINV
// ADAPTER_PKA_REMOVE_MODEXP
// ADAPTER_PKA_REMOVE_MODEXPCRT
// ADAPTER_PKA_REMOVE_ECCADD
// ADAPTER_PKA_REMOVE_ECCMUL
// ADAPTER_PKA_REMOVE_ECCMULMONT
// ADAPTER_PKA_REMOVE_DSASIGN
// ADAPTER_PKA_REMOVE_DSAVERIFY
// ADAPTER_PKA_REMOVE_ECCDSASIGN
// ADAPTER_PKA_REMOVE_ECCDSAVERIFY
// ADAPTER_PKA_SCAP_ENABLE

/****************************************************************************
 * EIP-28 Adapter specific configuration parameters
 */

// Number of bounce buffers for DMAResource_CheckAndRegister'ed buffers
// note: used only when concurrency is impossible
#ifndef ADAPTER_BOUNCE_FACTOR
#define ADAPTER_BOUNCE_FACTOR               1
#endif

// Maximum number of DMA handles
#ifndef ADAPTER_MAX_DMARESOURCE_HANDLES
#define ADAPTER_MAX_DMARESOURCE_HANDLES     (ADAPTER_BOUNCE_FACTOR * 30)
#endif

// Maximum size of a PKA vector in words.
#ifndef ADAPTER_PKA_VECTOR_MAX_WORDS
#define ADAPTER_PKA_VECTOR_MAX_WORDS 128
#endif

// Device-specific interrupt sources tracing,
// Set to 0xFFFFFFFF to trace all the 32 sources
// Set to 0 to disable traces
#ifndef ADAPTER_INTERRUPTS_TRACEFILTER
#define ADAPTER_INTERRUPTS_TRACEFILTER 0    // No traces by default
#endif


#include "c_adapter_ext.h"      // chip-specific extensions


#endif /* Include Guard */


/* end of file c_adapter.h */
