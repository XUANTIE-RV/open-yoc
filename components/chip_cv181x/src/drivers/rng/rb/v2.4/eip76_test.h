/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_test.h
 *
 * EIP76 Driver Library Public Interface: Known-answer test (KAT) API
 *
 * Refer to the EIP76 Driver Library User Guide for information about
 * re-entrance and usage from concurrent execution contexts of this API
 */

#ifndef INCLUDE_GUARD_EIP76_TEST_H
#define INCLUDE_GUARD_EIP76_TEST_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint32_t

// EIP-76 Driver Library Types API
#include "eip76_types.h"        // EIP76_* types

// // EIP-76 Driver Library States
#include "eip76_fsm.h"          // EIP76_State_t

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// Supported known-answer test (KAT) types,
// only one test type can be active at a time
typedef enum
{
    EIP76_PRM_KAT_TYPE,
    EIP76_PRM_CONTINUOUS_KAT_TYPE,
    EIP76_SP80090_AES_CORE_KAT_TYPE,
    EIP76_SP80090_KAT_TYPE,
    EIP76_SP80090_BC_DF_KAT_TYPE,
    EIP76_SHA_KAT_TYPE,
    EIP76_SHA_RANDOM,
    EIP76_REPCOUNT_TYPE,
    EIP76_ADPRO_TYPE_64,
    EIP76_ADPRO_TYPE_4k,
    EIP76_ADPRO_TYPE_512
} EIP76_Test_Type_t;

// Clear intack register value
#define CLEAR_INTACK 0xE3FF

#define RESET_COUNTERS 0x00010001

#define SET_TEST_KNOW_NOISE 0x00000020

#define CLEAR_READY_BIT 0x00000001

#define CLEAR_TEST_READY_BIT 0x00000100


// Run known-answer test result, 20,000 bits = 625 32-bit words
typedef struct
{
    // Counter for single bit "runs" of value one bits " after 20,000 bits,
    // this counter should have a value in the range 2267 .. 2733 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 4095.
    uint32_t run_1_count_ones;
    // Counter for single bit "runs" of value zero bits " after 20,000 bits,
    // this counter should have a value in the range 2267 .. 2733 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 4095.
    uint32_t run_1_count_zeroes;
    // Counter for two bit "runs" of value one bits " after 20,000 bits,
    // this counter should have a value in the range 1079 .. 1421 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 2047.
    uint32_t run_2_count_ones;
    // Counter for two bit "runs" of value zero bits " after 20,000 bits,
    // this counter should have a value in the range 1079 .. 1421 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 2047.
    uint32_t run_2_count_zeroes;
    // Counter for three bit "runs" of value one bits " after 20,000 bits,
    // this counter should have a value in the range 502 .. 748 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 1023.
    uint32_t run_3_count_ones;
    // Counter for three bit "runs" of value zero bits " after 20,000 bits,
    // this counter should have a value in the range 502 .. 748 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 1023.
    uint32_t run_3_count_zeroes;
    // Counter for four bit "runs" of value one bits " after 20,000 bits,
    // this counter should have a value in the range 233 .. 402 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 511.
    uint32_t run_4_count_ones;
    // Counter for four bit "runs" of value zero bits " after 20,000 bits,
    // this counter should have a value in the range 233 .. 402 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 511.
    uint32_t run_4_count_zeroes;
    // Counter for five bit "runs" of value one bits " after 20,000 bits,
    // this counter should have a value in the range 90 .. 223 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 255.
    uint32_t run_5_count_ones;
    // Counter for five bit "runs" of value zero bits " after 20,000 bits,
    // this counter should have a value in the range 90 .. 223 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 255.
    uint32_t run_5_count_zeroes;
    // Counter for six and higher bit "runs" of value one bits " after 20,000
    // bits, this counter should have a value in the range 90 .. 233 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 255.
    uint32_t run_6_count_ones;
    // Counter for six and higher bit "runs" of value zero bits " after 20,000
    // bits, this counter should have a value in the range 90 .. 233 (inclusive)
    // to pass the "run test". This counter cannot increment past its maximum
    // value of 255.
    uint32_t run_6_count_zeroes;
} EIP76_RunKAT_Result_t;

// Poker known-answer test result
typedef struct
{
    uint8_t     poker_count_0; // Poker test counter for 4-bit value 0.
    uint8_t     poker_count_1; // Poker test counter for 4-bit value 1.
    uint8_t     poker_count_2; // Poker test counter for 4-bit value 2.
    uint8_t     poker_count_3; // Poker test counter for 4-bit value 3.
    uint8_t     poker_count_4; // Poker test counter for 4-bit value 4.
    uint8_t     poker_count_5; // Poker test counter for 4-bit value 5.
    uint8_t     poker_count_6; // Poker test counter for 4-bit value 6.
    uint8_t     poker_count_7; // Poker test counter for 4-bit value 7.
    uint8_t     poker_count_8; // Poker test counter for 4-bit value 8.
    uint8_t     poker_count_9; // Poker test counter for 4-bit value 9.
    uint8_t     poker_count_a; // Poker test counter for 4-bit value 0xA.
    uint8_t     poker_count_b; // Poker test counter for 4-bit value 0xB.
    uint8_t     poker_count_c; // Poker test counter for 4-bit value 0xC.
    uint8_t     poker_count_d; // Poker test counter for 4-bit value 0xD.
    uint8_t     poker_count_e; // Poker test counter for 4-bit value 0xE.
    uint8_t     poker_count_f; // Poker test counter for 4-bit value 0xF.
} EIP76_PokerKAT_Result_t;

#define REPCNT_TEST_BUSYWAIT_MAXLOOP      1000000


/*----------------------------------------------------------------------------
 * EIP76_Test_Start
 *
 * This function starts the known-answer test. Use the EIP76_Test_Status_Get()
 * function to check whether the test is in the ready state. Once the test is
 * in the ready state then the other test functions of this API can be called.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * TestType (input)
 *     Type of the test that must be started, see EIP76_Test_Type_t for details
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed, test is in the ready state
 *     EIP76_BUSY_RETRY_LATER : Operation is in progress, test is not in
 *                              the ready state yet. Use EIP76_Test_Status_Get()
 *                              to poll for the test status.
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_Test_Start(
        EIP76_IOArea_t * const IOArea_p,
        const EIP76_Test_Type_t TestType,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_Abort
 *
 * This function aborts the known-answer test. It will restore the state
 * existed at the time of calling the EIP76_Test_Start() function excluding
 * the interrupt state.
 *
 * CAUTION: Call this function after the test is started
 *          (via the EIP76_Test_Start() function) and before it is ended
 *
 * IOArea_p (input, output)
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
 *     EIP76_NO_ERROR : Test is aborted
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_Test_Abort(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_Status_Get
 *
 * This function indicates whether the test is in the ready state and the other
 * test functions of this API can be called.
 *
 * IOArea_p (input, output)
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
 *     EIP76_NO_ERROR : Operation is completed, test is in the ready state
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Operation is in progress, test is not in
 *                              the ready state yet.
 *     EIP76_MONOBIT_FAIL : Monobit (M) test failed,
 *                          use EIP76_Test_M_Result_Read() function to read
 *                          the test result. Poker/Run (PR) test cannot
 *                          continue.
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_Status_Get(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_PRM_Write
 *
 * This function writes the 32-bit input word for the Poker/Run/Monobit (PRM)
 * known-answer test. After calling this function ensure that the test is
 * in the ready state, e.g. either this function returns EIP76_NO_ERROR or
 * the EIP76_Test_Status_Get() function returns EIP76_NO_ERROR. Only
 * when the test is in the ready state this function can be called again to
 * write next 32-bit input word. The test requires 625 32-bit words to be
 * written before the test results can be read unless the continuous Poker
 * test is used.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * InputWord32 (input)
 *     32-bit word to write as input for the test.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed, PRM test is in the ready state
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Operation is in progress, PRM test is not in
 *                              the ready state yet. Use EIP76_Test_Status_Get()
 *                              to poll for the test status.
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_PRM_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t InputWord32,
        EIP76_EventStatus_t * const Events_p);

/*----------------------------------------------------------------------------
 * EIP76_Test_PRM_Write64
 *
 * This function writes the 64-bit input word for the Poker/Run/Monobit (PRM)
 * known-answer test. After calling this function ensure that the test is
 * in the ready state, e.g. either this function returns EIP76_NO_ERROR or
 * the EIP76_Test_Status_Get() function returns EIP76_NO_ERROR. Only
 * when the test is in the ready state this function can be called again to
 * write next 32-bit input word. The test requires 313 64-bit words to be
 * written before the test results can be read unless the continuous Poker
 * test is used.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * InputWord_First (input)
 *     32-bit word to write as input for the test (first half).
 *
 * InputWord_Second (input)
 *     32-bit word to write as input for the test (second half).
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed, PRM test is in the ready state
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Operation is in progress, PRM test is not in
 *                              the ready state yet. Use EIP76_Test_Status_Get()
 *                              to poll for the test status.
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_PRM_Write64(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t InputWord_First_p,
        const uint32_t InputWord_Second_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_CF_Block_Write
 *
 * This function writes the 64-bit input word for the Conditioner Function
 * known-answer test.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * InputWord_First (input)
 *     32-bit word to write as input for the test (first half).
 *
 * InputWord_Second (input)
 *     32-bit word to write as input for the test (second half).
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
 *     EIP76_BUSY_RETRY_LATER : Operation is in progress, Conditioner Function
 *                              test is not in
 *                              the ready state yet. Use EIP76_Test_Status_Get()
 *                              to poll for the test status.
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_CF_Block_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t InputWord_First_p,
        const uint32_t InputWord_Second_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_Input_Write
 *
 * This function writes the 64-bit input word for the Conditioner Function
 * known-answer test.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * SHA_Input_Data (input)
 *     SHA input data to write as input for the test.
 *
 * SHAVersion (input)
 *     SHA version (can be 1 or 2).
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed.
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_CF_Input_Write(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p,
        uint32_t *SHA_Input_Data,
        uint8_t const SHAVersion);


/*----------------------------------------------------------------------------
 * EIP76_Test_M_Result_Read
 *
 * This function reads the result of the Monobit (M) test when it fails.
 * This functions should be called only after the EIP76_Test_Status_Get()
 * function returns EIP76_MONOBIT_FAIL.
 *
 * The Monobit known-answer test result can be read only when the test fails.
 *
 * Note that if the Monobit (M) test fails then the Poker/Run (PR) test cannot
 * continue.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * MonobitCount_p (output)
 *     Pointer to the place holder in memory for the Monobit count value.
 *     Up/down counter monitoring "1" and "0" bits " after 625 32-bit words,
 *     this counter should have a value in the range 9310 .. 10690 (inclusive)
 *     to pass the Monobit test.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed, PRM test is finished
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_M_Result_Read(
        EIP76_IOArea_t * const IOArea_p,
        uint32_t * const MonobitCount_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_PR_Result_Read
 *
 * This function reads the result of the Poker/Run (PR) test.
 *
 * This functions should be called for the normal PR test after the 625 32-bit
 * words have been written and processed by the device, e.g.
 * the EIP76_Test_Status_Get() function returns EIP76_NO_ERROR.
 *
 * This functions should be called for the continuous PR test after N 32-bit
 * words have been written and processed by the device, e.g.
 * the EIP76_Test_Status_Get() function returns EIP76_NO_ERROR.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * RunKATResult_p (output)
 *     Pointer to the place holder in memory for the Run test result will
 *     be stored, see EIP76_RunKAT_Result_t for details.
 *
 * PokerKATResult_p (output)
 *     Pointer to the place holder in memory for the Poker test result will
 *     be stored, see EIP76_PokerKAT_Result_t for details.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_PR_Result_Read(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_RunKAT_Result_t * const RunKATResult_p,
        EIP76_PokerKAT_Result_t * const PokerKATResult_p,
        EIP76_EventStatus_t * const Events_p);

/*----------------------------------------------------------------------------
 * EIP76_Test_CF_Result_Read
 *
 * This function reads the result of the conditioner function test result.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * SHA_MD (output)
 *     Pointer to the place holder in memory for the conditioner function
 *     control vector
 *
 * SHAVersion (input)
 *     SHAVersion, holds the SHA version value used by the device
 *
 * Return value
 *     EIP76_NO_ERROR
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_CF_Read_Result(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p,
        uint32_t const * SHA_MD,
        uint8_t SHAVersion);

/*----------------------------------------------------------------------------
 * EIP76_Test_State_Block_Write
 *
 * This function writes the input block for the Repetition Count test.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 *
 * InputWord_First_p (input)
 *     Input parameter for the first half of the 64-bit input block.
 *
 * InputWord_Second_p (input)
 *     Input parameter for the Second half of the 64-bit input block.
 *
 * NewState (input)
 *     New state to transition to.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_Test_State_Block_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t InputWord_First_p,
        const uint32_t InputWord_Second_p,
        const EIP76_State_t NewState,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Get_Counter
 *
 * This function gets the counter values of the Repetition Count or Adaptive
 * Proportion test
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Return value
 *     Current device counter value
 */
uint32_t
EIP76_Get_Counters(
        EIP76_IOArea_t * const IOArea_p);


/*----------------------------------------------------------------------------
 * EIP76_Get_Values
 *
 * This function gets the sample values of the Repetition Count or Adaptive
 * Proportion test
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Return value
 *     Current device sample value
 */
uint32_t
EIP76_Get_Values(
        EIP76_IOArea_t * const IOArea_p);


/*----------------------------------------------------------------------------
 * EIP76_Get_REPCOUNT_GET_CUTOFF
 *
 * This function gets the cutoff value of the Repetition Count test
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Return value
 *     Repetition Count cutoff value
 */
uint32_t
EIP76_Test_REPCOUNT_GET_CUTOFF(
        EIP76_IOArea_t * const IOArea_p);

/*----------------------------------------------------------------------------
 * Device_SwapEndian32
 *
 * This function can be used to swap the byte order of a 32bit integer. The
 * implementation could use custom CPU instructions, if available.
 */
uint32_t
Device_SwapEndian(
        const uint32_t Value);

/*----------------------------------------------------------------------------
 * EIP76_Test_Rep_Count_Fail_Check
 *
 * This function checks if the Repetition Count cutoff value has been hit
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Return value
 *     EIP76_REPCNT_FAIL
 *     EIP76_NO_ERROR
 */
EIP76_Status_t
EIP76_Test_Rep_Count_Fail_Check(
        EIP76_IOArea_t * const IOArea_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_RepCnt_Adpro_Set_Counters
 *
 * This function sets the repetition count or adaptive proportion counters
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * uint32_t Adpro4kCnt (Input)
 *     Adaptive Proportion 4k counter
 *
 * uint32_t Adpro64Cnt (Input)
 *     Adaptive Proportion 64 byte counter
 *
 * uint32_t Repetition Count (Input)
 *     Repetition Count counter
 *
 */
void
EIP76_Test_RepCnt_Adpro_Set_Counters(
        EIP76_IOArea_t * const IOArea_p,
        uint32_t AdPro4kCnt,
        uint32_t AdPro64Cnt,
        uint32_t RepCnt);


/*----------------------------------------------------------------------------
 * EIP76_Test_CF_Status_Get
 *
 * This function checks if the Conditioner Function is done processing the
 * result
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_BUSY_RETRY_LATER
 *     EIP76_NO_ERROR
 */
EIP76_Status_t
EIP76_Test_CF_Status_Get(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_CF_NoneLastChain
 *
 * This function resets the conditioner function to process a new chain
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Return value
 *     EIP76_NO_ERROR
 */
EIP76_Status_t
EIP76_Test_CF_NonLastChain(EIP76_IOArea_t * const IOArea_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_Rep_Sample_Counter_Read
 *
 * This function reads the sample value and the counter value of the Repetition
 * Count test. And checks it with the control values.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * uint32_t ExpRepSample (Input)
 *     The control sample value
 *
 * uint32_t ExpRepCounter (Input)
 *     The counter sample value
 *
 * Return value
 *     Returns true of the test passes
 */
bool
EIP76_Test_Rep_Sample_Counter_Read(
        EIP76_IOArea_t * const IOArea_p,
        uint32_t ExpRepSample,
        uint32_t ExpRepCounter);

/*----------------------------------------------------------------------------
 * EIP76_Test_Adap_Sample_Counter_Read
 *
 * This function reads the sample value and the counter value of the Adaptive
 * Proportion test. And checks it with the control values.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * uint32_t ExpAdapSample (Input)
 *     The control sample value
 *
 * uint32_t AdproAdapCnt (Input)
 *     The counter sample value
 *
 *EIP76_Test_Type_t TestType (Input)
 *     Value of test variant type (64b or 4kB)
 *
 *Return value
 *   Returns true if the test passes.
 */
bool
EIP76_Test_Adap_Sample_Counter_Read(
        EIP76_IOArea_t * const IOArea_p,
        uint32_t ExpAdapSample,
        uint32_t ExpAdapCounter,
        const EIP76_Test_Type_t TestType);


/*----------------------------------------------------------------------------
 * EIP76_Test_Adap_Fail_Check
 *
 * This function checks if the Repetition Count cutoff value has been hit
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Return value
 *     EIP76_ADPRO_FAIL
 *     EIP76_NO_ERROR
 */
EIP76_Status_t
EIP76_Test_Adap_Fail_Check(
        EIP76_IOArea_t * const IOArea_p);

/*----------------------------------------------------------------------------
 * EIP76_Test_Is_Ready
 *
 * This function checks if ready bit is set
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * data_buf_p (output)
 *      Buffer to put random data read.
 *
 * Return value
 *     EIP76_BUSY_RETRY_LATER
 *     EIP76_NO_ERROR
 */
EIP76_Status_t
EIP76_Test_Is_Ready(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p,
        uint32_t * const data_buf_p);


/*----------------------------------------------------------------------------
 * EIP76_Test_Set_Reseed
 *
 * This function sets the resed bit
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     void
 */
void
EIP76_Test_Set_Reseed(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


#endif /* INCLUDE_GUARD_EIP76_TEST_H */

/* end of file eip76_test.h */
