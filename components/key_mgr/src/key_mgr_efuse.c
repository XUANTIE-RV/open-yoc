/**
* Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
*/

/******************************************************************************
 * @file     key_mgr_efuse.c
 * @brief    The File is for efuse operation
 * @version  V1.0
 * @date     3. June 2020
 ******************************************************************************/


#ifdef CONFIG_EFUSE_SUPPORT

#ifdef CONFIG_TEE_VERSION
#include "efuse.h"
#else
#include "key_mgr_efuse.h"
#include "key_mgr_log.h"
#include <string.h>
#include <stdio.h>

#ifdef CONFIG_CSI_V1
#include "drv/efusec.h"
#endif
#ifdef CONFIG_CSI_V2
#include "drv/efuse.h"
#endif

int32_t efusec_read_bytes(uint32_t addr, uint8_t *buf, uint32_t len)
{
    int32_t ret;
#ifdef CONFIG_CSI_V1
    efusec_handle_t handle = drv_efusec_initialize(0);
    ret = drv_efusec_read(handle, addr, buf, len);
    csi_efusec_uninitialize(handle);
#endif
#ifdef CONFIG_CSI_V2
    csi_efuse_t efuse;
    ret = drv_efuse_init(&efuse, 0);

    if (ret < 0U) {
        return ret;
    }

    ret = drv_efuse_read(&efuse, addr, buf, len);
    drv_efuse_uninit(&efuse);
#endif
    return ret;
}

#endif /* CONFIG_TEE_VERSION */
#endif
