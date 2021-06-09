/**
* Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
*/

/******************************************************************************
 * @file     key_mgr_efuse.h
 * @brief    The Head File is for efuse operation
 * @version  V1.0
 * @date     03. June 2020
 ******************************************************************************/

#ifndef _EFUSEC_H_
#define _EFUSEC_H_

#include <stdint.h>
#include <stdbool.h>

int32_t efusec_read_bytes(uint32_t addr, uint8_t *buf, uint32_t len);

#endif
