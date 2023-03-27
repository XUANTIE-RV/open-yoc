/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* sfxlpka.h
 *
 * This header declares the necessary definitions for using the public key
 * cryptography acceleration capabilities of the Security-IP PKA device.
 *
 * These API functions are blocking (synchronous), e.g. the functions
 * does not return until the device has done the operation or an error
 * is detected.
 *
 */

#ifndef SFXL_PKA_H_
#define SFXL_PKA_H_

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// ISO-C99
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

/* The following data structures describe the parameters as
   passed to the various functions in the driver.
*/

// Error codes
typedef enum {
        SFXL_PKA_NO_ERROR,               // Success
        SFXL_PKA_DEVICE_ERROR,           // PKA device reports an error
        SFXL_PKA_OUT_OF_SYSMEMORY_ERROR, // No system memory available
        SFXL_PKA_UNSUPPORTED_ERROR,      // Operation not supported
        SFXL_PKA_INTERNAL_ERROR,         // Internal implementation error
        SFXL_PKA_INPUT_ERROR,            // Invalid input parameter,
        SFXL_PKA_TIMEOUT_ERROR           // Timeout while waiting for result
} SfxlPKA_Error_t;

// Integer definition for Security-IP PKA device
typedef uint32_t SfxlPKA_Word_t;

typedef struct {
        // Pointer to the vector data (in SfxlPKA_Word_t words)
        // Data words must be in the CPU-native (Big or Little) endianness format
        SfxlPKA_Word_t *Word_p;

        // Vector word count
        unsigned int WordCount;

} SfxlPKA_Vector_t;

// ModExp command parameters:
// Result = Base ^ Exponent % Modulus
typedef struct {
        SfxlPKA_Vector_t Result; // output, size >= Modulus size + 1

        SfxlPKA_Vector_t Base;     // input
        SfxlPKA_Vector_t Exponent; // input
        SfxlPKA_Vector_t Modulus;  // input
        uint16_t         Extra;

} SfxlPKA_ModExp_Params_t;

// ModExpCRT command parameters:
// M1 = Base ^ ExponentP % (ModulusP - 1)
// M2 = Base ^ ExponentQ % (ModulusQ - 1)
// H  = (InverseQ * (M1 - M2)) % ModulusP
// Result = M2 + H * ExponentQ
typedef struct {
        SfxlPKA_Vector_t Result; // output, size >= ModulusP size * 2

        SfxlPKA_Vector_t Base;      // input
        SfxlPKA_Vector_t ExponentP; // input
        SfxlPKA_Vector_t ExponentQ; // input
        SfxlPKA_Vector_t ModulusP;  // input
        SfxlPKA_Vector_t ModulusQ;  // input
        SfxlPKA_Vector_t InverseQ;  // input

} SfxlPKA_ModExpCrt_Params_t;

/*----------------------------------------------------------------------------
 * Function pointer types
 *
 * Facilitate calling the API functions indirectly via pointers
 *
 */
typedef SfxlPKA_Error_t SfxlPKA_Device_Init_Func_t(const unsigned int DeviceID);

typedef SfxlPKA_Error_t
SfxlPKA_Device_Uninit_Func_t(const unsigned int DeviceID);

typedef SfxlPKA_Error_t
SfxlPKA_ModExp_Func_t(const unsigned int             DeviceID,
                      SfxlPKA_ModExp_Params_t *const Params_p);

typedef SfxlPKA_Error_t
SfxlPKA_ModExpCrt_Func_t(const unsigned int                DeviceID,
                         SfxlPKA_ModExpCrt_Params_t *const Params_p);

/*----------------------------------------------------------------------------
 * Local variables
 */

/*---------------------------------------------------------------------------
 * SfxlPKA_Device_Init
 *
 * Initialize the Security-IP PKA device.
 *
 * DeviceID (input)
 *      Security-IP device identifier.
 *
 * Return value
 *    SFXL_PKA_NO_ERROR: device is initialized successfully
 *    SFXL_PKA_INTERNAL_ERROR: device initialization failed
 *
 */
SfxlPKA_Error_t SfxlPKA_Device_Init();

/*---------------------------------------------------------------------------
 * SfxlPKA_Device_Uninit
 *
 * Un-initialize the Security-IP PKA device.
 *
 * DeviceID (input)
 *      Security-IP device identifier.
 *
 * Return value
 *    SFXL_PKA_NO_ERROR: device is initialized successfully
 *    SFXL_PKA_INTERNAL_ERROR: device initialization failed
 */
SfxlPKA_Error_t SfxlPKA_Device_Uninit();

/*---------------------------------------------------------------------------
 * SfxlPKA_ModExp
 *
 * Perform modexp operation on the Security-IP PKA device.
 *
 * DeviceID (input)
 *      Security-IP device identifier.
 *
 * Params_p (input)
 *      Pointer to memory location where the input parameters for
 *      the ModExp operation are stored.
 *
 * Return value
 *    SFXL_PKA_NO_ERROR: operation is performed successfully
 *    SFXL_PKA_UNSUPPORTED_ERROR: unsupported vector format or length
 *    SFXL_PKA_INTERNAL_ERROR: operation failed
 *
 */
SfxlPKA_Error_t SfxlPKA_ModExp(const unsigned int             DeviceID,
                               SfxlPKA_ModExp_Params_t *const Params_p);

/*---------------------------------------------------------------------------
 * SfxlPKA_ModExpCrt
 *
 * Perform modexpcrt operation on the Security-IP PKA device.
 *
 * DeviceID (input)
 *      Security-IP device identifier.
 *
 * Params_p (input)
 *      Pointer to memory location where the input parameters for
 *      the ModExpCrt operation are stored.
 *
 * Return value
 *    SFXL_PKA_NO_ERROR: operation is performed successfully
 *    SFXL_PKA_UNSUPPORTED_ERROR: unsupported vector format or length
 *    SFXL_PKA_INTERNAL_ERROR: operation failed
 *
 */
SfxlPKA_Error_t SfxlPKA_ModExpCrt(const unsigned int                DeviceID,
                                  SfxlPKA_ModExpCrt_Params_t *const Params_p);
void            SfxlPKALib_uSeconds_Wait(const unsigned int uSeconds);

#endif /* SFXL_PKA_H_ */

/* end of file sfxlpka.h */
