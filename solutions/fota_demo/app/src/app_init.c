#include <stdbool.h>
#include <board.h>
#include <aos/kv.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <uservice/uservice.h>
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
#include <debug/dbg.h>
#endif
#include "app_main.h"
#include <devices/spi.h>
#if defined(CONFIG_WITH_SE)
#include <se_device.h>
#endif

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

#if defined(CONFIG_WITH_SE)
static int se_drv_init(void)
{
    se_dev_t *dev = NULL;
    rvm_hal_spi_config_t spi_config;

    int ret;

    rvm_hal_spi_default_config_get(&spi_config);
	spi_config.mode = RVM_HAL_SPI_MODE_MASTER;
	spi_config.freq = 1 * 1000 * 1000; //10 * 1000000; // 10M
	spi_config.format = RVM_HAL_SPI_FORMAT_CPOL0_CPHA1;

	ret =  se_init(dev, 0, SE_IF_TYPE_SPI, &spi_config);
    CHECK_RET_WITH_RET(ret == 0, ret);

    printf("se_drv_init finish\n");

    return 0;
}
#endif

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    board_cli_init();
    printf("\n###Welcome to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    event_service_init(NULL);
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
    aos_debug_init();
#endif
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
        aos_assert((ret > 0));
    } else {
        LOGI(TAG, "find %d partitions", ret);
        ret = aos_kv_init("kv");
        if (ret != 0) {
            LOGE(TAG, "kv init failed.");
            aos_assert((ret == 0));
        }
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
        extern int bootab_init(void);
        if (bootab_init() < 0) {
            LOGE(TAG, "bootab init failed.");
        }
#endif
        extern uint32_t km_init(void);
        km_init();
#if defined(CONFIG_WITH_SE)
		extern int se_drv_init(void);
		se_drv_init();
#endif
    }
    app_network_init();
}
