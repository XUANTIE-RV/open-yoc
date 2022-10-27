/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_USE_LOG_IPC) && CONFIG_USE_LOG_IPC
#include <stdio.h>
#include <math.h>
#include <aos/aos.h>
#include <aos/ringbuffer.h>
#include <k_api.h>
#include <ipc.h>
#include <csi_core.h>
#include <aos/kernel.h>
#include "log_ipc.h"
#include <csi_core.h>

static aos_event_t g_output_sem;

static dev_ringbuf_t log_tx_dev;
static char log_tx_buffer[40 * KSIZE];
static int dbg_console_flag = 1;

static dev_ringbuf_t log_rx_dev;
static char log_rx_buffer[KSIZE];

static ipc_t *_debug_console_ipc;

int ipc_log_rx_read(const uint8_t * buffer, uint32_t size)
{
    return ringbuffer_read(&log_rx_dev, (uint8_t *)buffer, size);
}

static int log_rx_write(char *cmd, int len)
{
    if (cmd == NULL || len == 0)
        return -1;

    if (ringbuffer_available_write_space(&log_rx_dev) < len)
        return -1;

    ringbuffer_write(&log_rx_dev, (uint8_t *)cmd, len);

    return 0;
}


static int log_tx_read(char *data, int len)
{
    int ret = 0;
    int read_len = len;

    if (dbg_console_flag == 0) {
        return ret;
    }

    ret = ringbuffer_read(&log_tx_dev, (uint8_t *)data, read_len);
    if (ret == -1) {
        ret = 0;
    }

    return ret;
}

//Console输出给CPU0
console_ipcdata_t ipc_data;
static int _debug_console_ipc_send(uint16_t command, uint8_t flag, char *data, int len)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    // console_ipcdata_t ipc_data = {(size_t)data, len};
    ipc_data.data = (size_t)data;
    ipc_data.len = len;

    msg.service_id  = DBG_CONSOLE_IPC_SERIVCE_ID;
    msg.command     = command;
    msg.req_data    = (int64_t *)&ipc_data;
    msg.req_len     = sizeof(console_ipcdata_t);
    msg.flag        = flag;

    ipc_message_send(_debug_console_ipc, &msg, AOS_WAIT_FOREVER);

    return 0;
}

int ipc_log_tx_write(const uint8_t * buffer, uint32_t size)
{
    if (dbg_console_flag == 0) {
        return -1;
    }

    // FIXME： Will miss data when ringbuff full
    int ret = ringbuffer_write(&log_tx_dev, (uint8_t * )buffer, size);

    aos_event_set(&g_output_sem, 1, AOS_EVENT_OR);

    return ret;
}

static char console_data[KSIZE * 2 + 64] __attribute__((aligned(64)));
static void console_tx_task(void *priv)
{
    int actual_len = 0;
    unsigned int actl_flags = 0;


    while(1) {
        aos_event_get(&g_output_sem, 1, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);

        while (ringbuffer_available_read_space(&log_tx_dev)) {

            actual_len = log_tx_read(console_data, sizeof(console_data) - 64);

            /* set eof */
            console_data[actual_len] = 0;

            csi_dcache_clean_range((size_t*)console_data, sizeof(console_data));

            if (actual_len > 0) {
                _debug_console_ipc_send(IPC_CMD_DEBUG_CONSOLE_INFO, MESSAGE_ASYNC, console_data, actual_len + 1);
            }
        }
    }
}

static void console_tx_func(void)
{
    aos_event_new(&g_output_sem, 0);

    aos_task_t task_handle;
    aos_task_new_ext(&task_handle, "console_tx_task", console_tx_task, NULL, 2 * 1024, AOS_DEFAULT_APP_PRI + 10);

    //初始化log ringbuffer
    ringbuffer_create(&log_tx_dev, log_tx_buffer, sizeof(log_tx_buffer));
}

static void _debug_console_ipc_process(ipc_t *ipc, message_t *m, void *priv)
{
    switch (m->command) {
        case IPC_CMD_DEBUG_CMDINFO:
            dbg_console_flag = 1;
            log_rx_write((char *)m->req_data, m->req_len);
            ipc_log_read_event();
            break;

        case IPC_CMD_DEBUG_CONSOLE_START:
            dbg_console_flag = 1;
            break;

        case IPC_CMD_DEBUG_CONSOLE_STOP:
            dbg_console_flag = 0;
            break;

        default:
            break;
    }

    if (m->flag & MESSAGE_SYNC) {
        ipc_message_ack(_debug_console_ipc, m, AOS_WAIT_FOREVER);
    }
}

static void __except_process(int errno, const char *file, int line, const char *func_name, void *caller)
{
    int actual_len = 0;

    printf("Except! errno is %s, function: %s at %s:%d, caller: %p\n", strerror(errno),
           func_name ? func_name : "null", file ? file : "null", line, caller ? caller : NULL);

    csi_irq_save();
    aos_kernel_sched_suspend();
    actual_len = log_tx_read(console_data, sizeof(console_data) - 16);

    /* set eof */
    console_data[actual_len] = 0;

    csi_dcache_clean_range((size_t*)console_data, sizeof(console_data));

    _debug_console_ipc_send(IPC_CMD_DEBUG_EXCEPT_INFO, MESSAGE_ASYNC, console_data, actual_len + 1);

    while (1);
}


/* CPU1与CPU0之间DBG IPC初始化 */
int ipc_log_cp_init(int cpu_id)
{
    aos_set_except_callback(__except_process);

    //console的串口打印给CPU0
    _debug_console_ipc = ipc_get(cpu_id);
    ipc_add_service(_debug_console_ipc, DBG_CONSOLE_IPC_SERIVCE_ID, _debug_console_ipc_process, NULL);

    console_tx_func();

    ringbuffer_create(&log_rx_dev, log_rx_buffer, sizeof(log_rx_buffer));

    return 0;
}
#endif //#if defined(CONFIG_USE_LOG_IPC) && CONFIG_USE_LOG_IPC