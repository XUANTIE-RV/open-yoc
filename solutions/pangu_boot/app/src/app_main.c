/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "app_config.h"
#include "app_main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <soc.h>
#include <drv/porting.h>
#include <drv/tick.h>
#include <boot.h>
#include <boot_wrapper.h>
#include <yoc/partition.h>
#include <yoc/partition_flash.h>
#include "board.h"

#define JUMP_TO_IMG_NAME "tee"

#if 1
#include <key_mgr.h>
uint32_t km_get_pub_key_by_name(const char *name, key_handle *key, uint32_t *key_size)
{
#if 1
    // use develop key
    printf("Use develop key to verify...\n");
    static const uint8_t g_pubkey_rsa1024[] = {
        0x96, 0x36, 0xA2, 0x70, 0x2D, 0xC0, 0xB1, 0xC5, 0x50, 0xEE, 0x47, 0xEA, 0xB7, 0x2C, 0x4E, 0x28,
        0x5E, 0x1C, 0x79, 0xE8, 0x6E, 0x5D, 0xBF, 0xA0, 0x64, 0x01, 0x0F, 0xCB, 0x3A, 0x5A, 0xC2, 0x01,
        0xAA, 0x3E, 0xF8, 0x8A, 0xCB, 0x19, 0x97, 0xE5, 0xA5, 0xF5, 0x38, 0x83, 0x1D, 0x48, 0xE9, 0x20,
        0x41, 0x01, 0x63, 0x6E, 0xF1, 0x97, 0x18, 0xA8, 0x4F, 0xEA, 0xFB, 0x2D, 0xC4, 0xFD, 0xDB, 0x50,
        0x02, 0x06, 0xD6, 0x4B, 0x61, 0x74, 0x60, 0x5C, 0xFA, 0x83, 0xCE, 0xF7, 0x5C, 0x4C, 0xCE, 0xB8,
        0xF9, 0x64, 0x11, 0xA4, 0x74, 0x2F, 0xA3, 0x8F, 0xC6, 0x53, 0x4D, 0xDF, 0x82, 0x54, 0xF9, 0xD7,
        0xB9, 0x09, 0x75, 0x55, 0xC1, 0x9D, 0x33, 0x9A, 0x9B, 0xA6, 0x30, 0x87, 0x2B, 0x07, 0x99, 0x4B,
        0x02, 0xAF, 0xC7, 0x79, 0xD0, 0x12, 0xD1, 0x1E, 0x63, 0xC1, 0x85, 0x9C, 0xBF, 0x71, 0xF9, 0x9F,
    };
    if (key && key_size) {
        *key = (key_handle)g_pubkey_rsa1024;
        *key_size = sizeof(g_pubkey_rsa1024);
        return KM_OK;
    }
    return KM_ERR;
#else
    // Failure
    return KM_ERR;
#endif
}
#endif

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
        partition_flash_read(NULL, static_addr, (void *)load_addr, image_size);
    }
    printf("all copy over..");

    soc_dcache_clean();
    soc_icache_invalid();

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

#ifdef CONFIG_CHIP_PANGU
    extern void actions_before_application(void);
    actions_before_application();
#endif

    (*func)();
    while(1) {;}
}

void boot_sys_reboot(void)
{
    extern void drv_reboot(void);
    drv_reboot();
}

static void trap_c_cb(void)
{
    boot_sys_reboot();
}

extern void (*trap_c_callback)(void);
int main(int argc, char *argv[0])
{
    int ret;

    trap_c_callback = trap_c_cb;
    board_init();
    ret = boot_main(argc, argv);
    return ret;
}
