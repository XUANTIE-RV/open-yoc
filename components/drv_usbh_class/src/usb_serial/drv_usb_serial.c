/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/ringbuffer.h>
#include "devices/impl/uart_impl.h"

#include <usbh_core.h>
#include <usbh_serial.h>

#define TAG "uart_drv_usb_serial"

#define EVENT_WRITE  0x0F0F0000
#define EVENT_READ   0x00000F0F
#define UART_RB_SIZE 1024

typedef struct {
    rvm_dev_t   device;
    aos_event_t event_write_read;
    void (*write_event)(rvm_dev_t *dev, int event_id, void *priv);
    void *                         priv;
    int                            type;
    char *                         recv_buf;
    uint8_t *                      cdc_buffer;
    dev_ringbuf_t                  read_buffer;
    struct usbh_urb                bulkin_urb;
    struct usbh_urb                bulkout_urb;
    struct usbh_cdc_custom_serial *usb_class;
} uart_dev_t;

#define uart(dev) ((uart_dev_t *)dev)

static rvm_dev_t *usb_serial_uart_init(driver_t *drv, void *config, int id)
{
    uart_dev_t *uart = (uart_dev_t *)rvm_hal_device_new(drv, sizeof(uart_dev_t), id);

    return (rvm_dev_t *)uart;
}

#define usb_serial_uart_uninit rvm_hal_device_free

static void _data_input(void *argv, int size)
{
    int         ret;
    uart_dev_t *dev = argv;

    ret = ringbuffer_write(&dev->read_buffer, dev->cdc_buffer, size);

    if (ret != size) {
        LOGE(TAG, "ringbuffer full");
    }

    aos_event_set(&dev->event_write_read, EVENT_READ, AOS_EVENT_OR);

    if (dev->write_event) {
        dev->write_event((rvm_dev_t *)dev, USART_EVENT_READ, dev->priv);
    }

    usbh_bulk_urb_fill(&uart(dev)->bulkin_urb, uart(dev)->usb_class->bulkin, uart(dev)->cdc_buffer, 128, 0, _data_input,
                       uart(dev));
    ret = usbh_submit_urb(&uart(dev)->bulkin_urb);
    if (ret < 0) {
        LOGE(TAG, "bulk out error,ret:%d\r\n", ret);
        return;
    }
}

static int usb_serial_uart_open(rvm_dev_t *dev)
{
    int ret;

    if (aos_event_new(&uart(dev)->event_write_read, 0) != 0) {
        return -1;
    }

    int retry = 10;
    do {
        uart(dev)->usb_class = (struct usbh_cdc_custom_serial *)usbh_find_class_instance("/dev/serial");
        sleep(1);
    } while (uart(dev)->usb_class == NULL && retry--);

    if (uart(dev)->usb_class == NULL) {
        LOGE(TAG, "NO USB Device");
        goto error1;
    }

    uart(dev)->recv_buf = (char *)aos_malloc(UART_RB_SIZE);

    if (uart(dev)->recv_buf == NULL) {
        goto error1;
    }

    uart(dev)->cdc_buffer = aos_malloc(128);

    if (uart(dev)->cdc_buffer == NULL) {
        goto error2;
    }

    ringbuffer_create(&uart(dev)->read_buffer, uart(dev)->recv_buf, UART_RB_SIZE);

    usbh_bulk_urb_fill(&uart(dev)->bulkin_urb, uart(dev)->usb_class->bulkin, uart(dev)->cdc_buffer, 128, 0, _data_input,
                       uart(dev));
    ret = usbh_submit_urb(&uart(dev)->bulkin_urb);
    if (ret < 0) {
        LOGE(TAG, "bulk in error,ret:%d\r\n", ret);
        goto error3;
    }

    return 0;
error3:
    aos_free(uart(dev)->cdc_buffer);
error2:
    aos_free(uart(dev)->recv_buf);
error1:
    aos_event_free(&uart(dev)->event_write_read);

    return -1;
}

static int usb_serial_uart_close(rvm_dev_t *dev)
{
    aos_event_free(&uart(dev)->event_write_read);
    aos_free(uart(dev)->cdc_buffer);
    aos_free(uart(dev)->recv_buf);
    return 0;
}

static int usb_serial_uart_config(rvm_dev_t *dev, rvm_hal_uart_config_t *config)
{
    return 0;
}

static int usb_serial_uart_set_type(rvm_dev_t *dev, int type)
{
    uart(dev)->type = type;

    return 0;
}

static int usb_serial_uart_set_buffer_size(rvm_dev_t *dev, uint32_t size)
{
    return 0;
}

static int usb_serial_uart_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout_ms)
{
    int         ret;
    uart_dev_t *uart = uart(dev);

    usbh_bulk_urb_fill(&uart->bulkout_urb, uart->usb_class->bulkout, (uint8_t *)data, size, timeout_ms, NULL, NULL);
    ret = usbh_submit_urb(&uart->bulkout_urb);

    if (ret < 0) {
        LOGE(TAG, "bulk out error,ret:%d\r\n", ret);
    }

    return 0;
}

static int usb_serial_uart_recv(rvm_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms)
{
    unsigned int actl_flags;
    int          ret = 0;
    long long    time_enter, used_time;
    void *       temp_buf   = data;
    uint32_t     temp_count = size;
    time_enter              = aos_now_ms();

    while (1) {
        ret = ringbuffer_read(&uart(dev)->read_buffer, (uint8_t *)temp_buf, temp_count);

        temp_count = temp_count - ret;
        temp_buf   = (uint8_t *)temp_buf + ret;

        if (temp_count == 0 || timeout_ms == 0 || timeout_ms <= (used_time = aos_now_ms() - time_enter)) {
            break;
        }

        if (aos_event_get(&uart(dev)->event_write_read, EVENT_READ, AOS_EVENT_OR_CLEAR, &actl_flags,
                          timeout_ms - used_time)
            == -1)
        {
            break;
        }
    }

    return size - temp_count;
}

static void usb_serial_uart_event(rvm_dev_t *dev, void (*event)(rvm_dev_t *dev, int event_id, void *priv), void *priv)
{
    uart(dev)->priv        = priv;
    uart(dev)->write_event = event;
}

static uart_driver_t usb_serial_uart_driver = {
    .drv = {
        .name   = "usb_serial",
        .init   = usb_serial_uart_init,
        .uninit = usb_serial_uart_uninit,
        .open   = usb_serial_uart_open,
        .close  = usb_serial_uart_close,
    },
    .config          = usb_serial_uart_config,
    .set_type        = usb_serial_uart_set_type,
    .set_buffer_size = usb_serial_uart_set_buffer_size,
    .send            = usb_serial_uart_send,
    .recv            = usb_serial_uart_recv,
    .set_event       = usb_serial_uart_event,
};

void drv_ec200a_serial_register(uint8_t idx)
{
    rvm_driver_register(&usb_serial_uart_driver.drv, NULL, idx);
}
