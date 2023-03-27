/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_read.h
 *
 * EIP76 Driver Library Public Interface: Read Random Number API
 *
 * Refer to the EIP76 Driver Library User Guide for information about
 * re-entrance and usage from concurrent execution contexts of this API
 */


#ifndef INCLUDE_GUARD_EIP76_READ_H
#define INCLUDE_GUARD_EIP76_READ_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"

// Driver Framework Device API
#include "device_types.h"       // Device_Handle_t

// EIP-76 Driver Library Types API
#include "eip76_types.h"        // EIP76_* types


/*----------------------------------------------------------------------------
 * EIP76_Request_Random_Data
 *
 * This function requests a number of random bytes. If it returns
 * EIP76_BUSY_RETRY_LATER then it should be called again until it returns
 * EIP76_NO_ERROR or some other error code.
 *
 * IOArea_p (input)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * NumberOfBytes (input)
 *     Number of random bytes requested to be generated.
 *
 * Return value
 *     EIP76_NO_ERROR : Random number request is accepted
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Device busy, retry later
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Request_Random_Data(
        EIP76_IOArea_t * const IOArea_p,
        unsigned int NumberOfBytes);


/*----------------------------------------------------------------------------
 * EIP76_Random_IsBusy
 *
 * This function indicates whether a random number can be read from the Device
 *
 * IOArea_p (input)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Random number is available
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Device is busy,
 *                              next random number is not yet available
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Random_IsBusy(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Random_Get
 *
 * This function tries to get the requested number of random integers.
 * The random number size is the same for every Post Processor type, e.g.
 * four 32-bit words.
 *
 * Returns EIP76_BUSY_RETRY_LATER when the random data is not available.
 * In this case simply call this function again, possibly using
 * EIP76_Random_IsBusy() first.
 *
 * Alternatively the "Ready" interrupt can be used for signaling available
 * random data.
 *
 * The caller should disable the "Ready" and "Fatal Error" interrupts
 * before calling this function, as they can be activated while this function
 * executes.
 *
 * The "Ready" interrupt may cause the CPU to jump to another execution context
 * which may also call this function. This may produce a race condition, e.g.
 * two execution contexts trying to read the same Device resource.
 *
 * The "Fatal Error" interrupt will automatically bring the Device into
 * the reset state which will discard the offered random data.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Data_p (output)
 *     Pointer to the array of the EIP76_Random128_t elements where
 *     the random data must be written.
 *
 * NumberCount (input)
 *     Requested number of random numbers, must not be larger than the Data_p
 *     array size. It must be specified either in the 128-bit words
 *
 * GeneratedNumberCount_p (output)
 *     Pointer to the memory location where the number of retrieved random
 *     numbers will be stored. When the reading random number fails this
 *     parameter returns 0 and the function returns EIP76_RANDOM_READ_ERROR.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
 *     EIP76_BUSY_RETRY_LATER : Random number is not available yet
 *     EIP76_RANDOM_READ_ERROR : failed to read the random number
 */
EIP76_Status_t
EIP76_Random_Get(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_Random128_t * Data_p,
        const unsigned int NumberCount,
        unsigned int * const GeneratedNumberCount_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Alarm_Handle
 *
 * This function checks and handles the alarm events (see EIP76_Events_t).
 *
 * This function performs the FRO de-tuning and
 * restarts the stopped FRO's. It also clears the alarm events,
 * if active. The number of the stopped FRO's before
 * the corresponding EIP76_SHUTDOWN_OFLO_ALARM alarm event is generated
 * is configurable.
 *
 * IOArea_p (input)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t). If fatal error
 *     events are detected then they will be also reported here but this
 *     function will not handle fatal error event, the EIP76_FatalError_Handle
 *     function must be used to handle fatal errors
 *
 * FROAlarmMask_p (output)
 *     Pointer to the place holder in memory where the accumulated FRO alarms
 *     will be stored. This number will be less or equal to the number of the
 *     stopped FRO's before the EIP76_SHUTDOWN_OFLO_ALARM alarm event
 *     is generated.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Alarm_Handle(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p,
        uint32_t * FROAlarmMask_p);


/*----------------------------------------------------------------------------
 * EIP76_FatalError_Handle
 *
 * This function checks and handles the fatal error events (see EIP76_Events_t).
 *
 * It is configurable whether the EIP76_SHUTDOWN_OFLO_ALARM alarm event
 * should be handled as a fatal error. The number of the stopped FRO's before
 * the EIP76_SHUTDOWN_OFLO_FATAL_ERROR fatal error is generated
 * is also configurable.
 *
 * This function brings the TRNG Device to the Reset state before it returns
 * if it detects the fatal error events.
 *
 * If the fatal error events are enabled as interrupts then the occurrence of
 * such an event will bring the Device automatically to the Reset state.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_FatalError_Handle(
        EIP76_IOArea_t * const IOArea_p);


#endif /* INCLUDE_GUARD_EIP76_READ_H */


/* end of file eip76_read.h */
