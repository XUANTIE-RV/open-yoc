/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* c_sfxlpka.h
 *
 * Default configuration file for Security-IP PKA device integration module
 */



#ifndef C_SFXLPKA_H_
#define C_SFXLPKA_H_


/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */



/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Top-level configuration file
// #include "cs_sfxlpka.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Maximum number of simultaneously supported PKA devices
#ifndef SFXL_PKA_DEVICES_MAX_COUNT
#define SFXL_PKA_DEVICES_MAX_COUNT                  1
#endif

// Maximum number of simultaneous sessions per PKA device
#ifndef SFXL_PKA_SESSIONS_MAX_COUNT
#define SFXL_PKA_SESSIONS_MAX_COUNT                 1
#endif

// Use PKA device asynchronous notifications about operation completion
// when configured
//#define SFXL_PKA_NOTIFICATIONS_ENABLE

// Alignment for DMA-safe buffers allocated via DMABuf API
#ifndef SFXL_PKA_DMABUF_ALIGNMENT_BYTE_COUNT
#define SFXL_PKA_DMABUF_ALIGNMENT_BYTE_COUNT        4
#endif

// DMA buffer allocation memory bank
#ifndef SFXL_PKA_DMABUF_BANK
#define SFXL_PKA_DMABUF_BANK                        0
#endif

// Busy wait max attempts when using the PKA device in the polling mode
#ifndef SFXL_PKA_BUSYWAIT_COUNT
#define SFXL_PKA_BUSYWAIT_COUNT                     10000
#endif

// Default time period (in microseconds) between two consecutive attempts
// to read the result
#ifndef SFXL_PKA_DELAY_USEC
#define SFXL_PKA_DELAY_USEC                         100
#endif


// In general modular exponentiations should be performed with four (4)
// odd powers. This is a good compromise between needed PKA data RAM size and
// performance.
// Example:
//      1 odd power as baseline performance of 100%
//      2 odd powers delivers approximately 112% performance
//      4 odd powers delivers approximately 121% performance
//      8 odd powers delivers approximately 125% performance
// ModExp-variable uses minimum 1 odd power and maximum 16 odd powers.
//#ifndef SFXL_PKA_ODD_POWERS
//#define SFXL_PKA_ODD_POWERS                         8
/* othre */
//#define SFXL_PKA_ODD_POWERS                         4
/* rom */
#define SFXL_PKA_ODD_POWERS                         1
//#endif

// Strict input parameters (argument) checking is done when configured
//#define SFXL_PKA_STRICT_ARGS_CHECK


/* end of file c_sfxlpka.h */


#endif /* C_SFXLPKA_H_ */
