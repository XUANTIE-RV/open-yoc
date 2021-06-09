
#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <devices/devicelist.h>
#ifdef CONFIG_CSI_V1
#include "pin_name.h"
#else
#include "soc.h"
#endif
#include "app_init.h"
#include "board.h"
#include "ulog/ulog.h"

const char *TAG = "INIT";

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

void board_yoc_init()
{
	board_init();
    uart_csky_register(CONSOLE_UART_IDX);
    spiflash_csky_register(0);
    console_init(CONSOLE_UART_IDX, 115200, 512);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    LOGI(TAG, "[YoC]Build:%s,%s",__DATE__, __TIME__);

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

    event_service_init(NULL);
    board_cli_init();
}
