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

void boot_load_and_jump(void)
{
    const char *jump_to = "prim";
    unsigned long load_addr;
    partition_t part;
    partition_info_t *part_info;
    
    printf("jump to [%s]\n", jump_to);
    part = partition_open(jump_to);
    part_info = partition_info_get(part);
    partition_close(part);

    load_addr = part_info->load_addr;

    printf("jump to [0x%lx]\n", load_addr);
    void (*func)(void);
    if (memcmp((uint8_t *)(load_addr + 4), "CSKY", 4) == 0) {
        printf("j m\n");
        func = (void (*)(void))((unsigned long *)load_addr);
    } else {
        func = (void (*)(void))(*(unsigned long *)load_addr);
    }
    printf("j 0x%08lx\n", (unsigned long)(*func));

    csi_tick_uninit();

    (*func)();
    while(1) {;}
}

void boot_sys_reboot(void)
{
    extern void drv_reboot(void);
    drv_reboot();
}

#if CONFIG_IMG_AUTHENTICITY_NOT_CHECK == 0
bool check_is_need_verify(const char *name)
{
    static const char *g_need_verify_name[] = {
        "prim",
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