/******************************************************************************
 * @file     efuse.c
 * @brief    The File is for efuse operation
 * @version  V1.0
 * @date     3. June 2019
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

#include "efuse.h"
#include "ck_board.h"
#include <string.h>
#include <stdio.h>

int32_t efusec_program_word(uint32_t addr, uint32_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t fbase = OTP_CTR_BASEADDR;

    uint32_t time_count = 0;
    for (i = 0; i < len; i++) {
        *(volatile uint32_t *)(fbase + 0x04) = addr;
        *(volatile uint32_t *)(fbase + 0x08) = *buf;
        *(volatile uint32_t *)(fbase + 0x00) = EFUSEC_CTRL_PROGRAM_ENABLE;
        time_count                           = 0;
        while (1) {
            if ((*(volatile uint32_t *)(fbase + 0x10)) & EFUSEC_STA_PROGRAM_DONE) {
                break;
            }
            time_count++;
            if (time_count > 0x100000) {
                return -1;
            }
        }
        buf++;
        addr += 4;
    }

    return i;
}

int32_t efusec_read_bytes(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t fbase = OTP_CTR_BASEADDR;

    *(volatile uint32_t *)(fbase + 0x0c) = CK_EFUSEC_TIMER;
    uint32_t time_count                  = 0;
    for (i = 0; i < len; i++) {
        time_count = 0;
        while ((*(volatile uint32_t *)(fbase + 0x10)) & EFUSEC_STA_BUSY) {
            time_count++;
            if (time_count > 0x100000) {
                return -1;
            }
        }

        *(volatile uint32_t *)(fbase + 0x04) = addr;
        *(volatile uint32_t *)(fbase + 0x00) = EFUSEC_CTRL_READ_ENABLE;
        time_count                           = 0;
        while (1) {
            if ((*(volatile uint32_t *)(fbase + 0x10)) & EFUSEC_STA_READ_DONE) {
                break;
            }
            time_count++;
            if (time_count > 0x100000) {
                return -1;
            }
        }
        *buf = (uint8_t) * (volatile uint32_t *)(fbase + 0x14);
        buf++;
        addr += 1;
    }

    return i;
}

bool otp_program(uint32_t dest_addr, uint8_t *buf, uint32_t b_length)
{
    uint32_t temp_data[(b_length + 3) >> 2];
    int      ret = 0;

    dest_addr += OTP_BASEADDR;
    if (!IS_OTP_ADDR(dest_addr) || (!IS_OTP_ADDR(dest_addr + b_length)) || NULL == buf ||
        b_length == 0) {
        return false;
    }

    memcpy(temp_data, buf, b_length);
    efusec_program_word(dest_addr, temp_data, (b_length + 3) >> 2);
    memset(temp_data, 0, b_length);

    efusec_read_bytes(dest_addr, (uint8_t *)temp_data, b_length);
    ret = memcmp(temp_data, buf, b_length);
    if (ret != 0) {
        printf("write efuse check error\n");
        return false;
    }
    return true;
}
