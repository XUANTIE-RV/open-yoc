/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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
#ifndef _FSL_SDMMC_HOST_H
#define _FSL_SDMMC_HOST_H

#include "drv/sdif.h"
#include <dw_mmc.h>

#define FSL_FEATURE_SOC_SDIF_COUNT	2

/* \brief sdif transfer function. */
typedef int32_t (*sdif_transfer_function_t)(sdif_handle_t handle, sdif_transfer_t *content);

/* brief sdif host descriptor */
typedef struct {
    sdif_handle_t               base;                ///< sdif peripheral handle
    uint32_t                    source_clock_hz;     ///< sdif source clock frequency united in Hz
    sdif_config_t               config;              ///< sdif configuration
    sdif_transfer_function_t    transfer;            ///< sdif transfer function
    sdif_capability_t           capability;          ///< sdif capability information
} sdif_host_t;

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Common definition for support and not support macro */
#define SDMMCHOST_NOT_SUPPORT 0U /*!< use this define to indicate the host not support feature*/
#define SDMMCHOST_SUPPORT 1U     /*!< use this define to indicate the host support feature*/

/* Common definition for board support SDR104/HS200/HS400 frequency */
/* SDR104 mode freq */
#define SDMMCHOST_SUPPORT_SDR104_FREQ SD_CLOCK_208MHZ
/* HS200 mode freq */
#define SDMMCHOST_SUPPORT_HS200_FREQ MMC_CLOCK_HS200
/* HS400 mode freq */
#define SDMMCHOST_SUPPORT_HS400_FREQ MMC_CLOCK_HS400

/* Common definition for SDMMCHOST transfer complete timeout */
#define SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT (1000U)
/* Common definition for card detect timeout */
#define SDMMCHOST_CARD_DETECT_TIMEOUT (~0U)

/*********************************************************SDHC**********************************************************/
#if(defined(FSL_FEATURE_SOC_SDIF_COUNT) && (FSL_FEATURE_SOC_SDIF_COUNT > 0U))

/*define host baseaddr ,clk freq, IRQ number*/
#define SDMMCHOST_TYPE sdif_handle_t
#define SDMMCHOST_CONFIG sdif_host_t
#define SDMMCHOST_TRANSFER sdif_transfer_t
#define SDMMCHOST_COMMAND sdif_command_t
#define SDMMCHOST_DATA sdif_data_t
#define SDMMCHOST_BUS_WIDTH_TYPE sdif_bus_width_e
#define SDMMCHOST_CAPABILITY sdif_capability_t
#define SDMMCHOST_BOOT_CONFIG void
#define SDMMCHOST_TRANSFER_FUNCTION sdif_transfer_function_t

//#define CARD_DATA0_STATUS_MASK SDIF_STATUS_DATA_BUSY_MASK
#define CARD_DATA0_STATUS_MASK  (0x200U)
#define CARD_DATA0_NOT_BUSY 0U

#define CARD_DATA1_STATUS_MASK (0U)
#define CARD_DATA2_STATUS_MASK (0U)
#define CARD_DATA3_STATUS_MASK (0U)

#define kSDMMCHOST_DATABUSWIDTH1BIT SDIF_BUS_1BIT_WIDTH /*!< 1-bit mode */
#define kSDMMCHOST_DATABUSWIDTH4BIT SDIF_BUS_4BIT_WIDTH /*!< 4-bit mode */
#define kSDMMCHOST_DATABUSWIDTH8BIT SDIF_BUS_8BIT_WIDTH /*!< 8-bit mode */

#define SDMMCHOST_STANDARD_TUNING_START (0U) /*!< standard tuning start point */
#define SDMMCHOST_TUINIG_STEP (1U)           /*!< standard tuning step */
#define SDMMCHOST_RETUNING_TIMER_COUNT (4U)  /*!< Re-tuning timer */
#define SDMMCHOST_TUNING_DELAY_MAX (0x7FU)
#define SDMMCHOST_RETUNING_REQUEST (1U)
#define SDMMCHOST_TUNING_ERROR (2U)

/* function pointer define */
#define GET_SDMMCHOST_CAPABILITY(base, capability) (drv_sdif_get_capabilities(base, capability))
#define GET_SDMMCHOST_STATUS(base) (SDIF_GetControllerStatus(base))
#define SDMMCHOST_SET_CARD_CLOCK(base, sourceClock_HZ, busClock_HZ) (drv_sdif_set_clock(base, busClock_HZ))
#define SDMMCHOST_SET_CARD_BUS_WIDTH(base, busWidth) (drv_sdif_bus_width(base, busWidth))
#define SDMMCHOST_SEND_CARD_ACTIVE(base, timeout) (SDIF_SendCardActive(base, timeout))
#define SDMMCHOST_ENABLE_CARD_CLOCK(base, enable) (SDIF_EnableCardClock(base, enable))

#define SDMMCHOST_CARD_DETECT_STATUS()
#define SDMMCHOST_CARD_DETECT_INIT()
#define SDMMCHOST_CARD_DETECT_INTERRUPT_STATUS()
#define SDMMCHOST_CARD_DETECT_INTERRUPT_CLEAR(flag)
#define SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_HANDLER
#define SDMMCHOST_CARD_DETECT_IRQ
/* define card detect pin voltage level when card inserted */
#define SDMMCHOST_CARD_INSERT_CD_LEVEL (0U)

/* sd card detect through host CD */
#define SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base) (SDIF_EnableInterrupt(base, kSDIF_CardDetect))
#define SDMMCHOST_CARD_DETECT_INSERT_STATUS(base, data3) (SDIF_DetectCardInsert(base, data3))

#define SDMMCHOST_SWITCH_VOLTAGE180V(base, enable18v)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, flag)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_STATUS(base) (0U)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_RESULT(base) (1U)
#define SDMMCHOST_CONFIG_SD_IO(speed, strength)
#define SDMMCHOST_ENABLE_DDR_MODE(base, flag, nibblePos)
#define SDMMCHOST_FORCE_SDCLOCK_ON(base, enable)
#define SDMMCHOST_RESET_TUNING(base, timeout)
#define SDMMCHOST_CHECK_TUNING_ERROR(base) (0U)
#define SDMMCHOST_ADJUST_TUNING_DELAY(base, delay)
#define SDMMCHOST_AUTO_STANDARD_RETUNING_TIMER(base)
#define SDMMCHOST_ENABLE_TUNING_FLAG(data)
#define SDMMCHOST_ENABLE_MMC_POWER(flag)
#define SDMMCHOST_SWITCH_VCC_TO_330V()
#define SDMMCHOST_SWITCH_VCC_TO_180V()
#define SDMMCHOST_SWITCH_VOLTAGE120V(base, flag)
#define SDMMCHOST_CONFIG_MMC_IO(hz, bus_s)
#define SDMMCHOST_ENABLE_HS400_MODE(base, flag)
#define SDMMCHOST_CONFIG_STROBE_DLL(base, target, dd_update)
#define SDMMCHOST_ENABLE_STROBE_DLL(base, flag)
#define SDMMCHOST_SETMMCBOOTCONFIG(base, config)
#define SDMMCHOST_ENABLE_MMC_BOOT(base, flag)

/*! @brief SDIF host capability*/
enum _host_capability {
    kSDMMCHOST_SupportHighSpeed = SDIF_SUPPORT_HIGH_SPEED,
    kSDMMCHOST_SupportDma = SDIF_SUPPORT_DMA_SPEED,
    kSDMMCHOST_SupportSuspendResume = SDIF_SUPPORT_USPEND_RESUME,
    kSDMMCHOST_SupportV330 = SDIF_SUPPORT_V330,
    kSDMMCHOST_SupportV300 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportV180 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportV120 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_Support4BitBusWidth = SDIF_SUPPORT_4BIT,
    kSDMMCHOST_Support8BitBusWidth = SDIF_SUPPORT_8BIT,
    kSDMMCHOST_SupportDDR50 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportSDR104 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportSDR50 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportHS200 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportHS400 = SDMMCHOST_NOT_SUPPORT,

};

/*! @brief DMA table length united as word
 * One dma table item occupy four words which can transfer maximum 2*8188 bytes in dual DMA mode
 * and 8188 bytes in chain mode
 * The more data to be transferred in one time, the bigger value of SDHC_ADMA_TABLE_WORDS need to be set.
 * user need check the DMA descriptor table lenght if bigger enough.
 */
#define SDIF_DMA_TABLE_WORDS (0x40U)
/* address align */
#define SDMMCHOST_DMA_BUFFER_ADDR_ALIGN (4U)

/*********************************************************USDHC**********************************************************/
#endif /* (defined(FSL_FEATURE_SOC_SDHC_COUNT) && (FSL_FEATURE_SOC_SDHC_COUNT > 0U)) */

/*! @brief card detect callback definition */
typedef void (*sdmmchost_cd_callback_t)(bool isInserted, void *user_data);

typedef void (*sdmmchost_interrupt_t)(int idx, void *user_data);

/*! @brief host Endian mode
* corresponding to driver define
*/
enum _sdmmchost_endian_mode {
    kSDMMCHOST_EndianModeBig = 0U,         /*!< Big endian mode */
    kSDMMCHOST_EndianModeHalfWordBig = 1U, /*!< Half word big endian mode */
    kSDMMCHOST_EndianModeLittle = 2U,      /*!< Little endian mode */
};

/*! @brief sd card detect type */
typedef enum _sdmmchost_detect_card_type {
    kSDMMCHOST_DetectCardByGpioCD,    /*!< sd card detect by CD pin through GPIO */
    kSDMMCHOST_DetectCardByHostCD,    /*!< sd card detect by CD pin through host */
    kSDMMCHOST_DetectCardByHostDATA3, /*!< sd card detect by DAT3 pin through host */
} sdmmchost_detect_card_type_t;

/*! @brief sd card detect */
typedef struct _sdmmchost_detect_card {
    sdmmchost_detect_card_type_t cdType; /*!< card detect type */
    uint32_t cdTimeOut_ms; /*!< card detect timeout which allow 0 - 0xFFFFFFF, value 0 will return immediately, value
          0xFFFFFFFF will block until card is insert */

    sdmmchost_cd_callback_t card_inserted; /*!< card inserted callback which is meaningful for interrupt case */
    sdmmchost_cd_callback_t card_removed;  /*!< card removed callback which is meaningful for interrupt case */

    void *user_data; /*!< user data */
} sdmmchost_detect_card_t;

/*! @brief card power control function pointer */
typedef void (*sdmmchost_pwr_t)(void);

/*! @brief card power control */
typedef struct _sdmmchost_pwr_card {
    sdmmchost_pwr_t powerOn;  /*!< power on function pointer */
    uint32_t powerOnDelay_ms; /*!< power on delay */

    sdmmchost_pwr_t powerOff;  /*!< power off function pointer */
    uint32_t powerOffDelay_ms; /*!< power off delay */
} sdmmchost_pwr_card_t;


/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name adaptor function
 * @{
 */

/*!
 * @brief host not support function, this function is used for host not support feature
 * @param  void parameter ,used to avoid build warning
 * @retval kStatus_Fail ,host do not suppport
 */
static inline status_t SDMMCHOST_NotSupport(void *parameter)
{
    parameter = parameter;
    return kStatus_Success;
}

/*!
 * @brief Detect card insert, only need for SD cases.
 * @param base the pointer to host base address
 * @param cd card detect configuration
 * @param waitCardStatus status which user want to wait
 * @retval kStatus_Success detect card insert
 * @retval kStatus_Fail card insert event fail
 */
status_t SDMMCHOST_WaitCardDetectStatus(SDMMCHOST_TYPE hostBase,
                                        const sdmmchost_detect_card_t *cd,
                                        bool waitCardStatus);

/*!
 * @brief check card is present or not.
 * @retval true card is present
 * @retval false card is not present
 */
bool SDMMCHOST_IsCardPresent(SDMMCHOST_TYPE base);

/*!
 * @brief Init host controller.
 * @param host the pointer to host structure in card structure.
 * @param user_data specific user data
 * @retval kStatus_Success host init success
 * @retval kStatus_Fail event fail
 */
status_t SDMMCHOST_Init(SDMMCHOST_CONFIG *host, void *user_data);

/*!
 * @brief reset host controller.
 * @param host base address.
 */
void SDMMCHOST_Reset(SDMMCHOST_TYPE base);

/*!
 * @brief host controller error recovery.
 * @param host base address.
 */
void SDMMCHOST_ErrorRecovery(SDMMCHOST_TYPE base);

/*!
 * @brief Deinit host controller.
 * @param host the pointer to host structure in card structure.
 */
void SDMMCHOST_Deinit(void *host);

/*!
 * @brief host power off card function.
 * @param base host base address.
 * @param pwr depend on user define power configuration.
 */
void SDMMCHOST_PowerOffCard(SDMMCHOST_TYPE base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief host power on card function.
 * @param base host base address.
 * @param pwr depend on user define power configuration.
 */
void SDMMCHOST_PowerOnCard(SDMMCHOST_TYPE base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief SDMMC host delay function.
 * @param milliseconds delay counter.
 */
void SDMMCHOST_Delay(uint32_t milliseconds);

void SDMMCHOST_BindingSDIF(SDMMCHOST_CONFIG *host, uint32_t sdif);
/* @} */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_SDMMC_HOST_H */
