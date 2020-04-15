/*
 * The Clear BSD License
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FSL_COMMON_H_
#define _FSL_COMMON_H_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#if defined(__ICCARM__)
#include <stddef.h>
#include "silan_core_m0.h"
#endif

//#include "fsl_device_registers.h"

/*!
 * @addtogroup ksdk_common
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Construct a status code value from a group and code number. */
#define MAKE_STATUS(group, code) ((((group)*100) + (code)))

/*! @brief Construct the version number for drivers. */
#define MAKE_VERSION(major, minor, bugfix) (((major) << 16) | ((minor) << 8) | (bugfix))

/*! @name Driver version */
/*@{*/
/*! @brief common driver version 2.0.0. */
#define FSL_COMMON_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief Status group numbers. */
enum _status_groups {
    kStatusGroup_Generic = 0,                 /*!< Group number for generic status codes. */
    kStatusGroup_SDMMC = 18,                  /*!< Group number for SDMMC status code */
    kStatusGroup_SDIF = 59,                   /*!< Group number for SDIF status codes.*/
    kStatusGroup_ApplicationRangeStart = 101, /*!< Starting number for application groups. */
};

/*! @brief Generic status return codes. */
enum _generic_status {
    kStatus_Success = MAKE_STATUS(kStatusGroup_Generic, 0),
    kStatus_Fail = MAKE_STATUS(kStatusGroup_Generic, 1),
    kStatus_ReadOnly = MAKE_STATUS(kStatusGroup_Generic, 2),
    kStatus_OutOfRange = MAKE_STATUS(kStatusGroup_Generic, 3),
    kStatus_InvalidArgument = MAKE_STATUS(kStatusGroup_Generic, 4),
    kStatus_Timeout = MAKE_STATUS(kStatusGroup_Generic, 5),
    kStatus_NoTransferInProgress = MAKE_STATUS(kStatusGroup_Generic, 6),
};

/*! @brief Type used for all status and error return values. */
typedef int32_t status_t;

/*
 * Macro guard for whether to use default weak IRQ implementation in drivers
 */
#ifndef FSL_DRIVER_TRANSFER_DOUBLE_WEAK_IRQ
#define FSL_DRIVER_TRANSFER_DOUBLE_WEAK_IRQ 1
#endif

/*! @name Min/max macros */
/* @{ */
#if !defined(MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
/* @} */

/*! @brief Computes the number of elements in an array. */
#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/*! @name UINT16_MAX/UINT32_MAX value */
/* @{ */
#if !defined(UINT16_MAX)
#define UINT16_MAX ((uint16_t)-1)
#endif

#if !defined(UINT32_MAX)
#define UINT32_MAX ((uint32_t)-1)
#endif
/* @} */

/*! @name Alignment variable definition macros */
/* @{ */
#if (defined(__ICCARM__))
/**
 * Workaround to disable MISRA C message suppress warnings for IAR compiler.
 * http://supp.iar.com/Support/?note=24725
 */
_Pragma("diag_suppress=Pm120")
#define SDK_PRAGMA(x) _Pragma(#x)
_Pragma("diag_error=Pm120")
/*! Macro to define a variable with alignbytes alignment */
#define SDK_ALIGN(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var
#elif defined(__ARMCC_VERSION)
/*! Macro to define a variable with alignbytes alignment */
#define SDK_ALIGN(var, alignbytes) __align(alignbytes) var
#elif defined(__GNUC__)
/*! Macro to define a variable with alignbytes alignment */
//#define SDK_ALIGN(var, alignbytes) var __attribute__((aligned(alignbytes)))
#if defined(__XCC__)
#define SDK_ALIGN(var, alignbytes) var __attribute__((section(".dsp_dram")))
#endif
#if defined(__CSKY__)
#define SDK_ALIGN(var, alignbytes) var __attribute__((section(".sys_sram")))
#endif
#else
#error Toolchain not supported
#define SDK_ALIGN(var, alignbytes) var
#endif

/*! Macro to change a value to a given size aligned value */
#define SDK_SIZEALIGN(var, alignbytes) \
    ((unsigned int)((var) + ((alignbytes)-1)) & (unsigned int)(~(unsigned int)((alignbytes)-1)))
/* @} */

#endif /* _FSL_COMMON_H_ */
