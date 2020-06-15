/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_ADDR_MAP_H
#define TEE_ADDR_MAP_H

#define OTP_BANK_SIZE   640
#define OTP_BASE_ADDR   0

#define FLASH_BASE_ADDR     0x8000000
#define FLASH_SECTOR_SIZE   4096
#define FLASH_SECTOR_COUNT  2048

#define SRAM_BASE_ADDR  0x18000000

#define TW_RO_ADDR      0x18000000
#define TW_RO_SIZE      0x00008000
#define TW_RW_ADDR      0x18008000
#define TW_RW_SIZE      0x00004000

#define NTW_RO_ADDR     (SRAM_BASE_ADDR + 0x500000 - 64*1024)
#define NTW_RO_SIZE     0x00008000
#define NTW_RW_ADDR     (NTW_RO_ADDR + NTW_RO_SIZE + 0x00002000)
#define NTW_RW_SIZE     0x00004000

#define NTW_ENTRY_ADDR  NTW_RO_ADDR
#endif
