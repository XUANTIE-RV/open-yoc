/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <aos/aos.h>
#include <board.h>
#include <k_api.h>
#include <board.h>
#include <pinmux.h>
#include "wm_psram.h"

/* PSRAM */
#define PSRAM_CK_IO     PB0
#define PSRAM_CS_IO     PB1
#define PSRAM_DAT0_IO   PB2
#define PSRAM_DAT1_IO   PB3
#define PSRAM_DAT2_IO   PB4
#define PSRAM_DAT3_IO   PB5
#define PSRAM_CK_FUNC   PB0_PSRAM_CK
#define PSRAM_CS_FUNC   PB1_PSRAM_CS
#define PSRAM_DAT0_FUNC PB2_PSRAM_DAT0
#define PSRAM_DAT1_FUNC PB3_PSRAM_DAT1
#define PSRAM_DAT2_FUNC PB4_PSRAM_DAT2
#define PSRAM_DAT3_FUNC PB5_PSRAM_DAT3

extern size_t     __psram_data_start__;
extern size_t     __psram_data_end__;
extern k_mm_head *g_kmm_head;
k_mm_region_t     mm_region[] = {
    { (uint8_t *)&__psram_data_end__, (size_t)1024 * 6000 },
};

void board_preconfig()
{
    // PSRAM init
    drv_pinmux_config(PSRAM_CK_IO, PSRAM_CK_FUNC);
    drv_pinmux_config(PSRAM_CS_IO, PSRAM_CS_FUNC);
    drv_pinmux_config(PSRAM_DAT0_IO, PSRAM_DAT0_FUNC);
    drv_pinmux_config(PSRAM_DAT1_IO, PSRAM_DAT1_FUNC);
#ifdef SUPPORT_4BIT_PSRAM
    drv_pinmux_config(PSRAM_DAT2_IO, PSRAM_DAT2_FUNC);
    drv_pinmux_config(PSRAM_DAT3_IO, PSRAM_DAT3_FUNC);
#endif

    psram_init(0);

    memset((void *)&__psram_data_start__, 0, (uint32_t)&__psram_data_end__ - (uint32_t)&__psram_data_start__);
    krhino_add_mm_region(g_kmm_head, mm_region[0].start, mm_region[0].len);
}

void board_init(void)
{
    /* some borad preconfig */
    board_preconfig();

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
    board_gpio_pin_init();
#endif

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
    board_uart_init();
#endif

}
