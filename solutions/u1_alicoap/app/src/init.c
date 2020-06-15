#include <stdbool.h>
#include <yoc/yoc.h>
#include "pin_name.h"
#include "pin.h"
#include "app_main.h"
#include <devices/devicelist.h>
#include <vfs.h>
#include <soc.h>
#include <posix_init.h>
#include <addrenv.h>
#include <song_rptun.h>
#include <syslog_rpmsg.h>
#include <misc_rpmsg.h>
#include <uart_rpmsg.h>
#include <usrsock.h>
#include <usrsock_rpmsg.h>
#include <hostfs_rpmsg.h>
#include <lwip/sockets.h>
#include <devices/u1.h>
#include <yoc/netmgr_service.h>

const char *TAG = "INIT";

#ifndef CONSOLE_IDX
#define CONSOLE_IDX 0
#endif
netmgr_hdl_t app_netmgr_hdl;

void up_earlyinitialize(void)
{
    static const struct simple_addrenv_s addrenv[] =
    {
        {.va = 0x21000000, .pa = 0xb1000000, .size = 0x00100000},
        {.va = 0x00000000, .pa = 0x00000000, .size = 0x00000000},
    };

    simple_addrenv_initialize(addrenv);
}

static void up_misc_init(void)
{
    int fd;

    /* Retention init */

    misc_rpmsg_initialize(CPU_NAME_SP, true);

    fd = aos_open("/dev/misc", 0);
    if (fd >= 0) {
        /* Get board-id env from sp */

        struct misc_remote_envsync_s env =
        {
            .name = "board-id",
        };

        aos_ioctl(fd, MISC_REMOTE_ENVSYNC, (unsigned long)&env);
        aos_close(fd);
    }
}

static void up_rptun_init(void)
{
    static const struct song_rptun_config_s rptun_cfg[CONFIG_RPTUN_NUM] =
    {
        {
            .cpuname = CPU_NAME_CP,
            .rsc     = (struct rptun_rsc_s *)0xb0003400,
            .master  = true,
            .mailbox = CPU_INDEX_CP,
        },
        {
            .cpuname = CPU_NAME_SP,
            .rsc     = (struct rptun_rsc_s *)0xb0000000,
            .mailbox = CPU_INDEX_SP,
        },
    };

    song_rptun_initialize(rptun_cfg);

    syslog_rpmsg_server_init();

    up_misc_init();

#ifdef CONFIG_SERIAL_USE_VFS
    uart_rpmsg_init(CPU_NAME_SP, "SP", 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "CP", 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "AT", 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "AT1", 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "GPS", 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "GPS1", 256, false);
#else
    uart_rpmsg_init(CPU_NAME_SP, "SP", 4, 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "CP", 5, 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "AT", 6, 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "AT1", 7, 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "GPS", 8, 256, false);
    uart_rpmsg_init(CPU_NAME_CP, "GPS1", 9, 256, false);
#endif

    hostfs_rpmsg_init(CPU_NAME_SP);
}

void lpm_event_cb(pm_policy_t policy, lpm_event_e event)
{
    LOGI(TAG, "%s, policy %d, event %d, line %d\n", __func__, policy, event, __LINE__);
}

void board_init(void)
{
//    board_pinmux_config();
}

void ipcflash_csky_register(int idx);

void board_yoc_init(void)
{
    event_service_init(NULL);
    uart_csky_register(CONSOLE_IDX);
    console_init(CONSOLE_IDX, 921600, 128);

    up_earlyinitialize();

    posix_init();
    vfs_init();
    ipcflash_csky_register(0);

    up_rptun_init();

    usrsock_initialize();
    aos_task_new("usrsock", usrsock_main, CPU_NAME_SP, 2048);

    u1_nbiot_param_t nbiot_param;
    nbiot_param.device_name = "uart6";
    nbiot_u1_register(NULL, &nbiot_param);
    app_netmgr_hdl = netmgr_dev_nbiot_init();
    if (app_netmgr_hdl) {
        netmgr_service_init(NULL);
        netmgr_start(app_netmgr_hdl);
    }

    pm_init(lpm_event_cb);

    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);
    aos_kv_flash_init("eflash0", 512, 6);

    /* uService init */
    utask_t *task = utask_new("at&cli", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    board_cli_init(task);
}
