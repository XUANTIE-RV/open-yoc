/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "app_config.h"
#include "app_main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <soc.h>
#include <drv/tick.h>
#include <boot.h>
#include <boot_wrapper.h>
#include <yoc/partition.h>
#include <yoc/partition_flash.h>
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <bootab.h>
#endif

void boot_load_and_jump(void)
{
#define FLASH_XIP_BASE (0x58000000 - 0x11000)
    const char *jump_to = "prim";
    unsigned long static_addr;
    unsigned long load_addr;
    unsigned long flash_xip_base;
    uint32_t image_size;
    partition_t part;
    partition_info_t *part_info;

#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    char *ab;
    char *pre_slot;
    char j2part[16];

    ab = (char *)bootab_get_current_ab();
    if (ab == NULL) {
        printf("select valid prim[ab] failed, panic !\n");
        goto fail;
    }
    snprintf(j2part, sizeof(j2part), "prim%s", ab);
    pre_slot = (char *)bootab_fallback(ab);
    if (pre_slot) {
        printf("#########fallback to prim%s\n", pre_slot);
        snprintf(j2part, sizeof(j2part), "prim%s", pre_slot);
    }

    printf("load img & jump to [%s]\n", j2part);
    part = partition_open(j2part);
    part_info = partition_info_get(part);
    if (part_info == NULL) {
        goto fail;
    }
    partition_close(part);

    if (mtb_image_verify(j2part)) {
        goto fail;
    }
    jump_to = j2part;
#endif /*CONFIG_OTA_AB*/

    printf("load img & jump to [%s]\n", jump_to);
    part = partition_open(jump_to);
    part_info = partition_info_get(part);
    partition_close(part);

    static_addr = part_info->start_addr + part_info->base_addr;
    load_addr = part_info->load_addr;
    image_size = part_info->image_size;
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    image_size = part_info->length;
#endif

    printf("load&jump 0x%lx,0x%lx,%d\n", static_addr, load_addr, image_size);
    flash_xip_base = (unsigned long)FLASH_XIP_BASE;
    if (static_addr != load_addr) {
        flash_xip_base = 0;
        printf("start to copy data from 0x%lx to 0x%lx, size: %d\n", static_addr, load_addr, image_size);
        void *handle = partition_flash_open(0);
        if (partition_flash_read(handle, static_addr, (void *)(load_addr), image_size) < 0) {
            goto fail;
        }
        partition_flash_close(handle);
        printf("all copy over..\n");
    }
#ifdef CONFIG_CSI_V2
    soc_dcache_clean();
    soc_icache_invalid();
#else
    csi_dcache_clean();
    csi_icache_invalid();
#endif

    void (*func)(void);
    if (memcmp((uint8_t *)((unsigned long)(flash_xip_base + load_addr + 4)), "CSKY", 4) == 0) {
        printf("j m\n");
        func = (void (*)(void))((unsigned long *)(flash_xip_base + load_addr));
    } else {
        func = (void (*)(void))(*(unsigned long *)(flash_xip_base + load_addr));
    }
    printf("j 0x%08lx\n", (unsigned long)(*func));

    csi_tick_uninit();

    (*func)();
    while(1) {;}

fail:
    printf("jump failed. reboot.\n");
    mdelay(200);
    boot_sys_reboot();
}

void boot_sys_reboot(void)
{
    extern void drv_reboot(void);
    drv_reboot();
}

static void trap_c_cb()
{
    printf("I am in boot trap. going to reboot.");
    boot_sys_reboot();
}

#if CONFIG_IMG_AUTHENTICITY_NOT_CHECK == 0
bool check_is_need_verify(const char *name)
{
    static const char *g_need_verify_name[] = {
        "prima",
        "primb",
        NULL
    };
    int i = 0;
    while(1) {
        if (g_need_verify_name[i] == NULL) {
            break;
        }
        if (strcmp(g_need_verify_name[i], name) == 0) {
            return true;
        }
        i++;
    }
    return false;
}
#endif

int main(int argc, char *argv[0])
{
    int ret;
    extern void (*trap_c_callback)();
    trap_c_callback = trap_c_cb;

    extern void board_yoc_init(void);
    board_yoc_init();

    ret = boot_main(argc, argv);
    return ret;
}

//////////////////////////////////////////////////////////////
#include <sys/stat.h>
int _unlink(const char *path)
{
    (void)path;
    return 0;
}

int stat(const char *path, struct stat *buf)
{
    (void)path;
    (void)buf;
    return 0;
}