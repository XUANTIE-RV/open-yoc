#include <stdbool.h>
#include "pin_name.h"
#include "board.h"
#include "app_main.h"
#include <yoc/init.h>

#ifndef CONSOLE_UART_IDX
#define CONSOLE_UART_IDX 0
#endif

#define TAG "init"

void board_yoc_init(void)
{
	board_init();

    console_init(CONSOLE_UART_IDX, 115200, 512);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    LOGI(TAG, "[YoC]Build:%s,%s",__DATE__, __TIME__);
    
    board_cli_init();
}
