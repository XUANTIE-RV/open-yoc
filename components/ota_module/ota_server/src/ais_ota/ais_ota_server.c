/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include "ulog/ulog.h"
#include "ota_server.h"
#include "ota_process.h"
#include "ais_ota/ais_ota_server.h"
#include "ota_trans/ota_server_trans.h"
#include "ais_ota.h"
#include "aos/kernel.h"
#include "ais_ota.h"
#include "errno.h"

#define TAG "AIS_OTA_SERVER"

#define AIS_PDU_SIZE(pdu) (sizeof(ais_header_t) + pdu->header.payload_len)

#define GET_FRAME_SIZE(total, offset, frame_payload_len, frame_max_size)                                               \
    ((((total) - (offset)) / ((frame_max_size) * (frame_payload_len)) >= 1)                                            \
         ? (frame_max_size)                                                                                            \
         : (((total) - (offset) + (frame_payload_len - 1)) / (frame_payload_len)))

// static ota_server_event_cb  g_ota_server_cb;

#define AIS_MAX_OTA_FRAME 16

typedef struct {
    ais_header_t header;
    uint8_t *    data;
    uint8_t      data_len;
} __attribute__((packed)) ais_pdu_tx_t;

typedef struct {
    mac_t   addr;
    uint8_t status;
} connect_status;

typedef struct {
    // for the dev
    upgrade_device *dev;
    uint32_t        rx_size;
    uint32_t        read_offset;
    uint8_t         total_seq;
    uint8_t         last_seq;
    uint8_t         total_frame;
    uint8_t         the_last_pdu_payload_size;
    uint8_t         need_retrans_flag;
    uint8_t         tx_pdu_payload_size; // mtu = tx_pdu_payload_size +sizeof(ais_header)
} _ais_ota_dev_info;

typedef struct {
    // for firmware
    firmware_info *firmware;
    uint8_t *      tx_buffer;
    uint8_t        need_remalloc_flag;
    uint8_t        total_frame_last; // the last dev total frame
    uint16_t       mtu_last;
    slist_t *      dev_list_head;
    uint16_t       index;
} _ais_ota_firmware_info;

typedef struct _ais_ota_server_ctx {
    uint8_t                init_flag;
    ais_ota_server_state   ota_state;
    _ais_ota_firmware_info fir_info;
    _ais_ota_dev_info      dev_info;
    aos_mutex_t            tx_in_flag_mutex;
    uint8_t                tx_in_flag;
    uint32_t               start_time;
} ais_ota_server_ctx;

ais_ota_server_ctx g_ais_ota_server_ctx;

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#if defined(CONFIG_DEBUG) && CONFIG_DEBUG > 0
static const char *log_hex(const void *buf, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char       str[256];
    const uint8_t *   b = buf;
    size_t            i;

    len = MIN(len, (sizeof(str) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}
#endif

static int ais_ota_server_state_set_fail(uint8_t reason, uint32_t new_version)
{
    int                     ret = 0;
    ota_process_cmd_message message;
    message.event = OTA_EVENT_FAILED;
    memcpy(&message.data.success_data.dev_info, &g_ais_ota_server_ctx.dev_info.dev->device, sizeof(device_info));
    message.data.fail_data.reason                = reason;
    message.data.fail_data.old_ver               = g_ais_ota_server_ctx.dev_info.dev->old_version;
    message.data.fail_data.new_ver               = new_version;
    message.data.fail_data.cost_time             = aos_now_ms() - g_ais_ota_server_ctx.dev_info.dev->start_time;
    message.data.fail_data.channel               = g_ais_ota_server_ctx.fir_info.firmware->ota_chanel;
    message.data.fail_data.firmware_index        = g_ais_ota_server_ctx.fir_info.index;
    g_ais_ota_server_ctx.dev_info.dev->ota_state = OTA_STATE_FAILED;
    memset(&g_ais_ota_server_ctx.dev_info, 0x00, sizeof(_ais_ota_dev_info));
    ret = ota_process_cmd_set(&message);
    if (ret) {
        LOGE(TAG, "ais ota server send fail state fail");
    }
    ais_ota_server_channel_disconnect();
    return 0;
}

static int ais_ota_server_state_set_conn_fail(upgrade_device *fail_device)
{
    int                     ret = 0;
    ota_process_cmd_message message;
    message.event = OTA_EVENT_FAILED;
    memcpy(&message.data.success_data.dev_info, &fail_device->device, sizeof(device_info));
    message.data.fail_data.reason                = OTA_FAIL_CONN;
    message.data.fail_data.old_ver               = OTA_VERSION_UNKNOW;
    message.data.fail_data.new_ver               = g_ais_ota_server_ctx.fir_info.firmware->version;
    message.data.fail_data.cost_time             = 0;
    message.data.fail_data.channel               = g_ais_ota_server_ctx.fir_info.firmware->ota_chanel;
    message.data.fail_data.firmware_index        = g_ais_ota_server_ctx.fir_info.index;
    g_ais_ota_server_ctx.dev_info.dev->ota_state = OTA_STATE_FAILED;
    ret                                          = ota_process_cmd_set(&message);
    if (ret) {
        LOGE(TAG, "ais ota server send conn fail state fail");
    }
    return 0;
}

static int ais_ota_server_state_set_success()
{
    int                     ret = 0;
    ota_process_cmd_message message;
    message.event = OTA_EVENT_SUCCESS;
    memcpy(&message.data.success_data.dev_info, &g_ais_ota_server_ctx.dev_info.dev->device, sizeof(device_info));
    message.data.success_data.old_ver = g_ais_ota_server_ctx.dev_info.dev->old_version;
    message.data.success_data.new_ver = g_ais_ota_server_ctx.fir_info.firmware->version;
    if (g_ais_ota_server_ctx.dev_info.dev->download_finish_time < g_ais_ota_server_ctx.dev_info.dev->start_time) {
        message.data.success_data.cost_time = 0;
    } else {
        message.data.success_data.cost_time
            = g_ais_ota_server_ctx.dev_info.dev->download_finish_time - g_ais_ota_server_ctx.dev_info.dev->start_time;
    }
    message.data.success_data.channel            = g_ais_ota_server_ctx.fir_info.firmware->ota_chanel;
    message.data.success_data.firmware_index     = g_ais_ota_server_ctx.fir_info.index;
    g_ais_ota_server_ctx.dev_info.dev->ota_state = OTA_STATE_SUCCESS;
    ota_process_cmd_set(&message);

    if (ret) {
        LOGE(TAG, "ais ota server send success state fail");
    }
    ais_ota_server_channel_disconnect();

    return 0;
}

static int ais_ota_server_state_set_ongoing()
{
    int ret = 0;

    ota_process_cmd_message message;
    message.event = OTA_EVENT_ONGOING;
    memcpy(&message.data.ongoing_data.dev_info, &g_ais_ota_server_ctx.dev_info.dev->device, sizeof(device_info));
    message.data.ongoing_data.channel             = g_ais_ota_server_ctx.fir_info.firmware->ota_chanel;
    message.data.ongoing_data.firmware_index      = g_ais_ota_server_ctx.fir_info.index;
    g_ais_ota_server_ctx.dev_info.dev->ota_state  = OTA_STATE_ONGOING;
    g_ais_ota_server_ctx.dev_info.dev->start_time = aos_now_ms();
    ret                                           = ota_process_cmd_set(&message);

    if (ret) {
        LOGE(TAG, "ota process state set failed");
        return ret;
    }
    return 0;
}

static inline int _ais_set_pdu(ais_pdu_t *msg, uint8_t enc, uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len)
{
    if (!msg) {
        return -1;
    }

    memset(msg, 0, sizeof(*msg));
    msg->header.enc    = 1;
    msg->header.msg_id = msg_id;
    msg->header.cmd    = cmd;
    // TODO
    msg->header.payload_len = len;

    if (p_msg) {
        memcpy(msg->payload, p_msg, len);
    }

    LOGD(TAG, "len %d: %s", len + 4, log_hex(msg, len + 4));
    return 0;
}

static int ais_ota_server_send(ais_pdu_t *pdu, uint8_t trans, uint8_t ack)
{
    return ota_server_trans_send(trans, (void *)pdu, AIS_PDU_SIZE(pdu), ack);
}

static int ais_ota_server_get_version(uint8_t trans)
{
    int       ret = 0;
    ais_pdu_t pdu;
    // TODO
    ais_ota_ver_req_t ver_req;
    ver_req.image_type = 0x00;
    ret                = _ais_set_pdu(&pdu, 0, 0, AIS_OTA_VER_REQ, (uint8_t *)&ver_req, sizeof(ais_ota_ver_req_t));

    if (ret) {
        LOGE(TAG, "set ais pdu failed");
        return -1;
    }

    return ais_ota_server_send(&pdu, trans, 0);
}

static int ais_ota_server_send_update_req(uint8_t trans)
{
    ais_ota_upd_req_t req;
    ais_pdu_t         pdu;
    int               ret = 0;
    // TODO
    req.image_type = g_ais_ota_server_ctx.fir_info.firmware->image_type;
    req.ver        = g_ais_ota_server_ctx.fir_info.firmware->version;
    req.fw_size    = g_ais_ota_server_ctx.fir_info.firmware->size;
    req.crc16      = g_ais_ota_server_ctx.fir_info.firmware->crc16;
    req.ota_flag   = g_ais_ota_server_ctx.fir_info.firmware->ota_flag;
    // TODO
    ret = _ais_set_pdu(&pdu, 0, 0, AIS_OTA_FIRMWARE_REQ, (uint8_t *)&req, sizeof(ais_ota_upd_req_t));

    if (ret) {
        return -1;
    }

    return ais_ota_server_send(&pdu, trans, 0);
}

static int ais_ota_server_handle_version(ais_pdu_t *pdu)
{

    ais_ota_ver_resp_t *p_ver_resp = (ais_ota_ver_resp_t *)(pdu->payload);

    int ret = 0;

    if (p_ver_resp->image_type == 0) {
        if (g_ais_ota_server_ctx.dev_info.dev->ota_state == OTA_STATE_LOAD_COMPLETE) {
            if (p_ver_resp->ver != g_ais_ota_server_ctx.fir_info.firmware->version) {
                LOGE(TAG, "upgrade fail, the device new version is %04x but the update firmware is %04x",
                     p_ver_resp->ver, g_ais_ota_server_ctx.fir_info.firmware->version);
                g_ais_ota_server_ctx.dev_info.dev->failed_reason = OTA_FAIL_REBOOT_ERR_VERSION;
                ais_ota_server_state_set_fail(OTA_FAIL_REBOOT_ERR_VERSION, p_ver_resp->ver);
            } else {
                LOGD(TAG, "upgrade success");
                ais_ota_server_state_set_success();
            }

            return 0;
        } else if (g_ais_ota_server_ctx.dev_info.dev->ota_state == OTA_STATE_ONGOING) {
            if (g_ais_ota_server_ctx.dev_info.dev->old_version != 0
                && p_ver_resp->ver != g_ais_ota_server_ctx.dev_info.dev->old_version)
            {
                LOGE(TAG, "Err version, the device actual version is %04x but dev old version is setted to %04x",
                     p_ver_resp->ver, g_ais_ota_server_ctx.fir_info.firmware->version);
                g_ais_ota_server_ctx.dev_info.dev->failed_reason = OTA_FAIL_INVAILD_VERSION;
                ais_ota_server_state_set_fail(OTA_FAIL_INVAILD_VERSION, p_ver_resp->ver);
                return -1;
            }

            g_ais_ota_server_ctx.dev_info.dev->old_version = p_ver_resp->ver;

#ifndef CONFIG_ALLOW_OTA_FOR_HIGH_VERSION
            if (p_ver_resp->ver > g_ais_ota_server_ctx.fir_info.firmware->version) {
                LOGE(TAG, "Err version, the device version is %04x but the update firmware is %04x", p_ver_resp->ver,
                     g_ais_ota_server_ctx.fir_info.firmware->version);
                g_ais_ota_server_ctx.dev_info.dev->failed_reason = OTA_FAIL_INVAILD_VERSION;
                ais_ota_server_state_set_fail(OTA_FAIL_INVAILD_VERSION, p_ver_resp->ver);
                return -1;
            }
#endif
            if (p_ver_resp->ver == g_ais_ota_server_ctx.fir_info.firmware->version) {
                LOGD(TAG, "The device version is equal to the update firmware version");
                ais_ota_server_state_set_success();
                return 0;
            }

            // send update req
            return ais_ota_server_send_update_req(g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);
        }
    }
    return ret;
}

static int ais_ota_server_send_check_req(uint8_t trans)
{
    ais_ota_check_req_t req;
    ais_pdu_t           pdu;
    int                 ret = 0;
    // TODO
    req.state = 0x01;
    ret       = _ais_set_pdu(&pdu, 0, 0, AIS_OTA_CHECK_REQ, (uint8_t *)&req, sizeof(ais_ota_check_req_t));

    if (ret) {
        return -1;
    }

    return ais_ota_server_send(&pdu, trans, 0);
}

static int ais_ota_server_handle_check_resp(ais_pdu_t *pdu)
{
    ais_ota_check_resp_t *resp = (ais_ota_check_resp_t *)(pdu->payload);
    int                   ret  = 0;

    // TODO
    if (!resp->state) {
        LOGE(TAG, "ota image crc check err ota failed");
        g_ais_ota_server_ctx.dev_info.dev->failed_reason = OTA_FAIL_CRC_ERR;
        ais_ota_server_state_set_fail(OTA_FAIL_CRC_ERR, g_ais_ota_server_ctx.dev_info.dev->old_version);
    } else {
        LOGD(TAG, "ota check success");
        g_ais_ota_server_ctx.dev_info.dev->ota_state            = OTA_STATE_LOAD_COMPLETE;
        g_ais_ota_server_ctx.dev_info.dev->download_finish_time = aos_now_ms();
        ret = ota_server_trans_disconnect(g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);
        if (ret) {
            LOGW(TAG, "ota disconnect fail");
        }
        g_ais_ota_server_ctx.dev_info.dev->conect_failed_count = 0;
        if (g_ais_ota_server_ctx.fir_info.firmware->ota_chanel == OTA_CHANNEL_ON_HCI_UART
            && g_ais_ota_server_ctx.dev_info.dev->ota_state == OTA_STATE_LOAD_COMPLETE)
        { // no version check for uart chanel
            ota_server_trans_reset(OTA_CHANNEL_ON_HCI_UART);
            ais_ota_server_state_set_success();
        }
    }

    return 0;
}

static int ais_ota_server_handle_resp_err(ais_pdu_t *pdu)
{
    LOGE(TAG, "status err");
    ais_ota_server_state_set_fail(OTA_FAIL_DEV_FAILED, g_ais_ota_server_ctx.dev_info.dev->old_version);
    return 0;
}

static int ais_ota_server_prepare_data()
{
    int      ret     = 0;
    uint8_t *tx_addr = NULL;

    g_ais_ota_server_ctx.dev_info.total_seq = GET_FRAME_SIZE(
        g_ais_ota_server_ctx.fir_info.firmware->size, g_ais_ota_server_ctx.dev_info.read_offset,
        g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size, g_ais_ota_server_ctx.dev_info.total_frame + 1);

    tx_addr = g_ais_ota_server_ctx.fir_info.tx_buffer;

    for (int i = 0; i < g_ais_ota_server_ctx.dev_info.total_seq; i++) {

        ais_header_t *header = (ais_header_t *)tx_addr;

        uint32_t read_size
            = (g_ais_ota_server_ctx.fir_info.firmware->size - g_ais_ota_server_ctx.dev_info.read_offset)
                      < g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size
                  ? (g_ais_ota_server_ctx.fir_info.firmware->size - g_ais_ota_server_ctx.dev_info.read_offset)
                  : g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size;

        if (read_size == 0) {
            LOGE(TAG, "read ota firwarme finished");
            return 0;
        }

        // TODO
        header->msg_id      = 0;
        header->enc         = 0;
        header->ver         = 0;
        header->total_frame = g_ais_ota_server_ctx.dev_info.total_seq - 1;
        header->seq         = i;
        header->cmd         = AIS_OTA_DATA;
        header->payload_len = read_size;

        tx_addr += sizeof(ais_header_t);

        ret = ota_server_upgrade_firmware_read(g_ais_ota_server_ctx.fir_info.index,
                                               g_ais_ota_server_ctx.dev_info.read_offset, read_size, tx_addr);

        if (ret) {
            LOGE(TAG, "read ota firwarme failed %d", ret);
            return -1;
        }

        tx_addr += read_size;
        g_ais_ota_server_ctx.dev_info.read_offset += read_size;
        g_ais_ota_server_ctx.dev_info.the_last_pdu_payload_size = read_size;
    }
    LOGD(TAG, "frame now %d %d", g_ais_ota_server_ctx.dev_info.total_seq, g_ais_ota_server_ctx.dev_info.read_offset);
    return ret;
}

static int ais_ota_server_handle_upd(ais_pdu_t *pdu)
{
    ais_ota_upd_resp_t *p_upd_resp = (ais_ota_upd_resp_t *)(pdu->payload);

    if (!p_upd_resp->state) {
        LOGE(TAG, "The device refuse the ota req");
        g_ais_ota_server_ctx.dev_info.dev->failed_reason = OTA_FAIL_DEV_REFUSED;
        ais_ota_server_state_set_fail(OTA_FAIL_DEV_REFUSED, g_ais_ota_server_ctx.dev_info.dev->old_version);
        return -1;
    } else {
        g_ais_ota_server_ctx.dev_info.rx_size           = p_upd_resp->rx_size;
        g_ais_ota_server_ctx.dev_info.need_retrans_flag = 0;
        g_ais_ota_server_ctx.dev_info.total_frame       = p_upd_resp->total_frame;
        g_ais_ota_server_ctx.ota_state                  = AIS_OTA_SERVER_STATE_ONGOING;
        // malloc the buffer
        g_ais_ota_server_ctx.fir_info.need_remalloc_flag = 1;

        if (g_ais_ota_server_ctx.fir_info.tx_buffer
            && g_ais_ota_server_ctx.fir_info.mtu_last == g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size
            && g_ais_ota_server_ctx.dev_info.total_frame == g_ais_ota_server_ctx.fir_info.total_frame_last)
        {
            g_ais_ota_server_ctx.fir_info.need_remalloc_flag = 0;
            g_ais_ota_server_ctx.dev_info.read_offset        = 0;
        }

        if (g_ais_ota_server_ctx.fir_info.need_remalloc_flag) {
            if (g_ais_ota_server_ctx.fir_info.tx_buffer) {
                aos_free(g_ais_ota_server_ctx.fir_info.tx_buffer);
                g_ais_ota_server_ctx.fir_info.tx_buffer = NULL;
            }

            g_ais_ota_server_ctx.fir_info.tx_buffer
                = (uint8_t *)aos_malloc((g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size + sizeof(ais_header_t))
                                        * (g_ais_ota_server_ctx.dev_info.total_frame + 1));

            if (!g_ais_ota_server_ctx.fir_info.tx_buffer) {
                LOGE(TAG, "malloc tx buffer failed");
                return -1;
            }

            g_ais_ota_server_ctx.dev_info.read_offset      = 0;
            g_ais_ota_server_ctx.fir_info.mtu_last         = g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size;
            g_ais_ota_server_ctx.fir_info.total_frame_last = g_ais_ota_server_ctx.dev_info.total_frame;
        }

        // notify the ot process thread to satrt ota data send
        ota_process_cmd_message cmd;
        cmd.event = OTA_EVENT_SEND_CMD;
        ota_process_cmd_set(&cmd);
    }
    return 0;
}

static void ais_send_flag_set(uint8_t flag)
{
    aos_mutex_lock(&g_ais_ota_server_ctx.tx_in_flag_mutex, AOS_WAIT_FOREVER);
    g_ais_ota_server_ctx.tx_in_flag = flag;
    aos_mutex_unlock(&g_ais_ota_server_ctx.tx_in_flag_mutex);
}

static int ais_send_flag_get()
{
    uint8_t flag = 0;
    aos_mutex_lock(&g_ais_ota_server_ctx.tx_in_flag_mutex, AOS_WAIT_FOREVER);
    flag = g_ais_ota_server_ctx.tx_in_flag;
    aos_mutex_unlock(&g_ais_ota_server_ctx.tx_in_flag_mutex);
    return flag;
}

static int ais_ota_server_handle_status(ais_pdu_t *pdu)
{
    int                            ret                  = 0;
    uint8_t                        the_same_flag        = 0;
    static ais_ota_status_report_t p_status_report_last = { 0 };
    uint32_t                       cost_timer           = aos_now_ms() - g_ais_ota_server_ctx.start_time;
    ais_ota_status_report_t *      p_status_report      = (ais_ota_status_report_t *)(pdu->payload);

    if (g_ais_ota_server_ctx.dev_info.read_offset == 0) {
        LOGW(TAG, "Ignore the status message,when read offset equal to 0");
        return 0;
    }
    LOGD(TAG, "Last:%d:%d rx:%d/%d %d %dms", p_status_report->last_seq, p_status_report->total_frame,
         p_status_report->rx_size, g_ais_ota_server_ctx.fir_info.firmware->size,
         g_ais_ota_server_ctx.dev_info.read_offset, cost_timer);
    (void)cost_timer;

    if (memcmp(p_status_report, &p_status_report_last, sizeof(ais_ota_status_report_t))) {
        memcpy(&p_status_report_last, p_status_report, sizeof(ais_ota_status_report_t));
    } else {
        if (ais_send_flag_get() || g_ais_ota_server_ctx.dev_info.read_offset == p_status_report->rx_size) {
            LOGD(TAG, "ignore the same status");
            return 0;
        }
        if (g_ais_ota_server_ctx.dev_info.read_offset > p_status_report->rx_size) {
            the_same_flag = 1;
        }
    }

    if (p_status_report->last_seq == p_status_report->total_frame
        && p_status_report->rx_size == g_ais_ota_server_ctx.fir_info.firmware->size)
    {
        LOGD(TAG, "the whole image send finish");
        // g_ais_ota_server_ctx.send_finish_flag = 1;
        ret = ais_ota_server_send_check_req(g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);
        if (ret) {
            LOGE(TAG, "send ota check req failed");
            return -1;
        }

        return 0;
    }

    if (p_status_report->last_seq != p_status_report->total_frame || the_same_flag) {
        if (p_status_report->last_seq == p_status_report->total_frame) {
            LOGE(TAG, "start restrans from seq 0");
        } else {
            LOGE(TAG, "start restrans from seq %d", p_status_report->last_seq + 1);
        }
        g_ais_ota_server_ctx.dev_info.need_retrans_flag = 1;
        g_ais_ota_server_ctx.dev_info.last_seq          = p_status_report->last_seq;
    } else if ((p_status_report->last_seq == p_status_report->total_frame) && p_status_report->total_frame == 0) {
        LOGD(TAG, "The first frame not recived");
        g_ais_ota_server_ctx.dev_info.need_retrans_flag = 1;
        g_ais_ota_server_ctx.dev_info.last_seq          = 0;
    } else {
        g_ais_ota_server_ctx.dev_info.need_retrans_flag = 0;
        g_ais_ota_server_ctx.dev_info.last_seq          = 0;
    }

    ota_process_cmd_message cmd;
    cmd.event = OTA_EVENT_SEND_CMD;
    ota_process_cmd_set(&cmd);

    return 0;
}

static void ais_ota_server_cb(uint8_t event, const void *event_data)
{
    int ret = 0;
    LOGD(TAG, "ais ota server event %02x", event);
    switch (event) {
        case AIS_OTA_SERVER_STATE_CHAN_CREATE_CONN_FAIL: {
            if (!event_data) {
                LOGE(TAG, "No OTA event data for conn fail state");
                return;
            }
            upgrade_device *device   = NULL;
            device_info     dev_info = { 0 };
            memcpy(&dev_info.addr, (mac_t *)event_data, sizeof(mac_t));
            device = dev_list_dev_search(g_ais_ota_server_ctx.fir_info.dev_list_head, dev_info, DEV_LIST_FOUND_BY_MAC);
            if (!device) {
                LOGD(TAG, "The device cannot be found in the ota device list,it may be rm already");
                return;
            }
            device->conect_failed_count++;
            if (device->conect_failed_count >= DEF_MAX_CONN_RETRY) {
                ais_ota_server_state_set_conn_fail(device);
            }

        } break;

        case AIS_OTA_SERVER_STATE_CHAN_CONN: {
            connect_status conn_status = *(connect_status *)event_data;
            device_info    info;

            upgrade_device *ota_device = NULL;

            if (!g_ais_ota_server_ctx.fir_info.dev_list_head) {
                LOGD(TAG, "The firmware dev list is null");
                if (!conn_status.status) {
                    ais_ota_server_channel_disconnect();
                }
                return;
            }

            memcpy(&info.addr, &conn_status.addr, sizeof(mac_t)); // TODO more device info

            ota_device = dev_list_dev_search(g_ais_ota_server_ctx.fir_info.dev_list_head, info, DEV_LIST_FOUND_BY_MAC);
            if (!ota_device) {
                LOGD(TAG, "The device cannot be found in the ota device list,it may be rm already");
                if (!conn_status.status) {
                    ais_ota_server_channel_disconnect();
                }
            } else {
                LOGD(TAG, "The device found in the ota device list, state %02x", ota_device->ota_state);
                if (ota_device->ota_state == OTA_STATE_FAILED) {
                    if (!conn_status.status) {
                        ais_ota_server_channel_disconnect();
                    } else {
                        dev_list_rm_and_free_dev(g_ais_ota_server_ctx.fir_info.dev_list_head, info,
                                                 DEV_LIST_FOUND_BY_MAC);
                    }
                }
            }

        } break;

        case AIS_OTA_SERVER_STATE_CHAN_DISCONN: {
            mac_t *         peer_addr = (mac_t *)event_data;
            device_info     info;
            upgrade_device *ota_device = NULL;

            if (!g_ais_ota_server_ctx.fir_info.dev_list_head) {
                LOGD(TAG, "The firmware dev list is null");
                return;
            }

            memcpy(&info.addr, peer_addr, sizeof(mac_t)); // TODO more device info

            ota_device = dev_list_dev_search(g_ais_ota_server_ctx.fir_info.dev_list_head, info, DEV_LIST_FOUND_BY_MAC);
            if (!ota_device) {
                LOGD(TAG, "The device cannot be found in the ota device list,it may be rm already");
            } else {
                LOGD(TAG, "The device found in the ota device list, state %02x", ota_device->ota_state);
                if (ota_device->ota_state == OTA_STATE_FAILED) {
                    dev_list_rm_and_free_dev(g_ais_ota_server_ctx.fir_info.dev_list_head, info, DEV_LIST_FOUND_BY_MAC);
                } else if (ota_device->ota_state == OTA_STATE_ONGOING) {
                    ota_device->failed_reason = OTA_FAIL_DISCONN;
                    ais_ota_server_state_set_fail(OTA_FAIL_DISCONN, ota_device->old_version);
                }
            }
        } break;

        case AIS_OTA_SERVER_STATE_CHAN_READY: {
            uint8_t mtu_size = *(uint8_t *)event_data;

            if (mtu_size <= sizeof(ais_header_t)) {
                LOGE(TAG, "mtu size %d,it should be large than %d", mtu_size, sizeof(ais_header_t));
                return;
            } else {
                g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size = *(uint8_t *)event_data - sizeof(ais_header_t);
                LOGD(TAG, "ota trans mtu size %d", g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size);
            }
        } break;

        case AIS_OTA_SERVER_STATE_ONGOING: {
            device_info dev_addr = { 0 };
            memcpy(&dev_addr.addr, (mac_t *)event_data, sizeof(mac_t));
            upgrade_device *ota_device = NULL;
            ota_device
                = dev_list_dev_search(g_ais_ota_server_ctx.fir_info.dev_list_head, dev_addr, DEV_LIST_FOUND_BY_MAC);
            if (!ota_device) {
                LOGE(TAG, "The device cannot be found in the ota device list,it may be rm already");
                ais_ota_server_channel_disconnect();
                return;
            }

            if (g_ais_ota_server_ctx.dev_info.dev
                && !memcmp(&ota_device->device, &g_ais_ota_server_ctx.dev_info.dev->device, sizeof(device_info)))
            {
                if (g_ais_ota_server_ctx.dev_info.dev->ota_state != OTA_STATE_LOAD_COMPLETE) {
                    LOGD(TAG, "connected before,the device state now %d", g_ais_ota_server_ctx.dev_info.dev->ota_state);
                    return;
                }
            } else {
                g_ais_ota_server_ctx.dev_info.dev = ota_device;
                if (ota_device->ota_state == OTA_STATE_IDLE) {
                    g_ais_ota_server_ctx.ota_state = AIS_OTA_SERVER_STATE_ONGOING;
                    ais_ota_server_state_set_ongoing();
                }
            }

            ret = ais_ota_server_get_version(g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);
            if (ret) {
                LOGE(TAG, "ota server send get version failed %d by channel %d", ret,
                     g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);
                ais_ota_server_state_set_fail(OTA_FAIL_GET_VERSION, g_ais_ota_server_ctx.dev_info.dev->old_version);
                return;
            }
        } break;

        case AIS_OTA_VER_RESP: {
            ais_ota_server_handle_version((ais_pdu_t *)event_data);
        } break;

        case AIS_OTA_UPD_RESP: {
            ais_ota_server_handle_upd((ais_pdu_t *)event_data);
        } break;

        case AIS_OTA_STATUS: {
            ais_ota_server_handle_status((ais_pdu_t *)event_data);
        } break;

        case AIS_OTA_CHECK_RESP: {
            ais_ota_server_handle_check_resp((ais_pdu_t *)event_data);
        } break;
        case AIS_RESP_ERR: {
            ais_ota_server_handle_resp_err((ais_pdu_t *)event_data);
        } break;
        default:
            break;
    }

    return;
}

static int ais_ota_server_send_data(uint8_t seq, uint8_t trans, uint8_t ack)
{
    uint8_t  tx_len           = sizeof(ais_header_t);
    uint32_t tx_buffer_offset = seq * (sizeof(ais_header_t) + g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size);

    if (seq == g_ais_ota_server_ctx.dev_info.total_seq - 1) {
        tx_len += g_ais_ota_server_ctx.dev_info.the_last_pdu_payload_size;
    } else {
        tx_len += g_ais_ota_server_ctx.dev_info.tx_pdu_payload_size;
    }

    return ota_server_trans_send(trans, g_ais_ota_server_ctx.fir_info.tx_buffer + tx_buffer_offset, tx_len, ack);
}

// For user

int ais_ota_server_init(uint8_t channel)
{
    if (g_ais_ota_server_ctx.init_flag) {
        return -EALREADY;
    }

    int ret = 0;
    ret     = ota_server_trans_init(channel, ais_ota_server_cb);
    if (ret) {
        return ret;
    }
    aos_mutex_new(&g_ais_ota_server_ctx.tx_in_flag_mutex);
    g_ais_ota_server_ctx.init_flag = 1;

    return 0;
}

int ais_ota_server_prepare(uint16_t index, upgrade_firmware *firmware_info)
{
    int ret = 0;

    if (!firmware_info) {
        return -EINVAL;
    }

    if (g_ais_ota_server_ctx.fir_info.firmware != &firmware_info->info) {
        g_ais_ota_server_ctx.fir_info.firmware      = &firmware_info->info;
        g_ais_ota_server_ctx.fir_info.dev_list_head = &firmware_info->dev_list_head;
        g_ais_ota_server_ctx.fir_info.index         = index;
    }

    ret = ota_server_trans_prepare(firmware_info->info.ota_chanel, &firmware_info->dev_list_head);
    if (ret) {
        goto fail;
    }

    // TODO Timeout
    return 0;

fail:
    g_ais_ota_server_ctx.fir_info.firmware = NULL;
    ais_ota_server_reset();

    return ret;
}

int ais_ota_server_send_start()
{
    int     ret            = 0;
    uint8_t start_send_seq = 0;

    if (!g_ais_ota_server_ctx.dev_info.dev) {
        return -EINVAL;
    }

    if (g_ais_ota_server_ctx.ota_state != AIS_OTA_SERVER_STATE_ONGOING) {
        LOGE(TAG, "ota status not ready");
        return -1;
    }

    if (!g_ais_ota_server_ctx.dev_info.read_offset) {
        g_ais_ota_server_ctx.start_time = aos_now_ms();
    }

    ais_send_flag_set(1);

    if (!g_ais_ota_server_ctx.dev_info.need_retrans_flag) {
        ret = ais_ota_server_prepare_data();
        if (ret < 0) {
            LOGE(TAG, "ota server prepare data failed");
            ais_ota_server_state_set_fail(OTA_FAIL_READ_FIRMWARE, g_ais_ota_server_ctx.dev_info.dev->old_version);
            ais_send_flag_set(0);
            return -1;
        }
    } else {
        if (g_ais_ota_server_ctx.dev_info.last_seq == g_ais_ota_server_ctx.dev_info.total_frame)
        { // lost frame 0,so resend it
            start_send_seq = 0;
        } else {
            if (g_ais_ota_server_ctx.dev_info.total_seq == 0) {
                LOGE(TAG, "total seq equal null");
                return -1;
            }
            start_send_seq = (g_ais_ota_server_ctx.dev_info.last_seq + 1) % g_ais_ota_server_ctx.dev_info.total_seq;
        }
        LOGD(TAG, "retrans seq:%d total:%d", start_send_seq, g_ais_ota_server_ctx.dev_info.total_seq);
    }

    for (int i = start_send_seq; i < g_ais_ota_server_ctx.dev_info.total_seq; i++) {
        ret = ais_ota_server_send_data(i, g_ais_ota_server_ctx.fir_info.firmware->ota_chanel, 0);

        if (ret) {
            LOGE(TAG, "ais ota data send failed %d", ret);
            ais_ota_server_state_set_fail(OTA_FAIL_SEND_FIRMWARE, g_ais_ota_server_ctx.dev_info.dev->old_version);
            ais_send_flag_set(0);
            return -1;
        }
    }
    ais_send_flag_set(0);

    return 0;
}

int ais_ota_server_ota_stop()
{
    int ret = 0;

    if (g_ais_ota_server_ctx.dev_info.dev) {
        g_ais_ota_server_ctx.ota_state = AIS_OTA_SERVER_STATE_IDLE;
        ret = ota_server_trans_disconnect(g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);

        if (ret) {
            LOGE(TAG, "ota stop and disconnect fail");
        }

        g_ais_ota_server_ctx.dev_info.dev->failed_reason = OTA_FAIL_CANCEL;
        ais_ota_server_state_set_fail(OTA_FAIL_CANCEL, g_ais_ota_server_ctx.dev_info.dev->old_version);
    }
    return 0;
}

int ais_ota_server_channel_disconnect()
{
    return ota_server_trans_disconnect(g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);
}

int ais_ota_server_reset()
{

    LOGD(TAG, "ais ota server reset channel %02x", g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);
    if (!g_ais_ota_server_ctx.init_flag) {
        return 0;
    }
    ota_server_trans_unregister(g_ais_ota_server_ctx.fir_info.firmware->ota_chanel);

    if (g_ais_ota_server_ctx.fir_info.tx_buffer) {
        aos_free(g_ais_ota_server_ctx.fir_info.tx_buffer);
        g_ais_ota_server_ctx.fir_info.tx_buffer = NULL;
    }

    aos_mutex_free(&g_ais_ota_server_ctx.tx_in_flag_mutex);

    memset(&g_ais_ota_server_ctx, 0x00, sizeof(ais_ota_server_ctx));

    return 0;
}
