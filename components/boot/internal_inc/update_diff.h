/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __UPDATE_DIFF_H__
#define __UPDATE_DIFF_H__

#include <stdint.h>

int diff_init_config(uint32_t bs_ram_size, uint32_t img_flash_sector,
                         uint32_t misc_flash_sector, uint32_t bs_flash_end_addr);
int diff_updata_img(uint32_t old_img_addr, uint32_t old_img_len, uint32_t partion_size,
                        uint32_t diff_img_addr, uint32_t diff_img_len);

#endif