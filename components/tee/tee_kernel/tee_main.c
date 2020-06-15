/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <tee_addr_map.h>
#include "tee_debug.h"
#include "drv_trng.h"
#include "mtb.h"
#include "kp.h"

extern void rte_to_ntw(void);

const char version[] = CONFIG_TEE_VERSION;
const char compile_date[] = __DATE__;
const char compile_time[] = __TIME__;

unsigned int g_ntw_addr = NTW_ENTRY_ADDR;

static void tee_info_dump(void)
{
    TEE_LOG("Tee v%s Initliaze done, %s %s\n", version, compile_date, compile_time);
}

void jump_to_ntw(void)
{
    int ret;
    uint32_t static_addr = 0, load_addr = 0;
    uint32_t size;

    ret = mtb_get_ntw_addr(&static_addr, &load_addr, &size);

    if (ret) {
        TEE_LOGE("get ntw addr fail, ret %d\n", ret);
    } else {
        g_ntw_addr = load_addr;
    }

    TEE_LOGI("static addr:0x%x, load_addr:0x%x, size:%d \n", static_addr, load_addr, size);

    memcpy((void *)load_addr, (void *)static_addr, size);

    TEE_LOGI("jump to %s: 0x%x \n", NTW_IMG_NAME, g_ntw_addr);

    rte_to_ntw();
}

int tee_main(void)
{
#ifdef CONFIG_TEE_DEBUG
    extern void console_init();
    console_init();
#endif

    kp_init();

    mtb_init();

    tee_info_dump();

    jump_to_ntw();

    while (1);

    return 1;
}

