/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_driver.h
 *
 * Top-level Product Configuration Settings.
 */



#ifndef INCLUDE_GUARD_CS_DRIVER_H
#define INCLUDE_GUARD_CS_DRIVER_H

#include "cs_driver_ext.h"            // host HW specific extensions
#include "cs_systemtestconfig.h"      // defines SYSTEMTEST_CONFIG_Cnn

// Driver license for the module registration with the Linux kernel
#define DRIVER_LICENSE                      "Proprietary"

// Driver name used for reporting
#define DRIVER_NAME                         "security-eip-28"

// Set FSM error check enabled
#define DRIVER_PKA_FSM_ERROR_CHECK_ENABLE

// Max Vector size as imposed by the hardware.
// Note: if the hardware is configured with a reduced internal FIFO size,
// this parameter must be reduced.
#define DRIVER_PKA_VECTOR_MAX_WORDS         130

/* Allow input and result vectors to overlap */
#if defined(DRIVER_EIP28_HW_EIP28_LNME) || defined(DRIVER_EIP28_HW_EIP28_PKCP)
#define DRIVER_OVERLAP_INPUT_RESULT
#endif

// Logical interrupt number for the EIP-28 DATA READY interrupt event
#define DRIVER_EIP28_READY_IRQ              1

// Enables the SCAP feature.
#define DRIVER_PKA_SCAP_ENABLE

// Enable the Montgomery Ladder feature.
// Other values still use the ModExpVar method using the programmed number of odd powers.
// Can only be used on EIP-28A and EIP-28B configurations with at least HW2.2/FW3.2.
// #define DRIVER_EIP28_MONTGOMERY_LADDER_OPTION

/* Use the options below to selectively remove unused features.
   The SLAD EIP-28 Adapter does not support these PKA operations
   whereas the EIP-28 Driver Library supports them.
   This will disable these PKA operations in the EIP-28 Driver Library too. */
//#define DRIVER_PKA_REMOVE_ADD
//#define DRIVER_PKA_REMOVE_SUBTRACT
//#define DRIVER_PKA_REMOVE_SUBADD
//#define DRIVER_PKA_REMOVE_SHIFTRIGHT
//#define DRIVER_PKA_REMOVE_SHIFTLEFT
//#define DRIVER_PKA_REMOVE_COPY
//#define DRIVER_PKA_REMOVE_COMPARE
//#define DRIVER_PKA_REMOVE_DSASIGN
//#define DRIVER_PKA_REMOVE_DSAVERIFY
//#define DRIVER_PKA_REMOVE_ECCDSASIGN
//#define DRIVER_PKA_REMOVE_ECCDSAVERIFY

// Remove the ECC Montgomery Multiply
// Should be un-commented on configurations with older versions then
// HW2.2 and FW3.2.
// #define DRIVER_PKA_REMOVE_ECCMULMONT

// activate in case of endianness difference between CPU and EIP
// to ask driver to swap byte order of all control words
// we assume that if ARCH is not x86 or arm, then CPU is big endian
#ifdef ARCH_POWERPC
#define DRIVER_SWAPENDIAN
#endif //ARCH_POWERPC

// // C0 = Interrupts, no performance
// #ifdef SYSTEMTEST_CONFIGURATION_C0
// #define DRIVER_INTERRUPTS
// //#define DRIVER_PERFORMANCE
// #endif

// C1 = Polling, no performance
#define SYSTEMTEST_CONFIGURATION_C1
#ifdef SYSTEMTEST_CONFIGURATION_C1
//#define DRIVER_INTERRUPTS
//#define DRIVER_PERFORMANCE
#endif

// // C2 = Interrupts, performance
// #ifdef SYSTEMTEST_CONFIGURATION_C2
// #define DRIVER_INTERRUPTS
// #define DRIVER_PERFORMANCE
// #endif

#define DRIVER_EIP28_HW_EIP28_PKCP

#endif /* Include Guard */


/* end of file cs_driver.h */
