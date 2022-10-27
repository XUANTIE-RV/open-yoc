/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "app_config.h"
#include "app_main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <soc.h>
#include <boot.h>
#include <boot_wrapper.h>
#include <yoc/partition.h>
#include <yoc/partition_flash.h>

#define JUMP_TO_IMG_NAME "prim"

void boot_load_and_jump(void)
{
    const char *jump_to = (const char *)JUMP_TO_IMG_NAME;
    uint32_t static_addr;
    uint32_t load_addr;
    uint32_t image_size;
    partition_t part;
    partition_info_t *part_info;
    
    printf("load img & jump to [%s]\n", jump_to);
    part = partition_open(jump_to);
    part_info = partition_info_get(part);
    partition_close(part);

    static_addr = part_info->start_addr + part_info->base_addr;
    load_addr = part_info->load_addr;
    image_size = part_info->image_size;

    printf("load&jump 0x%x,0x%x,%d\n", static_addr, load_addr, image_size);
    if (static_addr != load_addr) {
        printf("start copy..");
        partition_flash_read(NULL, static_addr, (void *)load_addr, image_size);
        printf("all copy over..");
    } else {
        printf("xip...\n");
    }
    csi_dcache_clean_invalid();
    csi_icache_invalid();

    void (*func)(void);
    if (memcmp((uint8_t *)(load_addr + 4), "CSKY", 4) == 0) {
        printf("j m\n");
        func = (void (*)(void))((uint32_t *)load_addr);
    } else if (strcmp("tee", jump_to) == 0) {
        printf("j tee\n");
        func = (void (*)(void))((uint32_t *)load_addr);
    } else {
        func = (void (*)(void))(*(uint32_t *)load_addr);
    }
    printf("j 0x%08x\n", (uint32_t)(*func));

    (*func)();
    while(1) {;}
}

void boot_sys_reboot(void)
{
    extern void drv_reboot(void);
    drv_reboot();
}

int main(int argc, char *argv[0])
{
    int ret;

    extern void board_yoc_init(void);
    board_yoc_init();

    ret = boot_main(argc, argv);
    return ret;
}
