#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <csi_core.h>
#include "aos/cli.h"
#include "aos/kernel.h"

#include "drv/common.h"
#include "drv/timer.h"

void callback(void *arg)
{
    printf("xxxxx\n");
}

void cvi_timer_test(int32_t argc, char **argv)
{
    csi_timer_t timer1 = {0};

    csi_timer_init(&timer1, 0);
    csi_timer_attach_callback(&timer1, callback, NULL);
    csi_timer_start(&timer1, 1000*1000*2);

    aos_msleep(1000*10);
}
ALIOS_CLI_CMD_REGISTER(cvi_timer_test, timer, timer test);