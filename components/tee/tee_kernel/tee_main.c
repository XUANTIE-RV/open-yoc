/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <tee_addr_map.h>
#include "tee_debug.h"
#include "drv_trng.h"
#include <yoc/partition.h>
#include "key_mgr.h"

extern void rte_to_ntw(void);

const char version[] = CONFIG_TEE_VERSION;
const char compile_date[] = __DATE__;
const char compile_time[] = __TIME__;

unsigned int g_ntw_addr = NTW_ENTRY_ADDR;

static void tee_info_dump(void)
{
    TEE_LOG("Tee v%s Initliaze done, %s %s\n", version, compile_date, compile_time);
}

__attribute__((weak)) void jump_to_ntw(void)
{
    uint32_t static_addr = 0, load_addr = 0;
    uint32_t size;
    partition_t partition;
    partition_info_t *info;
    const char *j2part = "prim";

    partition = partition_open(j2part);
    info = partition_info_get(partition);
    partition_close(partition);
    static_addr = info->base_addr + info->start_addr;
    load_addr = info->load_addr;
    size = info->image_size;

    if (info == NULL) {
        TEE_LOGE("get %s addr fail.\n", j2part);
    } else {
        g_ntw_addr = load_addr;
    }

    TEE_LOGI("static addr:0x%x, load_addr:0x%x, size:%d \n", static_addr, load_addr, size);

    memcpy((void *)load_addr, (void *)static_addr, size);

    TEE_LOGI("jump to 0x%x \n", g_ntw_addr);

    rte_to_ntw();
}

int tee_main(void)
{
#ifdef CONFIG_TEE_DEBUG
    extern void console_init();
    console_init();
#endif

    km_init();

    partition_init();
    
    tee_info_dump();

    jump_to_ntw();

    while (1);

    return 1;
}

