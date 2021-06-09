/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "boot.h"
#include "boot_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <yoc/partition.h>

int boot_main(int argc, char **argv)
{
    printf("\nWelcome %s!\n", BOOT_VER);
    printf("build: %s %s\r\n", __DATE__, __TIME__);

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
    do {
        extern int update_init(void);
        update_init();    
    } while(0);
#endif
#if (CONFIG_PARITION_NO_VERIFY == 0)
    // partition verify
    if (partition_all_verify() != 0) {
        printf("all verify failed!\n");
        goto fail;
    }
#endif
    // load and jump
    boot_load_and_jump();

fail:
    printf("boot failed!!!\n");
    boot_sys_reboot();
    return 0;
}
