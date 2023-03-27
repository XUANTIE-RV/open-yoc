/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* api_pka.h
 *
 * Public Key Big Integer Math Acceleration Control API (PKA)
 */



#ifndef INCLUDE_GUARD_API_PKA_H
#define INCLUDE_GUARD_API_PKA_H

#include "basic_defs.h"
#include "api_dmabuf.h"

#define PKA_MAX_OPCODES   32
#define PKA_MAX_BUFFERS    6
#define PKA_MAXLEN_TEXT  300


/*----------------------------------------------------------------------------
 * PKA_Errors_t
 *
 * Error code Return Values for this API are enumerated by this type.
 */
typedef enum
{
    // all error codes are negative
    PKA_ERROR_NOT_IMPLEMENTED = -1,
    PKA_ERROR_INVALID_PARAMETER = -2,
    PKA_ERROR_INVALID_OPCODE = -3,
    PKA_ERROR_NO_SPACE = -4,
    PKA_ERROR_INTERNAL = -5,
    PKA_ERROR_INVALID_USE_ORDER = -6,
    PKA_ERROR_SCAP = -7,
    PKA_ERROR_RESET
} PKA_Errors_t;


/*----------------------------------------------------------------------------
 * PKA_Capabilities_t
 *
 * Vector_MaxBits
 *     Maximum length of a vector in bits.
 *
 * szTextDescription[]
 *     Zero-terminated descriptive text of the available services.
 *
 * OpCodesCount
 *     Number of opcodes in the OpCodes array.
 *
 * OpCodes[]
 *     Array of opcodes supported by the implementation.
 */
typedef struct
{
    unsigned int FirmwareCapabilities;
    unsigned int Vector_MaxBits;
    unsigned int LNME_Config;
    char szTextDescription[PKA_MAXLEN_TEXT];
    unsigned int OpCodesCount;
    unsigned int OpCodes[PKA_MAX_OPCODES];
} PKA_Capabilities_t;


/*----------------------------------------------------------------------------
 * PKA_Capabilities_Get
 *
 * This routine returns a structure that describes the capabilities of the
 * implementation. See description of PKA_Capabilities_t for details.
 *
 * Capabilities_p
 *     Pointer to the capabilities structure to fill in.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  Success
 *
 * This API function is re-entrant.
 */
int
PKA_Capabilities_Get(
        PKA_Capabilities_t * const Capabilities_p);


/*----------------------------------------------------------------------------
 * PKA_Session_t
 *
 * A session is used to keep applications separated in this API. Results will
 * be returned for the session on which the related command was given.
 * Each application must allocate a session and provide the reference in all
 * other calls.
 *
 * The session is really a void pointer that the application must remember. It
 * was wrapped in a struct to enable stronger type checking in the API.
 */
typedef struct
{
    void * p;
} PKA_Session_t;


/*----------------------------------------------------------------------------
 * PKA_Init
 *
 * This function must be used to initialize the service. No API function may
 * be used before this function has returned.
 *
 * InstanceNumber
 *     This number can be used by the implementation to select one of many
 *     internal engine instances or prioritized interfaces. The supported
 *     values are defined by implementation.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  Success. PKA is initialized.
 */
int
PKA_Init(
        const unsigned int InstanceNumber);


/*----------------------------------------------------------------------------
 * PKA_UnInit
 *
 * This call un-initializes the service. Use only when there are no pending
 * PKA operations. The caller must make sure that no other PKA API function
 * is used while this function executes.
 *
 * InstanceNumber
 *     This number can be used by the implementation to select one of many
 *     internal engine instances or prioritized interfaces. The supported
 *     values are defined by implementation.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  Success. PKA is uninitialized.
 */
int
PKA_UnInit(
        const unsigned int InstanceNumber);


/*----------------------------------------------------------------------------
 * PKA_Open
 *
 * This function must be used to create a session. The implementation requires
 * the session to buffer (until they are retrieved by the caller) results
 * related to commands posted on that same session.
 *
 * Session_p
 *     Pointer to the session variable that will be filled in by the function.
 *
 * InstanceNumber
 *     This number can be used by the implementation to select one of many
 *     internal engine instances or prioritized interfaces. The supported
 *     values are defined by each specific implementation.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  Success. A session was returned.
 *
 * This API function is re-entrant.
 */
int
PKA_Open(
        PKA_Session_t * const Session_p,
        const unsigned int InstanceNumber);


/*----------------------------------------------------------------------------
 * PKA_Close
 *
 * This function must be used to close a session previously opened by
 * PKA_Open. A session cannot be used anymore after it has been closed.
 * After closing a session, pending commands will no longer produce a result.
 *
 * Session
 *     Session value returned by PKA_Open.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  Success. The session was closed.
 *
 * This API function is re-entrant.
 */
int
PKA_Close(
        const PKA_Session_t Session);


/*----------------------------------------------------------------------------
 * PKA_Command_t
 *
 * Data structure to describe a command. The command consists of an OpCode
 * (the calculation to perform) and a series of operands (vectors to use in
 * the calculation).
 *
 * UserHandle
 *     This value will be returned in PKA_Result_t
 *     The caller can use this to match the result to a command
 *
 * OpCode
 *     The requested calculation to perform. The exact use of all the other
 *     parameters depends on this OpCode. This is documented separately and
 *     is specific for each implementation.
 *
 * A_Len
 * B_Len
 * Extra
 *     Three 16-bit values. The meaning depends on the OpCode. Typically,
 *     A_Len and B_Len will describe the length of the vector components and
 *     the Extra field is used to describe the Shift value, Odd Powers, etc.
 *
 * Encrypted
 *     This parameter is used to describe which of the input vector components
 *     requires decryption before it can be used. The exact format depends on
 *     the specific OpCode, but typically this would be a key reference and a
 *     bitmask for each vector component.
 *
 * Handles[]
 *     DMA Buffer Handles to the blocks of memory that are used as input or
 *     output for the operation. The exact number of handles and use is
 *     specific for each OpCode.
 *     The format of the vector is implementation specific, although typically
 *     it would be fixed to one format (little endian, which means the least
 *     significant part of the big integer must be first in the buffer) and
 *     either an array of bytes or array of 32bit words with clearly specified
 *     byte order within that 32bit word (bytes 0,1,2,3), which might be native
 *     to the device and not the host CPU.
 */
typedef struct
{
    void * UserHandle;
    int OpCode;

    uint16_t A_Len;
    uint16_t B_Len;
    uint16_t Extra;

    uint16_t Encrypted;

    CVI_DMABuf_Handle_t Handles[PKA_MAX_BUFFERS];
} PKA_Command_t;


/*----------------------------------------------------------------------------
 * PKA_CommandPut
 *
 * This function must be used to submit a command. When the function returns,
 * the command (provided by reference) has been copied and the are queued up
 * for later processing. The DMA buffers for which handles have been provided
 * must not be touched by the caller until the result for this command has
 * been received, or the session has been closed.
 *
 * Session
 *     Session value returned by PKA_Open.
 *
 * Command_p
 *     Pointer to the command descriptor block.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  The command was successfully queued up.
 */
int
PKA_CommandPut(
        const PKA_Session_t Session,
        const PKA_Command_t * const Command_p);


/*----------------------------------------------------------------------------
 * PKA_CommandPutMany
 *
 * This function has the same use as PKA_CommandPut, but accepts an array of
 * commands at once. It can be used to avoid many calls, but more importantly
 * it allows the caller to indicate that these commands should not be executed
 * in parallel because a dependency exists between them. Typically, the output
 * from one operation is used as the input for the next operation. If these
 * would be executed in parallel, the result would be wrong.
 *
 * This function will accept all the commands provided, or none. It will never
 * accept a fraction of the commands.
 *
 * Note that each command will generate a separate result.
 *
 * Session
 *     Session value returned by PKA_Open.
 *
 * fInOrder
 *     Set to true to avoid parallel processing of the commands in this block.
 *     This also means either ALL commands will be queued up, or NONE.
 *
 * CommandCount
 *     Number of commands pointed to by Commands_p.
 *
 * Commands_p
 *     Pointer to the array of command descriptor blocks.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  No command was queued up.
 *     >0  Up to CommandCount commands were queued up (1..CommandCount)
 *         (when fInOrder=true it cannot return < CommandCount)
 */
int
PKA_CommandPutMany(
        const PKA_Session_t Session,
        const bool fInOrder,
        const unsigned int CommandCount,
        const PKA_Command_t * Commands_p);


/*----------------------------------------------------------------------------
 * PKA_Result_t
 *
 * Data structure to describe a result. The result consists of an result code
 * and a series of result vectors. Every command will generate one result.
 *
 * UserHandle
 *     The same UserHandle that provided in the related PKA_Command_t
 *     The caller must use this to match the result to a command.
 *     (results can arrive out-of-order)
 *
 * Result
 *     The OpCode-specific result from the comparison operation.
 *     Used to indicate the comparison result (smaller, larger, equal),
 *     issues with inputs (not co-prime, point-at-infinity), etc.
 *
 * fIsZero
 *     Set to true when the result is zero.
 *     This value is not always provided (depends on the OpCode).
 *
 * MSW
 *     Location of the most significant word of the result vector.
 *     (also the number of significant words in the result vector, minus 1)
 *     This value is not always provided (depends on the OpCode).
 *
 * MSbit
 *     Most significant bit number in the most significant word.
 *     Can be used to calculate the number of significant bits in the result.
 *     This value is not always provided (depends on the OpCode).
 *
 * fIsZero2
 * MSW2
 *     Same as for fIsZero and MSW, but for the second result vector.
 *     These values are not always provided (depends on the OpCode).
 *     Only specific OpCodes with two output vectors would provide these
 *     parameters. Note: there is no MSbit2 parameter.
 */
typedef struct
{
    void * UserHandle;
    int Result;

    // main result (all optional!)
    bool fIsZero;
    uint16_t MSW;
    uint16_t MSbit;

    // secondary result (specific commands only)
    bool fIsZero2;
    uint16_t MSW2;

    // Compare result
    // For specific commands only, if not supported this value is 0 otherwise:
    // If 'A' == 'B', compare result = 0b001
    // Else if 'A' < 'B', compare result = 0b010
    // Else (if 'A' > 'B'), compare result = 0b100
    uint8_t CMPResult;
} PKA_Result_t;

/*----------------------------------------------------------------------------
 * PKA_ResultGet
 *
 * This function must be used to retrieve one result.
 *
 * Session
 *     Session value returned by PKA_Open.
 *
 * Result_p
 *     Pointer to the PKA_Result_t structure where the results will be
 *     written by the implementation.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  No result was available.
 *      1  Success. Result structure has been populated.
 *
 * This API function is re-entrant.
 */
int
PKA_ResultGet(
        const PKA_Session_t Session,
        PKA_Result_t * const Result_p,
        uint16_t ResultCode_Expected);


/*----------------------------------------------------------------------------
 * PKA_ResultNotifyFunction_t
 *
 * This type specifies the callback function prototype for the function
 * PKA_ResultNotify_Request. The notification will occur only once.
 *
 * NOTE: The exact context in which the callback function is invoked and the
 *       allowed actions in that callback are implementation specific. The
 *       intention is that all API functions can be used, except PKA_Close.
 *
 * Session
 *     Session value returned by PKA_Open.
 *
 * ResultCount
 *     Number of result descriptors that were ready to be retrieved using
 *     PKA_ResultGet at the time of making the callback.
 */
typedef void
(* PKA_ResultNotifyFunction_t)(
        const PKA_Session_t Session,
        const unsigned int ResultCount);


/*----------------------------------------------------------------------------
 * PKA_ResultNotify_Request
 *
 * This routine can be used to request a one-time notification of available
 * results. It is typically used after PKA_ResultGet returned zero results and
 * the thread does not want to poll for new results. The callback will be
 * invoked exactly once.
 *
 * Once new results are available, the implementation will invoke the callback
 * one time to notify the user of the available results.
 *
 * Session
 *     Session value returned by PKA_Open for which to register the
 *     notification request.
 *
 * CBFunc
 *     Address of the callback function.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  Success. The one-time notification request has been stored.
 *
 * This API function is re-entrant.
 */
int
PKA_ResultNotify_Request(
        const PKA_Session_t Session,
        PKA_ResultNotifyFunction_t CBFunc);

/*----------------------------------------------------------------------------
 * PKA_Statistics_t
 *
 * Data structure to describe the statistics from the ring.
 *
 * CommandsReadCount
 *     The number of command descriptors read from the Host ring.
 *
 * ResultsWrittenCount
 *     The number of result descriptors written to the Host ring.
*/
typedef struct
{
    uint16_t CommandsReadCount;
    uint16_t ResultsWrittenCount;
} PKA_Statistics_t;

/*----------------------------------------------------------------------------
 * PKA_Statistics_Get
 *
 * This routine reads and returns the statistics.
 *
 * Session
 *     Session value returned by PKA_Open for which to register the
 *     notification request.
 *
 * Statistics_p
 *     Address of the statistics structure.
 *
 * Return Value
 *     <0  Error (see list of PKA_ERROR_* codes)
 *      0  Success.
 *
 * This API function is re-entrant.
 */

int
PKA_Statistics_Get(
        const PKA_Session_t Session,
        PKA_Statistics_t * const Statistics_p);

#endif /* Include Guard */

/* end of file api_pka.h */
