/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <pin_name.h>
#include <pinmux.h>

#include <devices/uart.h>
#include <devices/hci.h>
#include <yoc/uservice.h>
#include <aos/kernel.h>

#define H4_NONE 0x00
#define H4_CMD  0x01
#define H4_ACL  0x02
#define H4_SCO  0x03
#define H4_EVT  0x04

#define TAG "H4"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifdef CONFIG_CHIP_CH2201
#define H4_UART              "uart2"
#define H4_UART_TX           PA24
#define H4_UART_RX           PA25
#define H4_UART_RTS          PA27
#define H4_UART_CTS          PA26
#define H4_UART_TX_FUNC      PA24_UART2_TX
#define H4_UART_RX_FUNC      PA25_UART2_RX
#define H4_UART_RTS_FUNC     PA27_UART2_RTS
#define H4_UART_CTS_FUNC     PA26_UART2_CTS
#else
#error "rafael hci is unsuported in this board"
#endif

extern int hci_recv(void *data, int32_t len);

struct bt_hci_evt_hdr {
    u8_t  evt;
    u8_t  len;
} __attribute__((__packed__));

struct bt_hci_acl_hdr {
    u16_t handle;
    u16_t len;
} __attribute__((__packed__));

typedef struct rx_t {
    uint8_t buf[256];
    uint8_t buf_len;

    uint16_t remaining;
    uint16_t discard;

    bool     have_hdr;
    bool     discardable;

    uint8_t  hdr_len;

    uint8_t  type;
    union {
        struct bt_hci_evt_hdr evt;
        struct bt_hci_acl_hdr acl;
        uint8_t hdr[4];
    };

    uint8_t ongoing;
} rx_t;

typedef struct  {
    aos_dev_t  device;
    aos_dev_t *uart_dev;
    rx_t rx;
    uservice_t *srv;
} h4_device_t;

static h4_device_t *h4_dev = NULL;

typedef enum {
    H4SERVER_SEND_CMD,
    H4SERVER_INTERRUPT_CMD,
    H4SERVER_END_CMD
} H4SERVER_CMD;

static int h4server_process_rpc(void *context, rpc_t *rpc);

static void reset_rx(void);

static uint32_t read_byte(uint8_t *data, uint32_t len)
{
    int32_t read_len;

    read_len = uart_recv(h4_dev->uart_dev, data, len, 0);

    if (read_len == 0) {
        h4_dev->rx.ongoing = 0;
    }

    return read_len;
}

static uint32_t write_byte(uint8_t *data, uint32_t len)
{
    int ret;

    ret = uart_send(h4_dev->uart_dev, data, len);

    if (ret) {
        return ret;
    }

    return 0;
}

static inline void h4_get_type(void)
{
    /* Get packet type */
    if (read_byte(&h4_dev->rx.type, 1) != 1) {
        h4_dev->rx.type = H4_NONE;
        return;
    }

    switch (h4_dev->rx.type) {
        case H4_EVT:
            h4_dev->rx.remaining = sizeof(h4_dev->rx.evt);
            h4_dev->rx.hdr_len = h4_dev->rx.remaining;
            break;

        case H4_ACL:
            h4_dev->rx.remaining = sizeof(h4_dev->rx.acl);
            h4_dev->rx.hdr_len = h4_dev->rx.remaining;
            break;

        default:
            LOGE(TAG, "Unknown H:4 type 0x%02x", h4_dev->rx.type);
            h4_dev->rx.type = H4_NONE;
    }
}

static inline void get_acl_hdr(void)
{
    struct bt_hci_acl_hdr *hdr = &h4_dev->rx.acl;
    int to_read = sizeof(*hdr) - h4_dev->rx.remaining;

    h4_dev->rx.remaining -= read_byte((uint8_t *)hdr + to_read,
                                      h4_dev->rx.remaining);

    if (!h4_dev->rx.remaining) {
        h4_dev->rx.remaining = hdr->len;//((hdr->len & 0xFF) << 8)|((hdr->len >> 8) & 0xFF);

        if (h4_dev->rx.remaining > sizeof(h4_dev->rx.buf) - h4_dev->rx.buf_len) {
            LOGE(TAG, "invaild ACL len %u bytes", h4_dev->rx.remaining);
            reset_rx();
            return;
        }

        //LOGD(TAG, "Got ACL header. Payload %u bytes", h4_dev->rx.remaining);
        h4_dev->rx.have_hdr = true;
    }
}

static inline void get_evt_hdr(void)
{
    struct bt_hci_evt_hdr *hdr = &h4_dev->rx.evt;
    int to_read = h4_dev->rx.hdr_len - h4_dev->rx.remaining;

    h4_dev->rx.remaining -= read_byte((uint8_t *)hdr + to_read,
                                      h4_dev->rx.remaining);

    if (h4_dev->rx.hdr_len == sizeof(*hdr) && h4_dev->rx.remaining < sizeof(*hdr)) {
        switch (h4_dev->rx.evt.evt) {
            case 0x3e: //BT_HCI_EVT_LE_META_EVENT
                h4_dev->rx.remaining++;
                h4_dev->rx.hdr_len++;
                break;
        }
    }

    if (!h4_dev->rx.remaining) {
        h4_dev->rx.remaining = hdr->len - (h4_dev->rx.hdr_len - sizeof(*hdr));
        //LOGD(TAG, "Got event header. Payload %u bytes", hdr->len);
        h4_dev->rx.have_hdr = true;
    }
}

static inline void copy_hdr(uint8_t *buf)
{
    buf[0] = h4_dev->rx.type;
    memcpy(buf + 1, h4_dev->rx.hdr, h4_dev->rx.hdr_len);
    h4_dev->rx.buf_len = h4_dev->rx.hdr_len + 1;
}

static void reset_rx(void)
{
    memset(h4_dev->rx.buf, 0, sizeof(h4_dev->rx.buf));
    h4_dev->rx.buf_len = 0;
    h4_dev->rx.type = H4_NONE;
    h4_dev->rx.remaining = 0;
    h4_dev->rx.have_hdr = false;
    h4_dev->rx.hdr_len = 0;
    h4_dev->rx.discardable = false;
}

static size_t h4_discard(size_t len)
{
    uint8_t buf[33];

    return read_byte(buf, min(len, sizeof(buf)));
}

static inline void read_payload(void)
{
    int read;

    read = read_byte(h4_dev->rx.buf + h4_dev->rx.buf_len, h4_dev->rx.remaining);
    h4_dev->rx.buf_len += read;
    h4_dev->rx.remaining -= read;

    // LOGD(TAG, "got %d bytes, remaining %u", read, h4_dev->rx.remaining);
    // LOGD(TAG, "Payload (len %u)", h4_dev->rx.buf_len);

    if (h4_dev->rx.remaining) {
        return;
    }

    hci_recv(h4_dev->rx.buf, h4_dev->rx.buf_len);
    reset_rx();
}

static inline void read_header(void)
{
    switch (h4_dev->rx.type) {
        case H4_NONE:
            h4_get_type();
            return;

        case H4_EVT:
            get_evt_hdr();
            break;

        case H4_ACL:
            get_acl_hdr();
            break;

        default:
            reset_rx();
            return;
    }

    if (h4_dev->rx.have_hdr && h4_dev->rx.buf) {
        if (h4_dev->rx.remaining > (sizeof(h4_dev->rx.buf) - h4_dev->rx.buf_len)) {
            LOGE(TAG, "Not enough space in buffer");
            h4_dev->rx.discard = h4_dev->rx.remaining;
            reset_rx();
        } else {
            copy_hdr(h4_dev->rx.buf);
        }
    }
}

static inline void process_rx(void)
{
    //LOGD(TAG, "remaining %u discard %u have_hdr %u buf len %u",
    //     h4_dev->rx.remaining, h4_dev->rx.discard, h4_dev->rx.have_hdr, h4_dev->rx.buf_len);

    if (h4_dev->rx.discard) {
        h4_dev->rx.discard -= h4_discard(h4_dev->rx.discard);
        return;
    }

    if (h4_dev->rx.have_hdr) {
        read_payload();
    } else {
        read_header();
    }
}

static void uart_event(aos_dev_t *dev, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ) {
        if (h4_dev->rx.ongoing) {
            return;
        }

        //uservice_call_async(h4_dev->srv, H4SERVER_INTERRUPT_CMD, NULL, 0);
        do {
            h4_dev->rx.ongoing = 1;
            process_rx();
        } while (h4_dev->rx.ongoing);
    }
}

static int interrupt_cmd_handle(void *context, rpc_t *rpc)
{
    do {
        h4_dev->rx.ongoing = 1;
        process_rx();
    } while (h4_dev->rx.ongoing);

    return 0;
}

static const rpc_process_t c_h4server_cmd_cb_table[] = {
    {H4SERVER_INTERRUPT_CMD,    interrupt_cmd_handle},
    {H4SERVER_END_CMD,           NULL},
};

static int h4server_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, c_h4server_cmd_cb_table);
}

static aos_dev_t *h4_hci_init(driver_t *drv, void *config, int id)
{
    drv_pinmux_config(H4_UART_TX, H4_UART_TX_FUNC);
    drv_pinmux_config(H4_UART_RX, H4_UART_RX_FUNC);
    drv_pinmux_config(H4_UART_RTS, H4_UART_RTS_FUNC);
    drv_pinmux_config(H4_UART_CTS, H4_UART_CTS_FUNC);

    utask_t *task;

    task = utask_new("hci_rx_task", 1024, 20, 7);

    h4_dev = (h4_device_t *)device_new(drv, sizeof(h4_device_t), id);

    if (h4_dev) {
        h4_dev->srv = uservice_new("h4server", h4server_process_rpc, NULL);
        utask_add(task, h4_dev->srv);
    }

    return (aos_dev_t *)h4_dev;
}

#define h4_hci_uninit device_free

static int h4_hci_send(aos_dev_t *dev, void *data, uint32_t size)
{
    if (dev == NULL || data == NULL || size == 0) {
        return -1;
    }

    write_byte(data, size);
    return 0;
}

static int h4_hci_open(aos_dev_t *dev)
{
    int ret;
    uart_config_t config;

    h4_dev->uart_dev = device_open(H4_UART);

    if (h4_dev->uart_dev == NULL) {
        LOGE(TAG, "h4 dev open fail");
        return -1;
    }

    uart_config_default(&config);
    config.flow_control = FLOW_CONTROL_CTS_RTS;

    ret = uart_config(h4_dev->uart_dev, &config);

    if (ret) {
        return ret;
    }

    ret = uart_set_buffer_size(h4_dev->uart_dev, 256);

    if (ret) {
        return ret;
    }

    uart_set_event(h4_dev->uart_dev, uart_event, NULL);

    h4_discard(32);
    return 0;
}

static int h4_hci_close(aos_dev_t *dev)
{
    return 0;
}

static hci_driver_t h4_driver = {
    .drv = {
        .name   = "hci_h4",
        .type   = "hci",
        .init   = h4_hci_init,
        .uninit = h4_hci_uninit,
        .lpm    = NULL,
        .open   = h4_hci_open,
        .close  = h4_hci_close,
    },
    .send       = h4_hci_send,
};

int hci_driver_h4_register(int idx)
{
    driver_register(&h4_driver.drv, NULL, idx);
    return 0;
}

