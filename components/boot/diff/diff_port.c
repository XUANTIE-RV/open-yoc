/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if (CONFIG_NO_OTA_UPGRADE == 0) && (CONFIG_OTA_NO_DIFF == 0)
#include <stdlib.h>
#include <string.h>
#include <csky_patch.h>
#include "boot_flash_porting.h"
#include "update_log.h"

/****************************************************************************
 * Functions
 ****************************************************************************/

int diff_bsp_flash_read(uint32_t addroffset, uint8_t *buff, int bytesize)
{
    return boot_flash_read(addroffset, buff, bytesize);
}

int diff_bsp_flash_write(uint32_t addroffset, uint8_t *buff, int bytesize)
{
    return boot_flash_write(addroffset, buff, bytesize);
}

int diff_bsp_flash_erase(uint32_t addroffset, uint32_t bytesize)
{
    // UPD_LOGD("diff, addroffset:0x%x, bytesize:%d", addroffset, bytesize);
    return boot_flash_erase(addroffset, bytesize);
}

void *diff_malloc(uint32_t bytesize)
{
    return malloc(bytesize);
}

void diff_free(void *ptr)
{
    free(ptr);
}
#endif