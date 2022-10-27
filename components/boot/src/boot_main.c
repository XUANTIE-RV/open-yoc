/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "boot.h"
#include "boot_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys_clk.h>
#include <yoc/partition.h>

__attribute__((weak)) void boot_pre_jump_hook(void)
{

}
__attribute__((weak)) void boot_pre_ota_hook(void)
{

}

int boot_main(int argc, char **argv)
{
#if defined(CONFIG_DEBUG) && (CONFIG_DEBUG > 0)
    printf("\nWelcome %s!\n", BOOT_VER);
    printf("build: %s %s\r\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
#endif
    if (partition_init() <= 0) {
        printf("partitoin init failed!\n");
        goto fail;
    }

#if (CONFIG_PARITION_NO_VERIFY == 0)
    partition_t part;
    part = partition_open(MTB_IMAGE_NAME_IMTB);
    if (partition_verify(part) != 0) {
        printf("mtb verify e");
        goto fail;
    }
    partition_close(part);
    do {
        extern uint32_t km_init(void);
        km_init();
    } while(0);
#endif

#if (CONFIG_NO_OTA_UPGRADE == 0)
    boot_pre_ota_hook();
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    extern int bootab_init(void);
    if (bootab_init() != 0) {
        printf("bootab init failed.\n");
        while(1){};
    }
#else
    extern int update_init(void);
    if (update_init() != 0) {
        printf("some error occur!\n");
        while(1){};
    }
#endif /*CONFIG_OTA_AB*/
#endif /*CONFIG_NO_OTA_UPGRADE*/


#if (CONFIG_PARITION_NO_VERIFY == 0)
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    // do verify in boot_load_and_jump
#else
    // partition verify
    if (partition_all_verify() != 0) {
        printf("all verify failed!\n");
        goto fail;
    }
#endif /*CONFIG_OTA_AB*/
#endif

    boot_pre_jump_hook();
    // load and jump
    boot_load_and_jump();

fail:
    printf("boot failed!!!\n");
    boot_sys_reboot();
    return 0;
}
