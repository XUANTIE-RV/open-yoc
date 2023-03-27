/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_ic.h
 *
 * EIP76 Driver Library Public Interface: Interrupt Control API
 *
 * Refer to the EIP76 Driver Library User Guide for information about
 * re-entrance and usage from concurrent execution contexts of this API
 */


#ifndef INCLUDE_GUARD_EIP76_IC_H
#define INCLUDE_GUARD_EIP76_IC_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint32_t

// EIP-76 Driver Library Types API
#include "eip76_types.h"        // EIP76_* types


//Note1:
//  In the API functions to follow the second parameter sometimes is
//  const EIP76_INT_SourceBitmap_t WhichIntSources.
//  This is always a set of interrupt sources, for which some operation
//  has to be performed.
//  If an interrupt source is not included in the EIP76_INT_SourceBitmap_t
//  instance, then the operation will not be performed for this source
//  (corresponding bit is not changed in a HW register).

//Note2:
//  If not stated otherwise, all API functions mentioned below this note
//  are re-rentrant and can be called concurrently with other API functions
//  except EIP76_Initialize and EIP76_Shutdown functions.


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Bit-mask for a set of EIP76 interrupts
// This represents an 'OR'-ed combination of EIP76_InterruptSource_t values
typedef uint32_t EIP76_INT_SourceBitmap_t;

// EIP-76 interrupts
typedef enum
{
    // Random data is available
    EIP76_INT_READY =          EIP76_DATA_READY_EVENT,

    // Alarm (or if configured Fatal error) critical FRO shutdown interrupt
    EIP76_INT_SHUTDON_OFLO =   EIP76_SHUTDOWN_OFLO_EVENT,

    // Fatal error Post Processor interrupt
    EIP76_INT_STUCK_OUT =      EIP76_STUCK_OUT_EVENT,

    // Fatal error Noise Fail interrupt
    EIP76_INT_NOISE_FAIL =     EIP76_NOISE_FAIL_EVENT,

    // Run test fail interrupt
    EIP76_INT_RUN_FAIL =       EIP76_RUN_FAIL_EVENT,

    // Alarm long run test fail interrupt
    EIP76_INT_LONG_RUN_FAIL =  EIP76_LONG_RUN_FAIL_EVENT,

    // Poker test fail interrupt
    EIP76_INT_POKER_FAIL =     EIP76_POKER_FAIL_EVENT,

    // Monobit test fail interrupt
    EIP76_INT_MONOBIT_FAIL =   EIP76_MONOBIT_FAIL_EVENT,

    // Stuck NRBG fail interrupt
    EIP76_INT_STUCK_NRBG_FAIL = EIP76_STUCK_NRBG_EVENT,

    // Repetition fail interrupt
    EIP76_INT_REPCNT_FAIL = EIP76_REPCNT_FAIL_EVENT,

    // Adaptive proportion fail interrupt
    EIP76_INT_APROP_FAIL = EIP76_APROP_FAIL_EVENT
} EIP76_InterruptSource_t;


/*----------------------------------------------------------------------------
 * EIP76_INT_EnableSources
 *
 * This function enables requested interrupts.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * IntSources (input)
 *      Specifies which interrupt sources to enable:
 *      '0' in a certain bit - interrupt source remains unchanged
 *      '1' in a certain bit - interrupt source will be enabled
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 */
EIP76_Status_t
EIP76_INT_EnableSources(
        EIP76_IOArea_t * const IOArea_p,
        const EIP76_INT_SourceBitmap_t IntSources);


/*----------------------------------------------------------------------------
 * EIP76_INT_DisableSources
 *
 * This function disables requested interrupts.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * IntSources (input)
 *      Specifies which interrupt sources to disable:
 *      '0' in a certain bit - interrupt source remains unchanged
 *      '1' in a certain bit - interrupt source will be disabled
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 */
EIP76_Status_t
EIP76_INT_DisableSources(
        EIP76_IOArea_t * const IOArea_p,
        const EIP76_INT_SourceBitmap_t IntSources);


/*----------------------------------------------------------------------------
 * EIP76_INT_IsActive
 *
 * Reads the status of all interrupt sources.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * PendingIntSources_p (output)
 *      Pointer to the place holder in memory to store the bitmask of
 *      active interrupt sources.
 *      Specifies which interrupt sources are active:
 *      '0' in a certain bit - interrupt source is not pending (inactive)
 *      '1' in a certain bit - interrupt source is pending (active)
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 */
EIP76_Status_t
EIP76_INT_IsActive(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_INT_SourceBitmap_t * const PendingIntSources_p);


/*----------------------------------------------------------------------------
 * EIP76_INT_Acknowledge
 *
 * This function acknowledges requested interrupt sources.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * IntSources (input)
 *      Specifies which interrupt sources to acknowledge (to clear):
 *      '0' in a certain bit - interrupt source is not acknowledged
 *      '1' in a certain bit - interrupt source is acknowledged (cleared)
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 */
EIP76_Status_t
EIP76_INT_Acknowledge(
        EIP76_IOArea_t * const IOArea_p,
        const EIP76_INT_SourceBitmap_t IntSources);


#endif /* INCLUDE_GUARD_EIP76_IC_H */

/* end of file eip76_ic.h */
