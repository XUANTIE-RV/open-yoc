/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if !defined(CONFIG_LOG_IPC_CP) || (CONFIG_LOG_IPC_CP == 0)

#include <stdint.h>
#include <aos/aos.h>
#include <aos/ringbuffer.h>
#include <k_api.h>
#include <csi_core.h>
#include "log_ipc.h"

#define TAG "DBG_CONSOLE"

#define printk printf

typedef struct {
    ipc_t*     ipc;
    uint8_t    cpu_id;
} log_ipc_t;

static log_ipc_t *g_log_ipc;
static int        g_log_ipc_num;

static log_ipc_t *find_by_cpuid(uint8_t cpu_id) 
{   
    log_ipc_t *log_ipc = g_log_ipc;
    for (int i = 0; i < g_log_ipc_num; i++) {
        if (cpu_id == log_ipc[i].cpu_id) {
            return &log_ipc[i];
        }
    }

    return NULL;
}

int log_ipc_cmd_send(uint8_t cpu_id, uint16_t cmd, char *cmd_data, int cmd_len)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.service_id  = DBG_CONSOLE_IPC_SERIVCE_ID;
    msg.command     = cmd;
    msg.req_data    = cmd_data;
    msg.req_len     = cmd_len;
    msg.flag        = MESSAGE_SYNC;

    log_ipc_t *log_ipc = find_by_cpuid(cpu_id);
    aos_check(log_ipc, -1);

    ipc_message_send(log_ipc->ipc, &msg, AOS_WAIT_FOREVER);

    return 0;
}

static void console_ipc_process(ipc_t *ipc, message_t *m, void *priv)
{
    log_ipc_t *log_ipc = (log_ipc_t *)priv;
    console_ipcdata_t *ipc_data = (console_ipcdata_t *)m->req_data; 
    csi_dcache_invalid_range((size_t *)ipc_data->data, ipc_data->len);

    switch (m->command) {
        case IPC_CMD_DEBUG_CONSOLE_INFO:
            ulog(LOG_DEBUG, TAG, ULOG_TAG, "CPU(%d):\r\n%s\r\n", log_ipc->cpu_id, (char *)ipc_data->data);
        break;

        case IPC_CMD_DEBUG_EXCEPT_INFO:
            printk("!!!!CPU(%d): EXCEPT!!!!\r\n%s\r\nCPU(%d) END\r\n", log_ipc->cpu_id, (char *)ipc_data->data, log_ipc->cpu_id);
            k_err_proc_debug(0, (char *)__func__, __LINE__);
        break;

        default:
            LOGE(TAG, "CPU(%d): unkown cmd: %d\r\n", log_ipc->cpu_id, m->command);
        break;
    }

    if (m->flag & MESSAGE_SYNC) {
        ipc_message_ack(log_ipc->ipc, m, AOS_WAIT_FOREVER);
    }
}

//dbg IPC初始化
int log_ipc_ap_init(uint8_t *cpu_id, int num)
{
    //CPU1 console的串口打印给CPU0
    g_log_ipc = calloc(sizeof(log_ipc_t), num);
    aos_check(g_log_ipc, -1);

    g_log_ipc_num = num;
    for (int i = 0; i < num ; i++) {
        g_log_ipc[i].ipc    = ipc_get(cpu_id[i]);
        g_log_ipc[i].cpu_id = cpu_id[i];
        ipc_add_service(g_log_ipc[i].ipc, DBG_CONSOLE_IPC_SERIVCE_ID, console_ipc_process, &g_log_ipc[i]);
    }

    return 0;
}
#endif
