/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_init.h
 *
 * EIP76 Driver Library Public Interface: Initialization API
 *
 * Refer to the EIP76 Driver Library User Guide for information about
 * re-entrance and usage from concurrent execution contexts of this API
 */


#ifndef INCLUDE_GUARD_EIP76_INIT_H
#define INCLUDE_GUARD_EIP76_INIT_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint8_t

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t

// EIP-76 Driver Library Types API
#include "eip76_types.h"        // EIP76_* types


/*----------------------------------------------------------------------------
 * EIP76_Initialize
 *
 * Configures the TRNG device including its post-processor if present and
 * enables all the configured FRO's.
 *
 * IOArea_p (output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Device (input)
 *     Handle for the device instance as returned by Device_Find.
 *
 * PS_Data_p (input, use only for the SP 800-90 Post Processor)
 *     Pointer to the array of 32-bit words
 *     where the Personalization String data is stored that must be used for
 *     the SP 800-90 PostProcessor.
 *
 *     N is between 0 and 11 inclusively
 *     PS_Data_p[N]: Bits [(N*32+31):(N*32)] "Personalization String"
 *
 * PS_WordCount (input, use only for the SP 800-90 Post Processor)
 *     Number of words in the PS_Data_p buffer that must be written,
 *     must be between 1 and 12 inclusively
 *
 * Return value:
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Operation is in progress
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Initialize(
        EIP76_IOArea_t * const IOArea_p,
        const Device_Handle_t Device,
        const uint32_t * PS_Data_p,
        const unsigned int PS_WordCount);


/*----------------------------------------------------------------------------
 * EIP76_Initialize_IsReady
 *
 * Checks if the operation started by the EIP76_Initialize() function
 * is completed. This function may only be called when EIP76_Initialize()
 * returns EIP76_BUSY_RETRY_LATER.
 *
 * Note: this function should only be used when the SP80090 post-processor
 *       is configured with the BCDF function.
 *
 * IOArea_p (output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * PS_Nonce_Data_p (input)
 *     Pointer to the array of 32-bit words where the concatenation of
 *     a Personalization String and Nonce data is stored.
 *
 *     N is between 0 and 11 inclusively
 *     PS_Nonce_Data_p[N]: Bits [(N*32+31):(N*32)] Personalization String and
 *     Nonce.
 *
 *     The first byte of the Nonce must be written to bits [31:24]
 *     of PS_Nonce_Data_p[0] while the last byte of the Personalization String
 *     must be written to bits [7:0] of PS_Nonce_Data_p[11].
 *
 *     Using a Personalization String is not a hard requirement. If it is
 *     not used, then the Nonce must be 48 bytes long to fill PS_Nonce_Data_p.
 *     All of the 48 Bytes must be used.
 *
 * PS_Nonce_WordCount (input)
 *     Number of words in the PS_Nonce_Data_p buffer that must be written,
 *     must be 12.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value:
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Operation is in progress, retry later
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Initialize_IsReady(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * PS_Nonce_Data_p,
        const unsigned int PS_Nonce_WordCount,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Shutdown
 *
 * Puts the TRNG device in the reset state, e.g. stops the device.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Return value
 *     EIP76_NO_ERROR : operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Shutdown(
        EIP76_IOArea_t * const IOArea_p);


/*----------------------------------------------------------------------------
 * EIP76_HWRevision_Get
 *
 * This function returns hardware revision information in the Capabilities_p
 * data structure.
 *
 * IOArea_p (input)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Capabilities_p (output)
 *     Pointer to the place holder in memory where the device capability
 *     information will be stored.
 *
 * Return value
 *     EIP76_NO_ERROR : operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 */
EIP76_Status_t
EIP76_HWRevision_Get(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_Capabilities_t * const Capabilities_p);


#endif /* INCLUDE_GUARD_EIP76_INIT_H */

/* end of file eip76_init.h */
