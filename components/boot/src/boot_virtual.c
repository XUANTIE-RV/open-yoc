/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "boot_wrapper.h"
#include <yoc/partition.h>
#include <soc.h>
#include <stdio.h>
#include <string.h>
#include "boot_flash_porting.h"
#ifdef CONFIG_CSI_V2
#include <drv/porting.h>
#endif

__attribute__((weak)) void boot_load_and_jump(void)
{
    const char *jump_to = "prim";
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
        boot_flash_read(static_addr, (void *)load_addr, image_size);
    }
    printf("all copy over..");
#ifdef CONFIG_CSI_V2
    soc_dcache_clean();
    soc_icache_invalid();
#else
    csi_dcache_clean();
    csi_icache_invalid();
#endif

    void (*func)(void);
    if (memcmp((uint8_t *)(load_addr + 4), "CSKY", 4) == 0) {
        printf("j m\n");
        *(uint32_t *)&func = load_addr;
    } else if (strcmp("tee", jump_to) == 0) {
        printf("j tee\n");
        *(uint32_t *)&func = load_addr;
    } else {
        *(uint32_t *)&func = *(uint32_t *)load_addr;
    }
    printf("j 0x%08x\n", (uint32_t)(*func));

    (*func)();
    while(1) {;}
}

__attribute__((weak)) void boot_sys_reboot(void)
{
    extern void drv_reboot(void);
    drv_reboot();
}