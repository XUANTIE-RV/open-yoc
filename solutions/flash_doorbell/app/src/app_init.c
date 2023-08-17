/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <board.h>
#include <board_config.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <uservice/uservice.h>
#include <cvi_misc.h>
#include "app_main.h"
#include "yoc/ota_ab.h"
#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
#include <bengine/bengine_dload.h>
#include <bengine/bestat.h>

extern void *_mmu_post_text_start;
extern void *_mmu_post_text_len;
extern void *_mmu_post_rodata_start;
extern void *_mmu_post_rodata_len;
extern void *_mmu_post_data_start;
extern void *_mmu_post_data_len;
#endif

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    int ret;

    board_init();

    board_audio_init();

    stduart_init();
    extern void  cxx_system_init(void);
    cxx_system_init();

    ulog_init();

#ifdef CONFIG_DEBUG
    aos_set_log_level(AOS_LL_DEBUG);
#else
    aos_set_log_level(AOS_LL_WARN);
#endif

    ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition_init failed(%d).\n", ret);
        aos_assert(false);
        return;
    }
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    extern int bootab_init(void);
    if (bootab_init() < 0) {
        LOGE(TAG, "bootab init failed.");
        aos_assert(false);
    }
#endif
#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
    {
        bdl_reg_config_t config = {0};
        config.layout.text_start   = (void*)(&_mmu_post_text_start);
        config.layout.text_size    = (size_t)(&_mmu_post_text_len);
        config.layout.rodata_start = (void*)(&_mmu_post_rodata_start);
        config.layout.rodata_size  = (size_t)(&_mmu_post_rodata_len);
        config.layout.data_start   = (void*)(&_mmu_post_data_start);
        config.layout.data_size    = (size_t)(&_mmu_post_data_len);
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
        snprintf(config.part_name, sizeof(config.part_name), "prim%s", otaab_get_current_ab());
#else
        strcpy(config.part_name, "prim");
#endif

        bengine_dload_init();
        bengine_dload_reg_kernel_elf_post(&config);
    }
#endif
    event_service_init(NULL);
    return;
}
