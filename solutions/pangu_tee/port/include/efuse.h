/******************************************************************************
 * @file     efuse.h
 * @brief    The Head File is for efuse operation
 * @version  V1.0
 * @date     03. June 2019
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
#ifndef _EFUSEC_H_
#define _EFUSEC_H_

#include <stdint.h>
#include <stdbool.h>

#define EFUSEC_CTRL_PROGRAM_ENABLE          (1 << 0)
#define EFUSEC_CTRL_RELOAD_ENABLE           (1 << 1)
#define EFUSEC_CTRL_READ_ENABLE             (1 << 2)

#define EFUSEC_STA_PROGRAM_DONE             (1 << 0)
#define EFUSEC_STA_RELOAD_DONE              (1 << 1)
#define EFUSEC_STA_READ_DONE                (1 << 2)
#define EFUSEC_STA_BUSY                     (1 << 3)

#define LOCK_CTRL_ENABLE_LOCK0              (1 << 0)  /* all shadow regs can't be programmed */
#define LOCK_CTRL_ENABLE_LOCK1              (1 << 1)  /* all eFuse can't be programmed */

#define CK_EFUSEC_TIMER                     (0x94c494c)

int32_t efusec_read_bytes(uint32_t addr, uint8_t *buf, uint32_t len);

bool otp_program(uint32_t dest_addr, uint8_t *buf, uint32_t b_length);

#endif
