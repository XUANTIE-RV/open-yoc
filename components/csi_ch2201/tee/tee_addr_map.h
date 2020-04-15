/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_ADDR_MAP_H
#define TEE_ADDR_MAP_H

#define PART_ADDR_TEE 0x10001000
#define PART_SIZE_TEE 0x005000

#define OTP_BANK_SIZE               2048
#define OTP_BASE_ADDR               0x4003f800//0x40006000

#define TW_RO_ADDR      PART_ADDR_TEE //0x10005800
#define TW_RO_SIZE      PART_SIZE_TEE
#define TW_RW_ADDR      0x60000000//0x20000000
#define TW_RW_SIZE      0x00001000

#define NTW_RO_ADDR     0x1000C000
#define NTW_RO_SIZE     0x00010000
#define NTW_RW_ADDR     (TW_RW_ADDR + 0x00002000)
#define NTW_RW_SIZE     0x00006000

#define SRAM_BASE_ADDR  0x60000000//0x20000000

#define NTW_ENTRY_ADDR  NTW_RO_ADDR
#endif
