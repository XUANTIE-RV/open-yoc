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
#include <yoc/partition_device.h>
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <bootab.h>
#endif

#if defined(CONFIG_DEBUG) && (CONFIG_DEBUG > 0)
#define DBG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DBG_PRINT(...)
#endif

void boot_load_and_jump(void)
{
    uint32_t t1, t2, t3;
    const char *jump_to = "prim";
    unsigned long static_addr;
    unsigned long load_addr;
    uint32_t image_size, preload_size;
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
    {
        extern int update_mtb_for_ab(const char *ab);
        if (update_mtb_for_ab(ab)) {
            printf("update mtb for ab failed !\n");
            goto fail;
        }
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

    DBG_PRINT("load img & jump to [%s]\n", jump_to);
    part = partition_open(jump_to);
    part_info = partition_info_get(part);

    static_addr = part_info->start_addr + part_info->base_addr;
    load_addr = part_info->load_addr;
    preload_size = part_info->preload_size;
    image_size = part_info->image_size;
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    mtb_partition_info_t mtb_part_info;
    if (mtb_get_partition_info(jump_to, &mtb_part_info)) {
        printf("get image size error\n");
        goto fail;
    }
    image_size = mtb_part_info.img_size;
    preload_size = mtb_part_info.preload_size;
    printf("the image size is 0x%x, preload_size: 0x%x, partition_length: 0x%lx \n", image_size, preload_size, part_info->length);
#endif

    DBG_PRINT("load&jump 0x%lx,0x%lx,img_size = 0x%x, pre_size = 0x%x\n", static_addr, load_addr, image_size, preload_size);
    image_size = preload_size ? preload_size : image_size;

    t1 = csi_tick_get_ms();
    if (!partition_check_firmware_is_packed(part)) {
        if (static_addr != load_addr) {
            DBG_PRINT("start copy %d bytes\n", image_size);
            DBG_PRINT("##cur_ms:%d\n", csi_tick_get_ms());
            if (partition_read(part, 0, (void *)load_addr, image_size)) {
                DBG_PRINT("part read e.\n");
                goto fail;
            }
            DBG_PRINT("##cur_ms:%d\n", csi_tick_get_ms());
            DBG_PRINT("all copy over..\n");
        } else {
            DBG_PRINT("xip...\n");
        }
    } else {
        // split & decompress
        printf("##start to decompress and copy.[%d ms]\n", csi_tick_get_ms());
        part = partition_open(jump_to);
        if (static_addr != load_addr) {
            // copy prim to ram
            if (partition_split_and_copy(part, 0)) {
                DBG_PRINT("decompress and copy prim bin failed.\n");
                goto fail;
            }
        }
        partition_close(part);
        printf("##decompress and copy ok.[%d ms]\n", csi_tick_get_ms());
    }
    t2 = csi_tick_get_ms();
    csi_dcache_clean_invalid();
    csi_icache_invalid();

    void (*func)(void);
    if (memcmp((uint8_t *)(load_addr + 4), "CSKY", 4) == 0) {
#if defined(CONFIG_DEBUG) && (CONFIG_DEBUG > 0)
        printf("j m\n");
#endif
        func = (void (*)(void))((unsigned long *)load_addr);
    } else {
        func = (void (*)(void))(*(unsigned long *)load_addr);
    }
    t3 = csi_tick_get_ms();
    printf("##ms: t1=%d, t2=%d, t3=%d\n", t1, t2, t3);
    DBG_PRINT("j 0x%08lx\n", (unsigned long)(*func));
    DBG_PRINT("##cur_ms:%d\n", csi_tick_get_ms());

    csi_tick_uninit();

    (*func)();
    while(1) {;}

fail:
    DBG_PRINT("jump failed. reboot.\n");
    mdelay(3000);
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
