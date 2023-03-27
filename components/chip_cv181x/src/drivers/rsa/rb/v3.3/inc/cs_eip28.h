/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* cs_eip28.h
 *
 * Configuration Settings for the EIP28 Driver Library.
 */



#ifndef INCLUDE_GUARD_CS_EIP28_H
#define INCLUDE_GUARD_CS_EIP28_H

// we accept a few settings from the top-level configuration file
#include "cs_driver.h"
#include "cs_eip28_ext.h"       // EIP-28 HW specific configuration


#if !defined(DRIVER_EIP28_HW_EIP28_LNME) && \
    !defined(DRIVER_EIP28_HW_EIP28_PKCP)
#error "Unsupported EIP-28 version"
#endif

// set this option to enable checking of all arguments to all EIP28 functions,
// disable it to reduce code size and reduce overhead
#ifndef DRIVER_PERFORMANCE
#define EIP28_STRICT_ARGS
#endif

// set this option to enable checking the vectors in PKA RAM
#ifndef DRIVER_PERFORMANCE
#define EIP28_STRICT_VECTORS
#endif

#ifdef DRIVER_EIP28_HW_EIP28_LNME
// Offset of PKA RAM with respect to address of PKA registers.
#define EIP28_OFFSET_PKARAM         0x2000
#endif // DRIVER_EIP28_HW_EIP28_LNME

#ifdef DRIVER_EIP28_HW_EIP28_PKCP
// Offset of PKA RAM with respect to address of PKA registers.
#define EIP28_OFFSET_PKARAM         0x2000
#endif // DRIVER_EIP28_HW_EIP28_PKCP

// define the maximum length of a vector (in bits)
// that is supported by the EIP28 device
#ifdef DRIVER_PKA_VECTOR_MAX_WORDS
#define EIP28_VECTOR_BITS_MAX       (DRIVER_PKA_VECTOR_MAX_WORDS * 32)
#endif

// Enable the Montgomery Ladder feature.
// Other values still use the ModExpVar method using the programmed number of odd powers.
// Can only be used on EIP-28A and EIP-28B configurations with at least HW2.2/FW3.2.
#ifdef DRIVER_EIP28_MONTGOMERY_LADDER_OPTION
#define EIP28_MONTGOMERY_LADDER_OPTION
#endif

/* use the options below to selectively remove unused features */

#ifdef DRIVER_EIP28_REMOVE_FIRMWARE_DOWNLOAD
#define EIP28_REMOVE_FIRMWARE_DOWNLOAD
#endif

#ifdef DRIVER_PKA_REMOVE_ADD
#define EIP28_REMOVE_ADD
#endif
#ifdef DRIVER_PKA_REMOVE_SUBTRACT
#define EIP28_REMOVE_SUBTRACT
#endif
#ifdef DRIVER_PKA_REMOVE_SUBADD
#define EIP28_REMOVE_SUBADD
#endif
#ifdef DRIVER_PKA_REMOVE_SHIFTRIGHT
#define EIP28_REMOVE_SHIFTRIGHT
#endif
#ifdef DRIVER_PKA_REMOVE_SHIFTLEFT
#define EIP28_REMOVE_SHIFTLEFT
#endif
#ifdef DRIVER_PKA_REMOVE_MULTIPLY
#define EIP28_REMOVE_MULTIPLY
#endif

#define EIP28_REMOVE_BIGUINT_ZP_HELPER_FUNC
#define EIP28_REMOVE_MODULO_OR_COPY

#ifdef ADPATER_PKA_REMOVE_DIVIDE
#define EIP28_REMOVE_DIVIDE
#endif
#ifdef DRIVER_PKA_REMOVE_COPY
#define EIP28_REMOVE_COPY
#endif
#ifdef DRIVER_PKA_REMOVE_COMPARE
#define EIP28_REMOVE_COMPARE
#endif
#ifdef DRIVER_PKA_REMOVE_MODINV
#define EIP28_REMOVE_MODINV
#endif
#ifdef DRIVER_PKA_REMOVE_MODEXP
#define EIP28_REMOVE_MODEXP
#endif
#ifdef DRIVER_PKA_REMOVE_MODEXPCRT
#define EIP28_REMOVE_MODEXPCRT
#endif
#ifdef DRIVER_PKA_REMOVE_ECCADD
#define EIP28_REMOVE_ECCADD
#endif
#ifdef DRIVER_PKA_REMOVE_ECCMUL
#define EIP28_REMOVE_ECCMUL
#endif
#ifdef DRIVER_PKA_REMOVE_ECCMULMONT
#define EIP28_REMOVE_ECCMULMONT
#endif
#ifdef DRIVER_PKA_REMOVE_DSASIGN
#define EIP28_REMOVE_DSASIGN
#endif
#ifdef DRIVER_PKA_REMOVE_DSAVERIFY
#define EIP28_REMOVE_DSAVERIFY
#endif
#ifdef DRIVER_PKA_REMOVE_ECCDSASIGN
#define EIP28_REMOVE_ECCDSASIGN
#endif
#ifdef DRIVER_PKA_REMOVE_ECCDSAVERIFY
#define EIP28_REMOVE_ECCDSAVERIFY
#endif


#endif /* Include Guard */


/* end of file cs_eip28.h */
