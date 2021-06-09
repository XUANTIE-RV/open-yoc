
#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <devices/devicelist.h>
#include <devices/drv_snd_pangu.h>
#ifdef CONFIG_CSI_V2
#include <drv/gpio_pin.h>
#include <drv/pin.h>
#else
#include "pinmux.h"
#include "pin_name.h"
#endif
#include "app_init.h"
#include "board.h"

#include "app_main.h"

const char *TAG = "INIT";

#ifndef CONSOLE_UART_IDX
#define CONSOLE_UART_IDX 0
#endif


#include <yoc/partition.h>
#define reg_wr(reg_addr) *((volatile uint32_t *)reg_addr)
extern void mdelay(uint32_t ms);
void cpu1_init(void)
{
    partition_t p = partition_open("cpu1");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "cpu1->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "cpu1->length=0x%x", info->length);
    LOGD(TAG, "cpu1 load_addr=0x%x", info->load_addr);
    if (info->load_addr == ~0) {
        LOGE(TAG, "cpu1 load addr error!");
        return;
    }
    partition_read(p, 0, (void *)info->load_addr, info->length);
    partition_close(p);

    // FIXME:
    reg_wr(0x30000080) = info->load_addr;
    reg_wr(0x30000008) &= ~0x1;
    reg_wr(0x30000008) |= 0x1;
    // mdelay(200);
}

void cpu2_init(void)
{
    partition_t p = partition_open("cpu2");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "cpu2->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "cpu2->length=0x%x", info->length);
    LOGD(TAG, "cpu2 load_addr=0x%x", info->load_addr);
    if (info->load_addr == ~0) {
        LOGE(TAG, "cpu2 load addr error!");
        return;
    }
    partition_read(p, 0, (void *)info->load_addr, info->length);
    partition_close(p);
    reg_wr(0x30000044) = info->load_addr;
    reg_wr(0x30000008) &= ~0x02;
    reg_wr(0x30000008) |= 0x02;
    // mdelay(200);
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

void speaker_init()
{
#ifdef CONFIG_CSI_V2
    csi_gpio_pin_t pgpio_pin_handle;
    memset(&pgpio_pin_handle, 0, sizeof(pgpio_pin_handle));
    csi_pin_set_mux(PANGU_PA, PIN_FUNC_GPIO);
    csi_gpio_pin_init(&pgpio_pin_handle, PANGU_PA);
    csi_gpio_pin_mode(&pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_dir(&pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(&pgpio_pin_handle, true);
#else
    drv_pinmux_config(PANGU_PA, PIN_FUNC_GPIO);
    gpio_pin_handle_t pgpio_pin_handle = csi_gpio_pin_initialize(PANGU_PA, NULL);
    csi_gpio_pin_config_mode(pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pgpio_pin_handle, true);
#endif
}

void speaker_uninit()
{
#ifdef CONFIG_CSI_V2
    csi_pin_set_mux(PANGU_PA, PIN_FUNC_GPIO);
    csi_gpio_pin_t pgpio_pin_handle;
    memset(&pgpio_pin_handle, 0, sizeof(pgpio_pin_handle));
    csi_gpio_pin_init(&pgpio_pin_handle, PANGU_PA);
    csi_gpio_pin_mode(&pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_dir(&pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(&pgpio_pin_handle, false);
#else
    drv_pinmux_config(PANGU_PA, PIN_FUNC_GPIO);
    gpio_pin_handle_t pgpio_pin_handle = csi_gpio_pin_initialize(PANGU_PA, NULL);
    csi_gpio_pin_config_mode(pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pgpio_pin_handle, false);
#endif
}

void board_yoc_init()
{
    board_init();
    itcm_code_load();

    console_init(CONSOLE_UART_IDX, 115200, 512);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);

    /* load partition */
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        cpu1_init();
        cpu2_init();

        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");

    snd_card_register(NULL);
    speaker_init();

    event_service_init(NULL);

    board_cli_init();
}
