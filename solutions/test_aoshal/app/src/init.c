#include <stdbool.h>
#include <soc.h>
#include "app_main.h"
#include <yoc/init.h>
#include <ulog/ulog.h>
#include <yoc/atserver.h>
#include <devices/uart.h>

#ifndef CONSOLE_IDX
#define CONSOLE_IDX 1
#endif

#define TAG "init"


//#define AT_HELP {"AT+HELP", at_cmd_callback}

extern atserver_cmd_t at_cmd[];

void at_cmd_callback(char *cmd, int type, char *data) {
        char format[1026];
        int cid;
        atserver_scanf("%s",&format);
        LOGD(TAG,"cmd is %s",cmd);
        AT_BACK_OK();
}

static void send_atserver(char *format){
    int ret;
    ret = atserver_send("atserver send content is %s",format);
    if(ret==0){
        LOGD(TAG,"atserver send success!");
    }else{
        LOGD(TAG,"atserver send failed!");
    }
}

void board_yoc_init(void)
{
    board_init();

    console_init(CONSOLE_IDX, 115200, 128);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    uart_config_t config;
    uart_config_default(&config);
    config.baud_rate = 115200;

    utask_t *task1 = utask_new("at_srv", 5 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    int ret = atserver_init(task1, "uart1", &config);

    if(ret == 0){
        LOGD(TAG,"atserver init success!");
    }else{
        LOGD(TAG,"atserver init failed!\n");
    }
}