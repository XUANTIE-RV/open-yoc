/******************************************************************************
 * @file     tee_addr_map.h
 * @brief    save the cpu context
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


#ifndef TEE_ADDR_MAP_H
#define TEE_ADDR_MAP_H

#define TW_RO_ADDR      0x18000000
#define TW_RO_SIZE      0x00008000
#define TW_RW_ADDR      0x18010000
#define TW_RW_SIZE      0x00004000

#define NTW_RO_ADDR     0x18008000
#define NTW_RO_SIZE     0x00008000
#define NTW_RW_ADDR     (TW_RW_ADDR + 0x00002000)
#define NTW_RW_SIZE     0x00004000

#define SRAM_BASE_ADDR  0x20000000

#define NTW_ENTRY_ADDR  NTW_RO_ADDR
#endif
