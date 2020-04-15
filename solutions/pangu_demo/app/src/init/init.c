#include <yoc_config.h>

#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <devices/devicelist.h>
#include <devices/drv_snd_pangu.h>
#include "pin_name.h"
#include "app_init.h"
#include "pin.h"

const char *TAG = "INIT";

#ifndef CONSOLE_IDX
#define CONSOLE_IDX 0
#endif


#include <yoc/partition.h>
#define reg_wr(reg_addr) *((volatile uint32_t *)reg_addr)
extern void mdelay(uint32_t ms);

static void cpu1_init(void)
{
#define CPU1_RUN_ADDR   (0x18500000)
    partition_t p = partition_open("cpu1");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "cpu1->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "cpu1->length=0x%x", info->length);
    partition_read(p, 0, (void *)CPU1_RUN_ADDR, info->length);
    partition_close(p);

    // FIXME:
    reg_wr(0x30000080) = CPU1_RUN_ADDR;
    reg_wr(0x30000008) &= ~0x1;
    reg_wr(0x30000008) |= 0x1;
    mdelay(200);
}

static void cpu2_init(void)
{
#define CPU2_RUN_ADDR               (0x18600000)

    partition_t p = partition_open("cpu2");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "cpu2->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "cpu2->length=0x%x", info->length);
    partition_read(p, 0, (void *)CPU2_RUN_ADDR, info->length);
    partition_close(p);

    reg_wr(0x30000044) = CPU2_RUN_ADDR;
    reg_wr(0x30000008) &= ~0x02;
    reg_wr(0x30000008) |= 0x02;
    mdelay(200);
}

static void itcm_code_load()
{
extern int __data_end__;
extern int __itcm_code_start__;
extern int __itcm_code_end__;
uint32_t len = &__itcm_code_end__ - &__itcm_code_start__;
    memset((void *)(&__itcm_code_start__), 0, 0x7FFF);
    memcpy((void *)(&__itcm_code_start__), \
        (uint8_t *)(&__data_end__), \
        len * 4);
}

// static void cpu2_dnn_init(void)
// {
// #define CPU2_DNN_ADDR               (0x18680000)

//     partition_t p = partition_open("cpu2-dnn");
//     partition_info_t *info = hal_flash_get_info(p);
//     if (info) {
//         LOGD(TAG, "cpu2-dnn->start_addr=0x%x", info->start_addr);
//         LOGD(TAG, "cpu2-dnn->length=0x%x", info->length);
//         partition_read(p, 0, (void *)CPU2_DNN_ADDR, info->length);        
//     }
//     partition_close(p);
// }

void board_init(void)
{
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);
    /** for wifi sdio */
    drv_pinmux_config(PA13, PA13_SDIO_DAT2);
    drv_pinmux_config(PA14, PA14_SDIO_DAT3);
    drv_pinmux_config(PA15, PA15_SDIO_CMD); //ok
    drv_pinmux_config(PA16, PA16_SDIO_CLK);
    drv_pinmux_config(PA17, PA17_SDIO_DAT0);
    drv_pinmux_config(PA18, PA18_SDIO_DAT1);

    drv_pinmux_config(BT_UART_TXD, BT_UART_TXD_FUNC);
    drv_pinmux_config(BT_UART_RXD, BT_UART_RXD_FUNC);
}

void speaker_init()
{
    drv_pinmux_config(PANGU_PA, PIN_FUNC_GPIO);
    gpio_pin_handle_t pgpio_pin_handle = csi_gpio_pin_initialize(PANGU_PA, NULL);
    csi_gpio_pin_config_mode(pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pgpio_pin_handle, true);
}

void speaker_uninit()
{
    drv_pinmux_config(PANGU_PA, PIN_FUNC_GPIO);
    gpio_pin_handle_t pgpio_pin_handle = csi_gpio_pin_initialize(PANGU_PA, NULL);
    csi_gpio_pin_config_mode(pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pgpio_pin_handle, false);
}

void board_yoc_init()
{
    itcm_code_load();
    uart_csky_register(0);
    uart_csky_register(1);

    spiflash_csky_register(0);

    console_init(CONSOLE_IDX, 115200, 512);

    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);

    /* load partition */
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        cpu1_init();
        // cpu2_dnn_init();
        cpu2_init();

        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");

    snd_card_register(6300);
    speaker_init();

    event_service_init(NULL);
    /* uService init */
    utask_t *task = utask_new("at&cli", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    board_cli_init(task);
}
