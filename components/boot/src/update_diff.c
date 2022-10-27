/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if (CONFIG_NO_OTA_UPGRADE == 0) && (CONFIG_OTA_NO_DIFF == 0)
#include "update_diff.h"
#include "csky_patch.h"
#include "update_log.h"

int diff_init_config(uint32_t bs_ram_size, uint32_t img_flash_sector, uint32_t misc_flash_sector,
                     uint32_t bs_flash_end_addr)
{
    int ret;
    static int g_diff_init = 0;
    UPD_LOGD("diff init config start.");
    if (g_diff_init == 1) {
        // init already
        UPD_LOGD("diff already init.");
        return 0;
    }
    ret = pat_config(bs_ram_size, img_flash_sector, misc_flash_sector, bs_flash_end_addr);
    g_diff_init = 1;
    UPD_LOGD("diff init config over.");
    return ret;
}

int diff_updata_img(uint32_t old_img_addr, uint32_t old_img_len, uint32_t partion_size,
                    uint32_t diff_img_addr, uint32_t diff_img_len)
{
    return pat_process((uint32_t *)old_img_addr, old_img_len, partion_size,
                       (uint32_t *)diff_img_addr, diff_img_len);
}
#endif