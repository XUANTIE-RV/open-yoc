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

#include <drv/sdif.h>
#include <stdbool.h>
#include <soc.h>
#include "sdmmc_host.h"
#include "sdmmc_event.h"

/*!
 * @brief SDMMCHOST detect card insert status by host controller.
 * @param base host base address.
 * @param data3 flag indicate use data3 to detect card or not.
 */
static void SDMMCHOST_DetectCardInsertByHost(uint32_t sdif, void *user_data);

/*!
 * @brief SDMMCHOST transfer function.
 * @param base host base address.
 * @param content transfer configurations.
 */
static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE base, SDMMCHOST_TRANSFER *content);

/*!
 * @brief card detect deinit function.
 */
static void SDMMCHOST_CardDetectDeinit(SDMMCHOST_TYPE base);

/*!
 * @brief card detect deinit function.
 * @param host base address.
 * @param host detect card configuration.
 */
static status_t SDMMCHOST_CardDetectInit(SDMMCHOST_TYPE base, const sdmmchost_detect_card_t *cd);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief DMA descriptor table. */
#if defined(__XCC__)
static uint32_t s_sdifDmaTable[4][SDIF_DMA_TABLE_WORDS] __attribute__((section(".dsp_dram")));
#endif
#if defined(__CSKY__)
uint32_t s_sdifDmaTable[4][SDIF_DMA_TABLE_WORDS] __attribute__((aligned(64)));
#endif
#if defined(__riscv)
uint32_t s_sdifDmaTable[4][SDIF_DMA_TABLE_WORDS] __attribute__((aligned(64)));
#endif

static volatile bool g_sdifTransferSuccessFlag[4] = {true, true, true, true};
/*! @brief Card detect flag. */
static volatile bool s_sdInsertedFlag[4] = {false, false, false, false};
static sdmmchost_interrupt_t g_interrupt_func[4];

static void SDMMCHOST_DetectCardInsertByHost(uint32_t sdif, void *user_data)
{
    s_sdInsertedFlag[sdif] = true;
    SDMMCEVENT_Notify(sdif, kSDMMCEVENT_CardDetect);

    /* application callback */
    if (user_data && ((sdmmchost_detect_card_t *)user_data)->card_inserted) {
        ((sdmmchost_detect_card_t *)user_data)->card_inserted(true, ((sdmmchost_detect_card_t *)user_data)->user_data);
    }
}

static void SDMMCHOST_DetectCardRemoveByHost(uint32_t sdif, void *user_data)
{
    s_sdInsertedFlag[sdif] = false;

    /* application callback */
    if (user_data && ((sdmmchost_detect_card_t *)user_data)->card_removed) {
        ((sdmmchost_detect_card_t *)user_data)->card_removed(false, ((sdmmchost_detect_card_t *)user_data)->user_data);
    }
}

/* Transfer complete callback function. */
static void SDMMCHOST_transfer_completeCallback(uint32_t sdif, void *handle, status_t status, void *user_data)
{
    /* receive the right status, notify the event */
    if (status == kStatus_Success) {
        g_sdifTransferSuccessFlag[sdif] = true;
    } else {
        g_sdifTransferSuccessFlag[sdif] = false;
    }

    SDMMCEVENT_Notify(sdif, kSDMMCEVENT_transfer_complete);
}

void SDMMCHOST_RegisterInterrupt(int idx, sdmmchost_interrupt_t interrupt_func)
{
    if (idx < ARRAY_SIZE(g_interrupt_func))
        g_interrupt_func[idx] = interrupt_func;
}

void SDMMCHOST_Enable_Interrupt(int idx)
{
    csi_sdif_enable_sdio_interrupt(csi_sdif_get_handle(idx));
}

void SDMMCHOST_Disable_Interrupt(int idx)
{
    csi_sdif_disable_sdio_interrupt(csi_sdif_get_handle(idx));
}

static void SDMMCHOST_Interrupt(uint32_t idx, void *user_data)
{
    /* application callback */
    if (g_interrupt_func[idx]) {
        g_interrupt_func[idx](idx, user_data);
    }
}

/* User defined transfer function. */
static status_t SDMMCHOST_TransferFunction(SDMMCHOST_TYPE base, SDMMCHOST_TRANSFER *content)
{
    uint32_t sdif = csi_sdif_get_idx(base);
    status_t error = kStatus_Success;
    sdif_dma_config_t dmaConfig;

    //memset(s_sdifDmaTable[sdif], 0, sizeof(s_sdifDmaTable[0]));
    memset(&dmaConfig, 0, sizeof(dmaConfig));

    /* user DMA mode transfer data */
    if (content->data != NULL) {
        dmaConfig.enable_fix_burst_len = false;
        dmaConfig.mode = SDIF_DUAL_DMA_MODE;
        dmaConfig.dma_des_buffer_start_addr = s_sdifDmaTable[sdif];
        dmaConfig.dma_des_buffer_len = SDIF_DMA_TABLE_WORDS;
        dmaConfig.dma_dws_skip_len = 0U;
    }

#ifdef DW_SDIO_NOSUPPORT_DMA
    // FIXME:
    SDIF_TransferBlocking(base, NULL, content);
#else

    do {
        error = csi_sdif_transfer(base, &dmaConfig, content);
    } while (error == kStatus_SDIF_SyncCmdTimeout);

    if ((error != kStatus_Success) ||
        (false == SDMMCEVENT_Wait(sdif, kSDMMCEVENT_transfer_complete, SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT)) ||
        (!g_sdifTransferSuccessFlag[sdif])) {
        error = kStatus_Fail;
    }

#endif

    SDMMCEVENT_Delete(sdif, kSDMMCEVENT_transfer_complete);

    return error;
}

static status_t SDMMCHOST_CardDetectInit(SDMMCHOST_TYPE base, const sdmmchost_detect_card_t *cd)
{
    uint32_t sdif = csi_sdif_get_idx(base);
    sdmmchost_detect_card_type_t cdType = kSDMMCHOST_DetectCardByHostCD;

    if (cd != NULL) {
        cdType = cd->cdType;
    }

    /* for interrupt case, only kSDMMCHOST_DetectCardByHostCD can generate interrupt, so implement it only */
    if (cdType != kSDMMCHOST_DetectCardByHostCD) {
        return kStatus_Fail;
    }

    if (!SDMMCEVENT_Create(sdif, kSDMMCEVENT_CardDetect)) {
        return kStatus_Fail;
    }

    /* enable the card detect interrupt */
    SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base);

    /* check if card is inserted */
    if (SDMMCHOST_CARD_DETECT_INSERT_STATUS(base, false)) {
        s_sdInsertedFlag[sdif] = true;

        /* application callback */
        if (cd && cd->card_inserted) {
            cd->card_inserted(true, cd->user_data);
        }
    }

    return kStatus_Success;
}

static void SDMMCHOST_CardDetectDeinit(SDMMCHOST_TYPE base)
{
    uint32_t sdif = csi_sdif_get_idx(base);

    SDMMCEVENT_Delete(sdif, kSDMMCEVENT_CardDetect);
    s_sdInsertedFlag[sdif] = false;
}

void SDMMCHOST_Delay(uint32_t milliseconds)
{
    SDMMCEVENT_Delay(milliseconds);
}

status_t SDMMCHOST_WaitCardDetectStatus(SDMMCHOST_TYPE base, const sdmmchost_detect_card_t *cd, bool waitCardStatus)
{
    uint32_t sdif = csi_sdif_get_idx(base);

    uint32_t timeout = SDMMCHOST_CARD_DETECT_TIMEOUT;

    if (cd != NULL) {
        timeout = cd->cdTimeOut_ms;
    }

    if (waitCardStatus != s_sdInsertedFlag[sdif]) {
        /* Wait card inserted. */
        do {
            if (!SDMMCEVENT_Wait(sdif, kSDMMCEVENT_CardDetect, timeout)) {
                return kStatus_Fail;
            }
        } while (waitCardStatus != s_sdInsertedFlag[sdif]);
    }

    return kStatus_Success;
}

bool SDMMCHOST_IsCardPresent(SDMMCHOST_TYPE base)
{
    uint32_t sdif = csi_sdif_get_idx(base);

    return s_sdInsertedFlag[sdif];
}

void SDMMCHOST_PowerOffCard(SDMMCHOST_TYPE base, const sdmmchost_pwr_card_t *pwr)
{
    if (pwr != NULL) {
        pwr->powerOff();
        SDMMCHOST_Delay(pwr->powerOffDelay_ms);
    } else {
#ifdef CONFIG_CSI_V2
#else
        /* disable the card power */
        csi_sdif_power_control(base, DRV_POWER_OFF);
#endif
        /* Delay several milliseconds to make card stable. */
        SDMMCHOST_Delay(500U);
    }
}

void SDMMCHOST_PowerOnCard(SDMMCHOST_TYPE base, const sdmmchost_pwr_card_t *pwr)
{
    /* use user define the power on function  */
    if (pwr != NULL) {
        pwr->powerOn();
        SDMMCHOST_Delay(pwr->powerOnDelay_ms);
    } else {
#ifdef CONFIG_CSI_V2
#else
        /* Enable the card power */
        csi_sdif_power_control(base, DRV_POWER_FULL);

#endif
        /* Delay several milliseconds to make card stable. */
        SDMMCHOST_Delay(500U);
    }
}

status_t SDMMCHOST_Init(SDMMCHOST_CONFIG *host, void *user_data)
{
    uint32_t sdif = csi_sdif_get_idx(host->base);

    sdif_callback_t sdifCallback = {0};
    sdif_host_t *sdifHost = (sdif_host_t *)host;

    /* init event timer. */
    SDMMCEVENT_InitTimer();

    /* Initialize SDIF. */
    sdifHost->config.endian_mode = kSDMMCHOST_EndianModeLittle;
    sdifHost->config.response_timeout = 0xFFU;
    sdifHost->config.card_det_debounce_clock = 0xFFFFFFU;
    sdifHost->config.data_timeout = 0xFFFFFFU;
    /* Set callback for SDIF driver. */
    sdifCallback.transfer_complete = SDMMCHOST_transfer_completeCallback;
    sdifCallback.card_inserted = SDMMCHOST_DetectCardInsertByHost;
    sdifCallback.card_removed = SDMMCHOST_DetectCardRemoveByHost;
    sdifCallback.sdif_interrupt = SDMMCHOST_Interrupt;

    csi_sdif_initialize(sdif, &sdifCallback, user_data);
    csi_sdif_config(sdifHost->base, &(sdifHost->config));

    /* Create transfer complete event. */
    if (false == SDMMCEVENT_Create(sdif, kSDMMCEVENT_transfer_complete)) {
        return kStatus_Fail;
    }

    /* Define transfer function. */
    sdifHost->transfer = SDMMCHOST_TransferFunction;

    /* Enable the card power here for mmc card case, because mmc card don't need card detect*/
    csi_sdif_enable_card_power(sdifHost->base, true);

    SDMMCHOST_CardDetectInit(sdifHost->base, (sdmmchost_detect_card_t *)user_data);

    return kStatus_Success;
}

void SDMMCHOST_Reset(SDMMCHOST_TYPE base)
{
    /* reserved for future */
}

void SDMMCHOST_Deinit(void *host)
{
    sdif_host_t *sdifHost = (sdif_host_t *)host;
#ifdef CONFIG_CSI_V2
    uint32_t sdif = csi_sdif_get_idx(sdifHost->base);
    csi_sdif_uninitialize(sdif, sdifHost->base);
#else
    csi_sdif_uninitialize(sdifHost->base);
#endif
    SDMMCHOST_CardDetectDeinit(sdifHost->base);
}

void SDMMCHOST_ErrorRecovery(SDMMCHOST_TYPE base)
{
    /* reserved for future */
}

void SDMMCHOST_BindingSDIF(SDMMCHOST_CONFIG *host, uint32_t sdif)
{
    host->base = csi_sdif_get_handle(sdif);
}

