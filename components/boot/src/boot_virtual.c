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
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <bootab.h>
#endif

__attribute__((weak)) void boot_load_and_jump(void)
{
    const char *jump_to = "prim";
    unsigned long static_addr;
    unsigned long load_addr;
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
    {
        extern int update_mtb_for_ab(void);
        if (update_mtb_for_ab()) {
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
#endif

    printf("load img & jump to [%s]\n", jump_to);
    part = partition_open(jump_to);
    part_info = partition_info_get(part);

    static_addr = part_info->start_addr + part_info->base_addr;
    load_addr = part_info->load_addr;
    image_size = part_info->image_size;
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    mtb_partition_info_t mtb_part_info;
    if (mtb_get_partition_info(jump_to, &mtb_part_info)) {
        printf("get image size error\n");
        goto fail;
    }
    image_size = mtb_part_info.img_size;
    printf("the image size is 0x%x\n", image_size);
#endif

    printf("load&jump 0x%lx,0x%lx,%d\n", static_addr, load_addr, image_size);
    if (static_addr != load_addr) {
        printf("start to copy data from 0x%lx to 0x%lx, size: %d\n", static_addr, load_addr, image_size);
        if (partition_read(part, part_info->start_addr, (void *)(load_addr), image_size) < 0) {
            printf("copy failed.\n");
            goto fail;
        }
    }
    printf("all copy over..\n");
#ifdef CONFIG_CSI_V2
    soc_dcache_clean();
    soc_icache_invalid();
#else
    csi_dcache_clean();
    csi_icache_invalid();
#endif

    void (*func)(void);
    if (memcmp((uint8_t *)((unsigned long)(load_addr + 4)), "CSKY", 4) == 0) {
        printf("j m\n");
        func = (void (*)(void))((unsigned long *)load_addr);
    } else if (strcmp("tee", jump_to) == 0) {
        printf("j tee\n");
        func = (void (*)(void))((unsigned long *)load_addr);
    } else {
        func = (void (*)(void))(*(unsigned long *)load_addr);
    }
    printf("j 0x%08lx\n", (unsigned long)(*func));

    (*func)();
    while(1) {;}
fail:
    printf("jump failed. reboot.\n");
    boot_sys_reboot();
}

__attribute__((weak)) void boot_sys_reboot(void)
{
    extern void drv_reboot(void);
    drv_reboot();
}

__attribute__((weak)) bool boot_is_no_needed_ota(const char *name)
{
#if 0
    static const char *unota_list[] = {
        "cpu1",
        "prim",
        "yyy"
    };
    int count = 3;

    if (!name) {
        return false;
    }

    printf("%s, %d, %s\n", __func__, __LINE__, name);
    for (int i = 0; i < count; i++) {
        if (!strncmp(name, unota_list[i], MTB_IMAGE_NAME_SIZE)) {
            printf("--got-----------%s\n", unota_list[i]);
            return true;
        }
    }
#endif
    return false;
}