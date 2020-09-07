/******************************************************************************
 * @file     ck_board.h
 * @brief    The File is for config the borad information
 * @version  V1.0
 * @date     14. Mar 2019
 ******************************************************************************/
/* ---------------------------------------------------------------------------
 * Copyright (C) 2017 CSKY Limited. All rights reserved.
 *
 * Redistribution and use of this software in source and binary forms,
 * with or without modification, are permitted provided that the following
 * conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of CSKY Ltd. nor the names of CSKY's contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission of CSKY Ltd.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------------- */

#ifndef __CK_BOARD_H
#define __CK_BOARD_H

/**************************************
 * MCU & Borads.
 *************************************/
/* APB frequence definition */
#define APB_DEFAULT_FREQ               (48000000)  /* 48MHz for asic 30MHz for fpga*/
/***** AES ******/
#define CK_AES_ADDRBASE                (0x28000000UL)

/***** Uart *******/
#define CK_UART0_ADDRBASE              (0x3F000000UL)
#define CK_UART1_ADDRBASE              (0x3F800000UL)

/**** Timer ****/
#define CK_TIMER0_BASSADDR             (0x39800000UL)
#define CK_TIMER1_BASSADDR             (0x39800014UL)
#define CK_TIMER_CONTROL_BASSADDR      (0x398000a0UL)

/****** RSA  *************/
#define CK_RSA_ADDRBASE                (0x33000000UL)

/****** TRNG *************/
#define CK_TRNG_ADDRBASE               (0x34000000UL)

/****** LPM POWM  *************/
#define PMU_SYSCLK_DIVEN                (0x30000004)
#define CK_POWM_ADDRBASE                0x30000014
#define CPU_JTAG_EN_REGISTER            (0x3a000020)

/****** POWM  *************/
#define AP_CPU0_PMU_LPBOOT_ADDRBASE     0x30000068
#define CP_CPU1_PMU_BOOT_ADDRBASE       0x30000080
#define CP_CPU2_PMU_BOOT_ADDRBASE       0x30000044
/****** SHA **************/
#define CK_SHA_ADDRBASE                 (0x29000000UL)

/**** QSPI FLASH ****/
#define FLASH_BASEADDR                  (0x3A800000UL)

/***** WDT ******/
#define CK_WDT_ADDRBASE                 (0x38000000UL)

/***** ioctl ******/
#define CK_IOCTL_ADDRBASE               (0x3B000000UL)

/* dcc register define */
#define DCC_TXRX_DATA                   0xE0011058
#define DCC_TXRX_FLAG                   0xE001103C
#define DCC_TXRX_INTSTATE               0xE0011034

/* stack size config */
#define STACK_SIZE                      0x4000
#define INVALID_ADDRESS                 0xffffffff
#define INVALID_FLASH_KEY_VALUE         0x0
#define KEY_PACKAGE_MAX_SIZE            OTP_SIZE
#define SOC_INTERNAL_EFUSE_ORIGIN_BIT_VALUE 0

#define QSPICLK_CONFIG_ADDR             (0x264)
#define QSPICLK_CONFIG_MAGIC            (0xf0)

#define CK_MANIFEST_IDX_BASE            0x0
#define EFUSE_LOCK_ADDR                 (0x270)

#define OTP_CTR_BASEADDR               (0x3A000000UL)
#define OTP_BASEADDR                    0
#define OTP_SIZE                       (0x280)

#define CK_SRAM_ADDRESS_BASE            0x10000000
#define SRAM_SIZE                       0x7FFF

#define FLASH_START                     0x08000000
#define FLASH_SIZE                      0x800000

#define RUN_MEM_BASEADDR                FLASH_START
#define STORAGE_MEM_BASEADDR            FLASH_START
#define STORAGE_MEM_SIZE                FLASH_SIZE
#define CONFIG_NXIP_MEM

/* host tool connecting uart */
#define HT_CHANNEL_UART_BASE            CK_UART1_ADDRBASE

/* timer for timeout count, timeout 500ms */
#define TIMEOUT_COUNT_TIMER             CK_TIMER0_BASSADDR

#define IS_FLASH_ADDR(addr) \
                ((addr & 0xff000000) == 0x08000000)
#define IS_OTP_ADDR(addr) \
                ((addr >= (OTP_BASEADDR)) && (addr < OTP_BASEADDR + OTP_SIZE))

#define OTP_LOCK_MAGIC      0x2
#endif
