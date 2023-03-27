/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#define LOG_TAG "HCI_UART_H5"


#include <devices/impl/hci_impl.h>
#include <hci_uart_h5/config.h>
#include <aos/aos.h>
#include <devices/uart.h>
#include <devices/hci.h>

#define BT_DBG_ENABLED 0

#include <common/log.h>

#define TAG "HCI"

#define HCI_VSC_H5_INIT                         0xFCEE

#if defined(CONFIG_HCI_UART_STATIC_STACK) && CONFIG_HCI_UART_STATIC_STACK
ktask_t          g_hci_task;
cpu_stack_t      g_hci_task_stack[CONFIG_HCI_UART_H5_TASK_STACK_SIZE/4] = {0};
#endif

static struct {
    int uart_idx;
    rvm_hal_uart_config_t uart_config;
    rvm_dev_t *uart_dev;
    aos_event_t uart_event;
    aos_task_t task_handle;
    hci_driver_send_cmd_t cmd_send;
    hci_event_cb_t event;
    void *event_priv;
    rvm_dev_t *hci_dev;
} hci_uart_h5 = { 0 };

void hci_uart_entry(void *arg)
{
    uint32_t act_flag;

    while (1) {
        aos_event_get(&hci_uart_h5.uart_event, 0x01, AOS_EVENT_OR_CLEAR, &act_flag, AOS_WAIT_FOREVER);
        if(hci_uart_h5.event) {
            hci_uart_h5.event(HCI_EVENT_READ, 0, hci_uart_h5.event_priv);
        }
    }
}

static void uart_event(rvm_dev_t *dev, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ || event_id == USART_OVERFLOW) {
        aos_event_set(&hci_uart_h5.uart_event, 0x1, AOS_EVENT_OR);
    }
}

static int h5_hal_open(rvm_dev_t *dev)
{
    aos_check(!aos_event_new(&hci_uart_h5.uart_event, 0), -EIO);

#if defined(CONFIG_HCI_UART_STATIC_STACK) && CONFIG_HCI_UART_STATIC_STACK
    aos_check(!krhino_task_create(&g_hci_task, "hci_uart", NULL,
                           CONFIG_HCI_UART_H5_TASK_PRIO, 0u, g_hci_task_stack,
                           CONFIG_HCI_UART_H5_TASK_STACK_SIZE/4, hci_uart_entry, 1u), -EIO);
#else
    aos_check(!aos_task_new_ext(&hci_uart_h5.task_handle, "hci_uart", hci_uart_entry, NULL, CONFIG_HCI_UART_H5_TASK_STACK_SIZE, CONFIG_HCI_UART_H5_TASK_PRIO), -EIO);
#endif

    char devname[32] = {0};
    snprintf(devname, sizeof(devname), "uart%d", hci_uart_h5.uart_idx);
    hci_uart_h5.uart_dev = rvm_hal_device_open(devname);
    if (!hci_uart_h5.uart_dev)
    {
        return -EIO;
    }

    rvm_hal_uart_config(hci_uart_h5.uart_dev, &hci_uart_h5.uart_config);

    rvm_hal_uart_set_event(hci_uart_h5.uart_dev, uart_event, NULL);

    rvm_hal_uart_set_type(hci_uart_h5.uart_dev, UART_TYPE_SYNC);

    return 0;
}

static int h5_hal_close(rvm_dev_t *dev)
{
    LOGD(TAG, "%s", __func__);

    rvm_hal_uart_close(hci_uart_h5.uart_dev);

    return 0;
}

static int h5_send_data(rvm_dev_t *dev, uint8_t *data, uint32_t size)
{
    BT_DBG(TAG, "hci send %s", bt_hex_real(data, size));
    return rvm_hal_uart_send(hci_uart_h5.uart_dev, data, size, AOS_WAIT_FOREVER);
}

static int h5_recv_data(rvm_dev_t *dev, uint8_t *data, uint32_t size)
{
    return rvm_hal_uart_recv(hci_uart_h5.uart_dev , data, size, 0);
}

static int h5_set_event(rvm_dev_t *dev, hci_event_cb_t event, void *priv)
{
    hci_uart_h5.event = event;
    hci_uart_h5.event_priv = priv;

    return 0;
}

static int h5_start(rvm_dev_t *dev, hci_driver_send_cmd_t send_cmd)
{
    hci_uart_h5.cmd_send = send_cmd;
    return send_cmd(HCI_VSC_H5_INIT, NULL, 0, NULL, NULL);
}

static rvm_dev_t *h5_hal_init(driver_t *drv, void *g_uart_config, int id)
{
    hci_uart_h5.hci_dev = (rvm_dev_t *)rvm_hal_device_new(drv, sizeof(rvm_dev_t), id);

    return (rvm_dev_t *)hci_uart_h5.hci_dev;
}

#define h5_hal_uninit rvm_hal_device_free

static hci_driver_t h5_driver = {
    .drv = {
        .name   = "hci",
        .init   = h5_hal_init,
        .uninit = h5_hal_uninit,
        .lpm    = NULL,
        .open   = h5_hal_open,
        .close  = h5_hal_close,
    },
    .set_event = h5_set_event,
    .send      = h5_send_data,
    .recv      = h5_recv_data,
    .start     = h5_start,
};

/**
 * @brief  register bluetooth driver of h5 
 * @param  [in] config
 * @return  
 */
void bt_hci_uart_h5_register(int uart_idx, rvm_hal_uart_config_t config)
{
    hci_uart_h5.uart_idx = uart_idx;
    hci_uart_h5.uart_config = config;
    rvm_driver_register(&h5_driver.drv, NULL, 0);
}
