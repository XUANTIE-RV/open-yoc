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

#include "sdmmc_event.h"

#if defined(__XCC__)
#include "silan_timer.h"
#include "silan_timer_regs.h"
#include "silan_irq.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get event instance.
 * @param eventType The event type
 * @return The event instance's pointer.
 */
static volatile uint32_t *SDMMCEVENT_GetInstance(uint32_t sdif, sdmmc_event_t eventType);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Card detect event. */
static volatile uint32_t g_eventCardDetect[2];

/*! @brief transfer complete event. */
static volatile uint32_t g_eventtransfer_complete[2];

/*! @brief Time variable unites as milliseconds. */
static volatile uint32_t g_eventTimeMilliseconds;

/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    g_eventTimeMilliseconds++;
}

void SDMMCEVENT_InitTimer(void)
{
#if defined(__CC_ARM)
    /* Set systick reload value to generate 1ms interrupt */
    SysTick_Config(silan_get_mcu_cclk() / 1000U);
#endif

#if defined(__XCC__)
    silan_timer_task_request(SysTick_Handler);
#endif

#if defined(__CSKY__)
    //silan_timer_task_request(SysTick_Handler);
#endif
}

static volatile uint32_t *SDMMCEVENT_GetInstance(uint32_t sdif, sdmmc_event_t eventType)
{
    volatile uint32_t *event;

    switch (eventType) {
        case kSDMMCEVENT_transfer_complete:
            event = &g_eventtransfer_complete[sdif];
            break;

        case kSDMMCEVENT_CardDetect:
            event = &g_eventCardDetect[sdif];
            break;

        default:
            event = NULL;
            break;
    }

    return event;
}

bool SDMMCEVENT_Create(uint32_t sdif, sdmmc_event_t eventType)
{
    volatile uint32_t *event = SDMMCEVENT_GetInstance(sdif, eventType);

    if (event) {
        *event = 0;
        return true;
    } else {
        return false;
    }
}

bool SDMMCEVENT_Wait(uint32_t sdif, sdmmc_event_t eventType, uint32_t timeoutMilliseconds)
{
    uint32_t startTime;
    uint32_t elapsedTime = 0;

    volatile uint32_t *event = SDMMCEVENT_GetInstance(sdif, eventType);

    if (timeoutMilliseconds && event) {
        startTime = g_eventTimeMilliseconds;

        do {
#ifdef CONFIG_CHIP_TX216
            mdelay(1);
            elapsedTime++;
#else
            elapsedTime = (g_eventTimeMilliseconds - startTime);
#endif
        } while ((*event == 0U) && (elapsedTime < timeoutMilliseconds));

        *event = 0U;

        return ((elapsedTime < timeoutMilliseconds) ? true : false);
    } else {
        return false;
    }
}

bool SDMMCEVENT_Notify(uint32_t sdif, sdmmc_event_t eventType)
{
    volatile uint32_t *event = SDMMCEVENT_GetInstance(sdif, eventType);

    if (event) {
        *event = 1U;
        return true;
    } else {
        return false;
    }
}

void SDMMCEVENT_Delete(uint32_t sdif, sdmmc_event_t eventType)
{
    volatile uint32_t *event = SDMMCEVENT_GetInstance(sdif, eventType);

    if (event) {
        *event = 0U;
    }
}

void SDMMCEVENT_Delay(uint32_t milliseconds)
{
    uint32_t startTime = g_eventTimeMilliseconds;
    uint32_t periodTime = 0;

    while (periodTime < milliseconds) {
        periodTime = g_eventTimeMilliseconds - startTime;
    }
}
