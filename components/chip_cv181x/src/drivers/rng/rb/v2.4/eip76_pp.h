/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* eip76_pp.h
 *
 * EIP76 Driver Library Public Interface: Post Processor API
 *
 * Refer to the EIP76 Driver Library User Guide for information about
 * re-entrance and usage from concurrent execution contexts of this API
 */



#ifndef INCLUDE_GUARD_EIP76_PP_H
#define INCLUDE_GUARD_EIP76_PP_H

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework Basic Definitions API
#include "basic_defs.h"         // uint32_t

// EIP-76 Driver Library Types API
#include "eip76_types.h"        // EIP76_* types


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_BlockCount_Get
 *
 * This function reads the number of 128-bit blocks generated
 * by the Post Processor. This number is 0 when the Post Processor is disabled.
 * This number will be set to 0 after the Post Processor re-seed and can be
 * used to determine when to re-seed the Post Processor.
 *
 * IOArea_p (input)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * BlockCount_p (output)
 *     Pointer to the place holder in memory for the numbers of generated
 *     random blocks since the last re-seed operation.
 *
 * Return value
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_UNSUPPORTED_FEATURE_ERROR : Feature is not supported
*/
EIP76_Status_t
EIP76_PostProcessor_BlockCount_Get(
        EIP76_IOArea_t * const IOArea_p,
        uint32_t * const BlockCount_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_IsBusy
 *
 * This function returns the Post Processor state.
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
 *     EIP76_NO_ERROR : Operation is completed
 *     EIP76_BUSY_RETRY_LATER : Post Processor re-seed operation is in progress
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_IsBusy(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_Reseed_Start
 *
 * This function starts the re-seed operation for the X9.31 as well as
 * for the SP 800-90 Post Processor.
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
 *     EIP76_NO_ERROR : X9.31 Post Processor re-seed operation is completed or
 *                      SP 800-90 Post Processor re-seed operation is started
 *     EIP76_BUSY_RETRY_LATER : Post Processor re-seed operation is in progress
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_Reseed_Start(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_Reseed_Write
 *
 * This function can be used to write the Additional Input
 * String data in order to re-seed the SP 800-90 Post Processor.
 *
 * This function must NOT be used for the X9.31 Post Processor!
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * PS_AI_Data_p (input)
 *     Pointer to the array of 32-bit words
 *     where the PS or AI data is stored that must be used for
 *     the SP 800-90 PostProcessor.
 *
 *     N is between 0 and 11 inclusively
 *     PS_AI_Data_p[N]: Bits [(N*32+31):(N*32)] "Personalization String"
 *                      (Instantiate function) or "Additional Input" (Re-seed
 *                      function).
 *
 * PS_AI_WordCount (input)
 *     Number of words in the PS_AI_Data_p buffer that must be written,
 *     must be between 1 and 12 inclusively
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Post Processor operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_Reseed_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * PS_AI_Data_p,
        const unsigned int PS_AI_WordCount,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_NIST_Write
 *
 * This function can be used to write the input data for the standard NIST
 * known-answer test for the SP 800-90 Post Processor.
 *
 * This function must NOT be used for the X9.31 Post Processor!
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * PS_AI_Data_p (input)
 *     Pointer to the array of 32-bit words
 *     where the data is stored that must be used for
 *     the SP 800-90 PostProcessor.
 *
 *     N is between 0 and 11 inclusively
 *     PS_AI_Data_p[N]: Bits [(N*32+31):(N*32)] "Personalization String"
 *                      (Instantiate function) or "Additional Input" (Re-seed
 *                      function).
 *
 * PS_AI_WordCount (input)
 *     Number of words in the PS_AI_Data_p buffer that must be written,
 *     must be between 1 and 12 inclusively
 *
 * VectorType (input)
 *     Vector type for the standard NIST known-answer test. Must be set to
 *     a non-zero value for the 2nd Generate function vector.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Post Processor operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_NIST_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * PS_AI_Data_p,
        const unsigned int PS_AI_WordCount,
        const unsigned int VectorType,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_PS_AI_Write
 *
 * This function can be used to write the Personalization
 * String data for the SP 800-90 Post Processor to perform the Instantiate
 * function after the NIST known-answer test.
 *
 * This function must NOT be used for the X9.31 Post Processor!
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * PS_AI_Data_p (input)
 *     Pointer to the array of 32-bit words
 *     where the PS or AI data is stored that must be used for
 *     the SP 800-90 PostProcessor.
 *
 *     N is between 0 and 11 inclusively
 *     PS_AI_Data_p[N]: Bits [(N*32+31):(N*32)] "Personalization String"
 *                      (Instantiate function) or "Additional Input" (Re-seed
 *                      function).
 *
 * PS_AI_WordCount (input)
 *     Number of words in the PS_AI_Data_p buffer that must be written,
 *     must be between 1 and 12 inclusively
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Post Processor operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
*/
EIP76_Status_t
EIP76_PostProcessor_PS_AI_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * PS_AI_Data_p,
        const unsigned int PS_AI_WordCount,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_Key_Write
 *
 * This function can be used to write the Key data for the Post Processor.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Key_Data_p (input)
 *     Pointer to the array of 32-bit words
 *     where the Key data for the Post Processor is stored
 *
 *     When X9.31 Post Processor is configured N is between 0 and 5 inclusively
 *     When X9.31 AES-256 Post Processor is configured N is between 0 and 7
 *     inclusively
 *     Key_Data_p[N]: Bits [(N*32+31):(N*32)] of the AES-256 and 3-key 3-DES
 *                    keys. For 3-key 3-DES keys, this register holds the LSW
 *                    of the 1st key and bits [0], [8], [16] and [24] are don"t
 *                    care bits (normally used to hold byte parity).
 *     When X9.31 AES-256 Post Processor is configured N is between 6 and 7
 *     inclusively
 *     Key_Data_p[N]: Bits [(N*32+31):(N*32)] of an AES-256 key
 *
 * Return value
 *     EIP76_NO_ERROR : Post Processor operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
*/
EIP76_Status_t
EIP76_PostProcessor_Key_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * Key_Data_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_V_Write
 *
 * This function can be used to write the V data for the Post Processor.
 *
 * This function must NOT be used for the SP 800-90 Post Processor!
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * V_Data_p (input)
 *     Pointer to the array of 32-bit words
 *     where the V data for the Post Processor is stored
 *
 *     When X9.31 Post Processor is configured N is between 0 and 1 inclusively
 *     When X9.31 AES-256 Post Processor is configured N is between 0 and 3
 *     inclusively
 *     V_Data_p[N]: Bits [(N*32+31):(N*32)] of the AES-256 and 3-key 3-DES
 *                  Post Processing "V" value.
 *     When X9.31 AES-256 Post Processor is configured N is between 2 and 3
 *     inclusively
 *     V_Data_p[N]: Bits [(N*32+31):(N*32)] of the AES-256 Post Processing
 *                  "V" value
 *
 * Return value
 *     EIP76_NO_ERROR : Post Processor operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
*/
EIP76_Status_t
EIP76_PostProcessor_V_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * V_Data_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_Input_Write
 *
 * This function can be used to write the Input data for
 * the X9.31 (3-DES and AES) Post Processor.
 *
 * This function can also be used to write the Input data for
 * the SP 800-90 Post Processor for the AES-256 Core known-answer test only.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Input_Data_p (input)
 *     Pointer to the array of 32-bit words
 *     where the Input data is stored that must be used for
 *     the Post Processor.
 *
 *     When X9.31 3-DES Post Processor is configured N is between 0 and 1
 *     inclusively
 *     When X9.31 AES-256 Post Processor is configured N is between 0 and 3
 *     inclusively
 *     Input_Data_p[0]: least significant word of 128-bit word of test data
 *                      for AES-256 Post Processor testing OR least significant
 *                      word of 64-bit word of test data for 3-DES
 *                      Post Processor testing
 *     Input_Data_p[1]: second word of 128-bit word of test data for AES-256
 *                      post-processor testing OR most significant word
 *                      of 64-bit word of test data for 3-DES Post Processor
 *                      testing
 *     When X9.31 AES-256 Post Processor is configured N is between 2 and 3
 *     inclusively
 *     Input_Data_p[2]: third word of 128-bit word of test data for AES-256
 *                      Post Processor testing
 *     Input_Data_p[3]: most significant word of 128-bit word of test data for
 *                      AES-256 Post Processor testing
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Input data is written successfully
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_Input_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * Input_Data_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_Result_Read
 *
 * This function can be used to read the Post Processor test result data.
 *
 * This function can be used for the X9.31 (3-DES or AES) Post Processor
 * to read result of the known-answer test.
 *
 * This function can also be used for the SP 800-90 Post Processor to read
 * 1) result of the AES-256 Core known-answer test
 * 2) result of the NIST known-answer test on the complete Post Processor
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Output_Data_p (output)
 *     Pointer to the array of 32-bit words where the test result data
 *     will be stored received from the Post Processor.
 *
 *     When X9.31 3-DES Post Processor is configured N is between 0 and 1
 *     inclusively
 *     When X9.31 AES-256 Post Processor is configured N is between 0 and 3
 *     inclusively
 *     Output_Data_p[0]: least significant word of 128-bit word of test data
 *                       for AES-256 Post Processor testing OR least significant
 *                       word of 64-bit word of test data for 3-DES
 *                       Post Processor testing
 *     Output_Data_p[1]: second word of 128-bit word of test data for AES-256
 *                       post-processor testing OR most significant word
 *                       of 64-bit word of test data for 3-DES Post Processor
 *                       testing
 *     When X9.31 AES-256 Post Processor is configured N is between 2 and 3
 *     inclusively
 *     Output_Data_p[2]: third word of 128-bit word of test data for AES-256
 *                       Post Processor testing
 *     Output_Data_p[3]: most significant word of 128-bit word of test data for
 *                       AES-256 Post Processor testing
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Output data is read successfully
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_Result_Read(
        EIP76_IOArea_t * const IOArea_p,
        uint32_t * Output_Data_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_IsReady
 *
 * This function can be used to read the test_ready bit from STATUS register.
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
 *     EIP76_NO_ERROR : test_ready bit is active
 *     EIP76_BUSY_RETRY_LATER : test_ready bit is not active
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_IsReady(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_BCDF_PS_AI_Write
 *
 * This function can be used to write the Personalization String data
 * for the SP 800-90 Post BCDF Processor to perform the Instantiate
 * function after the NIST known-answer test.
 *
 * This function must NOT be used for the X9.31 Post Processor!
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * PS_AI_Data_p (input)
 *     Pointer to the array of 32-bit words
 *     where the PS or AI data is stored that must be used for
 *     the SP 800-90 BCDF PostProcessor.
 *
 *     N is between 0 and 11 inclusively
 *     PS_AI_Data_p[N]: Bits [(N*32+31):(N*32)] concatenation of Personalization
 *                      String and Nonce data (Instantiate function).
 *
 * PS_AI_WordCount (input)
 *     Number of words in the PS_AI_Data_p buffer that must be written,
 *     must be 12.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Post Processor operation is completed
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 */
EIP76_Status_t
EIP76_PostProcessor_BCDF_PS_AI_Write(
        EIP76_IOArea_t * const IOArea_p,
        const uint32_t * PS_AI_Data_p,
        const unsigned int PS_AI_WordCount,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_BCDF_Noise_Write
 *
 * This function writes one noise block data to the TRNG_RAW registers.
 * Each time this function is called one noise block is written
 * if EIP76_NO_ERROR is returned.
 *
 * Use the EIP76_PostProcessor_BCDF_Status_Get() function to check the status
 * of the operation. The EIP76_PostProcessor_BCDF_Noise_Write() must be called
 * repeatedly until the EIP76_PostProcessor_BCDF_Status_Get() function returns
 * EIP76_NO_ERROR.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Noise_Data_p (input)
 *     Pointer to Noise blocks data to write to TRNG_RAW_L/H registers
 *
 * Noise_Count (input)
 *     Number of noise to be written in total.
 *
 * Return value
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_NO_ERROR : All noise is written
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_BCDF_Noise_Write(
        EIP76_IOArea_t * const IOArea_p,
        uint32_t * const Noise_Data_p,
        const unsigned int Noise_Count);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_BCDF_Status_Get
 *
 * This function can be used to check the SP80090 BCDF test status. This
 * function must be used together with
 * the EIP76_PostProcessor_BCDF_Noise_Write() function.
 *
 * IOArea_p (input, output)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * Return value
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_PROCESSING : Not all Noise is written, more input data needed
 *     EIP76_BUSY_RETRY_LATER : Device busy processing one noise block
 *     EIP76_NO_ERROR : All noise is written and processed
 *     EIP76_ILLEGAL_IN_STATE
*/
EIP76_Status_t
EIP76_PostProcessor_BCDF_Status_Get(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_BCDF_Generate_Start
 *
 * This function starts random data Generate function for the SP80090 BCDF
 * post-processor test. If it returns EIP76_BUSY_RETRY_LATER then it should
 * be called again until it returns EIP76_NO_ERROR or some other error code.
 *
 * IOArea_p (input)
 *     Pointer to the place holder in memory for the IO Area.
 *
 * WordCount (input)
 *     Number of 32-bit random words requested to be generated.
 *
 * Events_p (output)
 *     Pointer to the place holder in memory where the bit-mask value for
 *     the detected events will be stored (see EIP76_Events_t).
 *     The EIP76_Alarm_Handle() function can be used to handle the alarm events.
 *     The EIP76_FatalError_Handle() function can be used to handle the fatal
 *     error events.
 *
 * Return value
 *     EIP76_NO_ERROR : Random number request is accepted
 *     EIP76_BUSY_RETRY_LATER : Device busy, retry later
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_ILLEGAL_IN_STATE
 */
EIP76_Status_t
EIP76_PostProcessor_BCDF_Generate_Start(
        EIP76_IOArea_t * const IOArea_p,
        const unsigned int WordCount,
        EIP76_EventStatus_t * const Events_p);


/*----------------------------------------------------------------------------
 * EIP76_PostProcessor_BCDF_Result_Read
 *
 * This function performs random data Generate functions for the SP80090 BCDF
 * post-processor test. It must be called repeatedly until it returns
 * EIP76_PROCESSING indicating all data is read for the 1st Generate function,
 * and afterwards EIP76_NO_ERROR indicating all data is read for the 2nd
 * Generate function.
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
 * Data_p (output)
 *     Pointer for storing the data
 *
 * Data_WordCount (input)
 *     Number of 32-bit data words to be read in total.
 *
 * Return value
 *     EIP76_ARGUMENT_ERROR : Passed wrong argument
 *     EIP76_BUSY_RETRY_LATER : Not all data is read
 *     EIP76_PROCESSING : 1st Generate function is done, requested 2nd Generate
 *     EIP76_NO_ERROR : Both Generate functions are done, all data is read
 */
EIP76_Status_t
EIP76_PostProcessor_BCDF_Result_Read(
        EIP76_IOArea_t * const IOArea_p,
        EIP76_EventStatus_t * const Events_p,
        uint32_t * const Data_p,
        const unsigned int Data_WordCount);


#endif /* INCLUDE_GUARD_EIP76_PP_H */


/* end of file eip76_pp.h */
