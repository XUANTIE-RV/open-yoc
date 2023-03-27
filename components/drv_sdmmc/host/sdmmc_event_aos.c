/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
#ifndef CONFIG_KERNEL_NONE
#include <aos/kernel.h>
#include "sdmmc_event.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static aos_event_t g_sdmmc_event[4];
static uint8_t g_sdmmc_event_created[4];

/*******************************************************************************
 * Code
 ******************************************************************************/

void SDMMCEVENT_InitTimer(void)
{
}

bool SDMMCEVENT_Create(uint32_t sdif, sdmmc_event_t eventType)
{
    if (g_sdmmc_event_created[sdif] == 0) {
        int ret = aos_event_new(&g_sdmmc_event[sdif], 0);
        if (ret == 0) {
            g_sdmmc_event_created[sdif] = 1;
            // printf("%s, %d, sdif:%d, eventType:%d\n", __func__, __LINE__, sdif, eventType);
            return true;
        } else {
            return false;
        }
    }

    return true;
}

bool SDMMCEVENT_Wait(uint32_t sdif, sdmmc_event_t eventType, uint32_t timeoutMilliseconds)
{
    uint32_t actl;
    // printf("%s, %d, sdif:%d, eventType:%d, timeoutMilliseconds:%d\n", __func__, __LINE__, sdif, eventType, timeoutMilliseconds);
    int ret = aos_event_get(&g_sdmmc_event[sdif], (uint32_t)(1 << eventType), AOS_EVENT_AND_CLEAR, &actl, timeoutMilliseconds);
    if (ret == 0) {
        return true;
    } else {
        return false;
    }
}

bool SDMMCEVENT_Notify(uint32_t sdif, sdmmc_event_t eventType)
{
    // printf("%s, %d, sdif:%d, eventType:%d\n", __func__, __LINE__, sdif, eventType);
    int ret = aos_event_set(&g_sdmmc_event[sdif], (uint32_t)(1 << eventType), AOS_EVENT_OR);
    if (ret == 0) {
        return true;
    } else {
        return false;
    }
}

void SDMMCEVENT_Delete(uint32_t sdif, sdmmc_event_t eventType)
{
    // printf("%s, %d, sdif:%d, eventType:%d\n", __func__, __LINE__, sdif, eventType);
}

void SDMMCEVENT_Delay(uint32_t milliseconds)
{
    aos_msleep(milliseconds);
}
#endif /*CONFIG_KERNEL_NONE*/