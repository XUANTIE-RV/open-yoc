/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_OTA_SERVER_TRANS_HCI_UART) && CONFIG_OTA_SERVER_TRANS_HCI_UART

#include "ota_server.h"
#include "ais_ota/ais_ota_server.h"
#include "hci_driver/h5.h"
#include "aos/kernel.h"
#include "common/log.h"
#include "ota_trans/ota_server_trans_hci_uart.h"

#define TAG "OTA_TRANS_UART"

#define HCI_VSC_DOWNLOAD_FW_PATCH 0xFC20
#define CMD_BUF_SIZE              BT_BUF_RX_SIZE
#define H5_ACK_PKT                0x00
#define HCI_COMMAND_PKT           0x01
#define HCI_ACLDATA_PKT           0x02
#define HCI_SCODATA_PKT           0x03
#define HCI_EVENT_PKT             0x04
#define H5_VDRSPEC_PKT            0x0E
#define DEF_MTU_SIZE              80

typedef void (*h5_ota_recv_cb)(uint8_t type, uint8_t *data, uint16_t len);

static ota_server_event_cb g_ota_server_cb = NULL;

#define DEF_UART_CHANN_CHECK_TIMEOUT 1000

enum
{
    CMD_START = 0x01,
    CMD_STOP  = 0x02,
    CMD_DATA  = 0x03,
} cmd_ops;

enum
{
    CMD_SUCCESS          = 0x00,
    CMD_FAIL_ALREADY     = 0x01,
    CMD_FAIL_BUSY        = 0x02,
    CMD_FAIL_INVALID_CMD = 0x03,
    CMD_FAIL_NOT_START   = 0x04,
} cmd_ops_status;

enum
{
    TRANS_IDLE  = 0x00,
    TRANS_START = 0x01,
    TRANS_BUSY  = 0X02,
} trans_status;

typedef struct {
    uint16_t cmd;
    uint8_t  len;
    uint8_t  ops;
    uint8_t  data_len;
} __packed fw_upgrade_cmd;

typedef struct {
    uint8_t ops_status;
    uint8_t ops;
} __packed fw_upgrade_cmd_status;

typedef struct {
    uint8_t chan_status;
    // aos_timer_t  check_timer;
    upgrade_device *device;
} trans_hci_uart_ctx;

trans_hci_uart_ctx g_uart_ctx;

static void _ais_ota_msg_handle(ais_pdu_t *p_msg, uint16_t len)
{
    LOGD(TAG, "cmd %02x", p_msg->header.cmd);
    // LOGD(TAG, "len %d: %s", len, bt_hex(p_msg, len));

    switch (p_msg->header.cmd) {
        case AIS_OTA_VER_RESP:
            if (g_ota_server_cb) {
                g_ota_server_cb(AIS_OTA_VER_RESP, p_msg);
            }

            break;

        case AIS_OTA_UPD_RESP:
            if (g_ota_server_cb) {
                g_ota_server_cb(AIS_OTA_UPD_RESP, p_msg);
            }

            break;

        case AIS_OTA_STATUS:
            if (g_ota_server_cb) {
                g_ota_server_cb(AIS_OTA_STATUS, p_msg);
            }

            break;

        case AIS_OTA_CHECK_RESP:
            if (g_ota_server_cb) {
                g_ota_server_cb(AIS_OTA_CHECK_RESP, p_msg);
            }

            break;

        case AIS_RESP_ERR: {
            if (g_ota_server_cb) {
                g_ota_server_cb(AIS_RESP_ERR, p_msg);
            }
        } break;

        default:
            /* recv some unsupport cmd, just return */
            LOGW(TAG, "unsupport cmd %x", p_msg->header.cmd);
            return;
    }
}

static void ota_cmd_event_process(uint8_t *data, uint8_t len)
{
    fw_upgrade_cmd_status *status = (fw_upgrade_cmd_status *)data;

    switch (status->ops) {
        case CMD_START: {
            if (status->ops_status == CMD_SUCCESS) {
                g_uart_ctx.chan_status = TRANS_START;
                uint8_t mtu            = DEF_MTU_SIZE;

                if (g_ota_server_cb) {
                    g_ota_server_cb(AIS_OTA_SERVER_STATE_CHAN_READY, &mtu);
                }

                if (g_ota_server_cb) {
                    g_ota_server_cb(AIS_OTA_SERVER_STATE_ONGOING, &g_uart_ctx.device->device.addr);
                }

            } else {
                LOGE(TAG, "ota trans uart start fail %d", status->ops_status);
                // TODO
            }
        } break;

        case CMD_STOP: {
            if (status->ops_status == CMD_SUCCESS) {
                g_uart_ctx.chan_status = TRANS_IDLE;
            } else {
                LOGE(TAG, "ota trans uart stop fail %d", status->ops_status);
                // TODO
            }
        } break;

        case CMD_DATA: {
            data += sizeof(fw_upgrade_cmd_status);
            len = len - sizeof(fw_upgrade_cmd_status);
            _ais_ota_msg_handle((ais_pdu_t *)data, len);
        }

        default:
            break;
    }
}

void uart_recv_cb(uint8_t type, uint8_t *data, uint16_t len)
{
    if (type == HCI_EVENT_PKT) {
        ota_cmd_event_process(data, len);
    } else {
        LOGE(TAG, "unexpect event type");
    }

    return;
}

static int h5_send(uint8_t type, uint8_t *data, uint16_t length)
{
    const h5_t *h5_ctx = NULL;
    h5_ctx             = get_h5_interface();

    if (!h5_ctx) {
        LOGE(TAG, "open h5 interface failed");
        return -1;
    }

    h5_ctx->h5_send_cmd(type, data, length);
    return 0;
}

int hci_api_upda_cmd(uint8_t op)
{
    fw_upgrade_cmd cmd = {
        .cmd      = 0xFC20,
        .len      = 2,
        .ops      = op,
        .data_len = 0,
    };

    return h5_send(H5_VDRSPEC_PKT, (uint8_t *)&cmd, sizeof(fw_upgrade_cmd));
}

int ota_server_trans_hci_uart_init(ota_server_event_cb cb)
{
    memset(&g_uart_ctx, 0x00, sizeof(trans_hci_uart_ctx));
    // aos_timer_new_ext(&g_uart_ctx.check_timer, chan_check_cb, NULL, DEF_UART_CHANN_CHECK_TIMEOUT, 1,0);
    g_ota_server_cb = cb;
    return 0;
}

int ota_server_trans_hci_uart_prepare(slist_t *dev_list)
{
    if (!dev_list) {
        return -EINVAL;
    }

    extern void h5_ota_cb_register(h5_ota_recv_cb cb);
    h5_ota_cb_register(uart_recv_cb);

    g_uart_ctx.device = NULL;
    g_uart_ctx.device = dev_list_dev_get(dev_list);

    if (!g_uart_ctx.device) {
        LOGE(TAG, "no ota dev found using channel uart");
        return -1;
    }

    hci_api_upda_cmd(CMD_START);
    return 0;
}

int ota_server_trans_hci_uart_unregister()
{
    ota_server_trans_hci_uart_disconnect();
    extern void h5_ota_cb_unregister();
    h5_ota_cb_unregister();
    memset(&g_uart_ctx, 0x00, sizeof(trans_hci_uart_ctx));
    return 0;
}

int ota_server_trans_hci_uart_reset()
{
    return 0;
}

int ota_server_trans_hci_uart_disconnect()
{
    hci_api_upda_cmd(CMD_STOP);
    return 0;
}

int ota_server_trans_hci_uart_send(uint8_t *data, uint8_t len, uint8_t ack)
{
    int            ret = 0;
    fw_upgrade_cmd cmd = {
        .cmd      = 0xFC20,
        .len      = 2 + len,
        .ops      = CMD_DATA,
        .data_len = len,
    };
    extern uint16_t hci_h5_send_cmd_with_header(hci_data_type_t type, uint8_t * header, uint8_t header_len,
                                                uint8_t * data, uint16_t length);
    ret = hci_h5_send_cmd_with_header(H5_VDRSPEC_PKT, (uint8_t *)&cmd, sizeof(fw_upgrade_cmd), data, len);

    if (ret < 0) {
        return -1;
    }

    return 0;
}

#endif
