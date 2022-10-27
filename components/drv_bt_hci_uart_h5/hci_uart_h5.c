/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#define LOG_TAG "HCI_UART_H5"


#include <devices/hal/hci_impl.h>
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
    uart_config_t uart_config;
    aos_dev_t *uart_dev;
    aos_event_t uart_event;
    aos_task_t task_handle;
    hci_driver_send_cmd_t cmd_send;
    hci_event_cb_t event;
    void *event_priv;
    aos_dev_t *hci_dev;
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

static void uart_event(aos_dev_t *dev, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ || event_id == USART_OVERFLOW) {
        aos_event_set(&hci_uart_h5.uart_event, 0x1, AOS_EVENT_OR);
    }
}

static int h5_hal_open(aos_dev_t *dev)
{
    aos_check(!aos_event_new(&hci_uart_h5.uart_event, 0), -EIO);

#if defined(CONFIG_HCI_UART_STATIC_STACK) && CONFIG_HCI_UART_STATIC_STACK
    aos_check(!krhino_task_create(&g_hci_task, "hci_uart", NULL,
                           CONFIG_HCI_UART_H5_TASK_PRIO, 0u, g_hci_task_stack,
                           CONFIG_HCI_UART_H5_TASK_STACK_SIZE/4, hci_uart_entry, 1u), -EIO);
#else
    aos_check(!aos_task_new_ext(&hci_uart_h5.task_handle, "hci_uart", hci_uart_entry, NULL, CONFIG_HCI_UART_H5_TASK_STACK_SIZE, CONFIG_HCI_UART_H5_TASK_PRIO), -EIO);
#endif

    hci_uart_h5.uart_dev = uart_open_id("uart", hci_uart_h5.uart_idx);
    if (!hci_uart_h5.uart_dev)
    {
        return -EIO;
    }

    uart_config(hci_uart_h5.uart_dev, &hci_uart_h5.uart_config);

    uart_set_event(hci_uart_h5.uart_dev, uart_event, NULL);

    uart_set_type(hci_uart_h5.uart_dev, UART_TYPE_CONSOLE);

    return 0;
}

static int h5_hal_close(aos_dev_t *dev)
{
    LOGD(TAG, "%s", __func__);

    uart_close(hci_uart_h5.uart_dev);    

    return 0;
}

static int h5_send_data(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    BT_DBG(TAG, "hci send %s", bt_hex_real(data, size));
    return uart_send(hci_uart_h5.uart_dev, data, size);
}

static int h5_recv_data(aos_dev_t *dev, uint8_t *data, uint32_t size)
{
    return uart_recv(hci_uart_h5.uart_dev , data, size, 0);
}

static int h5_set_event(aos_dev_t *dev, hci_event_cb_t event, void *priv)
{
    hci_uart_h5.event = event;
    hci_uart_h5.event_priv = priv;

    return 0;
}

static int h5_start(aos_dev_t *dev, hci_driver_send_cmd_t send_cmd)
{
    hci_uart_h5.cmd_send = send_cmd;
    return send_cmd(HCI_VSC_H5_INIT, NULL, 0, NULL, NULL);
}

static aos_dev_t *h5_hal_init(driver_t *drv, void *g_uart_config, int id)
{
    hci_uart_h5.hci_dev = (aos_dev_t *)device_new(drv, sizeof(aos_dev_t), id);

    return (aos_dev_t *)hci_uart_h5.hci_dev;
}

#define h5_hal_uninit device_free

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
void bt_hci_uart_h5_register(int uart_idx, uart_config_t config)
{
    hci_uart_h5.uart_idx = uart_idx;
    hci_uart_h5.uart_config = config;
    driver_register(&h5_driver.drv, NULL, 0);
}
