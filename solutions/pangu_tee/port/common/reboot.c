/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <drv_wdt.h>
#include <tee_debug.h>

#define WDT_TIMEOUT 419

#ifdef CONFIG_WDG
static wdt_handle_t g_reboot_handle = 0;
#endif

static inline int getcurrent_psr (void)
{
     int flags;

      __asm__ __volatile__(
         "mfcr   %0, psr \n"
         :"=r"(flags)
         :
         :
         );

     return flags;
 }

 static inline int getcurrent_epsr (void)
{
     int flags;

      __asm__ __volatile__(
         "mfcr   %0, epsr \n"
         :"=r"(flags)
         :
         :
         );

     return flags;
 }


int reboot(void)
{
#ifdef CONFIG_WDG
    int ret = 0;
    TEE_LOGE("psr: %x\n", getcurrent_psr());
    TEE_LOGE("epsr: %x\n", getcurrent_epsr());

    g_reboot_handle = csi_wdt_initialize(0, 0);

    if (g_reboot_handle == 0) {
        TEE_LOGE("wdt init error\n");
        return -1;
    }

    ret = csi_wdt_set_timeout(g_reboot_handle, WDT_TIMEOUT);

    if (ret != 0) {
        TEE_LOGE("wdt set timeout error\n");
        return -1;
    }

    ret = csi_wdt_start(g_reboot_handle);

    TEE_LOGE("reboot\n");

    while (1);
#endif
    return 0;
}

