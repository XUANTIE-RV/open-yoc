/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#ifndef LOG_IPC_H
#define LOG_IPC_H

#include <ipc.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct console_ipc {
    uint64_t  data;
    uint32_t  len;
} console_ipcdata_t;

#define KSIZE       1024

//DBG SERVICE
#define DBG_CPU1_DATA_IPC_SERIVCE_ID    (0x53)
#define DBG_CPU2_DATA_IPC_SERIVCE_ID    (0x54)

#define DBG_CONSOLE_IPC_SERIVCE_ID      (0x55)

//cli cmd
#define IPC_CMD_DEBUG_CMDINFO              0x1F

//cpu 1 console start/ stop
#define IPC_CMD_DEBUG_CONSOLE_START        0x21
#define IPC_CMD_DEBUG_CONSOLE_STOP         0x22

//printf
#define IPC_CMD_DEBUG_CONSOLE_INFO         0x25
#define IPC_CMD_DEBUG_EXCEPT_INFO          0x26

//cpu1 dbg data info
#define IPC_CMD_DEBUG_DATAINFO_CPU1        0x1A
#define IPC_CMD_DEBUG_DATA_START_CPU1      0x1B
#define IPC_CMD_DEBUG_DATA_STOP_CPU1       0x1C

//cpu2 dbg data info
#define IPC_CMD_DEBUG_DATAINFO_CPU2        0x27
#define IPC_CMD_DEBUG_DATA_START_CPU2      0x28
#define IPC_CMD_DEBUG_DATA_STOP_CPU2       0x29

#if defined(CONFIG_LOG_IPC_CP) && CONFIG_LOG_IPC_CP
void log_ipc_uart_register(int idx, int (*read)(const uint8_t *buf, uint32_t size),
                                   int (*write)(const uint8_t *data, uint32_t size));

int log_ipc_rx_read(const uint8_t * buffer, uint32_t size);
int log_ipc_tx_write(const uint8_t * buffer, uint32_t size);
void log_ipc_read_event(void);

int log_ipc_cp_init(int cpu_id);
#else
int log_ipc_ap_init(uint8_t *cpu_id, int num);
int log_ipc_cmd_send(uint8_t cpu_id, uint16_t cmd, char *cmd_data, int cmd_len);
#endif

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus


#endif //LOG_IPC_H
