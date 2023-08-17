/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <aos/ringbuffer.h>
#include <drv/uart.h>
#include <drv/clk.h>
#include <yoc/lpm.h>
#include <devices/impl/uart_impl.h>

#ifdef CONFIG_HAL_UART_RINGBUF_LEN
#define UART_RB_SIZE (CONFIG_HAL_UART_RINGBUF_LEN)
#else
#define UART_RB_SIZE (1024)
#endif

#ifdef CONFIG_HAL_UART_FIFO_SIZE
#define HAL_UART_FIFO_SIZE CONFIG_HAL_UART_FIFO_SIZE
#else
#define HAL_UART_FIFO_SIZE (256)
#endif

#ifdef CONFIG_HAL_UART_NUM
#define HAL_UART_NUM (CONFIG_HAL_UART_NUM)
#else
#define HAL_UART_NUM (6)
#endif

static int8_t uart_shadow_list[HAL_UART_NUM];

#define TAG "uart_drv"

#define EVENT_WRITE 0x000000F0
#define EVENT_READ  0x0000000F

typedef struct {
    rvm_dev_t      device;
    csi_uart_t     handle;
    aos_event_t    event_write_read;
    char          *recv_buf;
    dev_ringbuf_t  read_buffer;
    void (*rx_cb)(rvm_dev_t *dev, int event_id, void *priv);
    void *priv;
    int type;
    int dma_en;
    rvm_hal_uart_config_t config;
    csi_dma_ch_t g_dma_ch_tx;
    csi_dma_ch_t g_dma_ch_rx;
} uart_dev_t;

#define UART(dev) ((uart_dev_t *)dev)

static int uart_csky_config(rvm_dev_t *dev, rvm_hal_uart_config_t *config);

static void usart_csky_event_cb_fun(csi_uart_t *uart_handler, csi_uart_event_t event, void *arg)
{
    uart_dev_t* uart = (uart_dev_t *)arg;

    if (!uart)
        return;

    switch (event) {
    case UART_EVENT_SEND_COMPLETE:
        aos_event_set(&uart->event_write_read, EVENT_WRITE, AOS_EVENT_OR);
        if (uart->rx_cb) {
            uart->rx_cb(&uart->device, USART_EVENT_WRITE, uart->priv);
        }
        break;

    case UART_EVENT_RECEIVE_COMPLETE: {
        aos_event_set(&uart->event_write_read, EVENT_READ, AOS_EVENT_OR);
        if (uart->rx_cb) {
            uart->rx_cb(&uart->device, USART_EVENT_READ, uart->priv);
        }
        break;
    };

    case UART_EVENT_RECEIVE_FIFO_READABLE: {
        int ret = 0;
        if (uart->recv_buf != NULL) {
            uint8_t temp_buf[HAL_UART_FIFO_SIZE] = {0};
            do {
                ret = csi_uart_receive(&uart->handle, temp_buf, HAL_UART_FIFO_SIZE, 0);
                if (ret > 0) {
                    if (ringbuffer_write(&uart->read_buffer, (uint8_t*)temp_buf, ret) != ret) {
                        break;
                    }
                }
            } while (ret);
        }
        aos_event_set(&uart->event_write_read, EVENT_READ, AOS_EVENT_OR);
        if (uart->rx_cb) {
            uart->rx_cb(&uart->device, USART_EVENT_READ, uart->priv);
        }
        break;
    }

    case UART_EVENT_ERROR_OVERFLOW: {
        int ret = 0;
        uint8_t temp_buf[HAL_UART_FIFO_SIZE];
        do {
            ret = csi_uart_receive(&uart->handle, temp_buf, HAL_UART_FIFO_SIZE, 0);
        } while(ret);

        ringbuffer_clear(&uart->read_buffer);
        aos_event_set(&uart->event_write_read, ~EVENT_READ, AOS_EVENT_AND);
        if (uart->rx_cb) {
            uart->rx_cb(&uart->device, USART_OVERFLOW, uart->priv);
        }
        break;
    }

    case UART_EVENT_ERROR_FRAMING:
    default:
        // LOGW(TAG, "uart%d event %d", idx, event);
        break;
    }
}

static rvm_dev_t *uart_csky_init(driver_t *drv, void *config, int id)
{
    uart_dev_t *uart = (uart_dev_t *)rvm_hal_device_new(drv, sizeof(uart_dev_t), id);

    if (uart && config) {
        memcpy(&uart->config, config, sizeof(uart->config));
    }
    return (rvm_dev_t *)uart;
}

#define uart_csky_uninit rvm_hal_device_free

static int uart_csky_open(rvm_dev_t *dev)
{
    uint8_t port = dev->id & (~RVM_HAL_SHADOW_PORT_MASK);

    /* shadow dev check */
    if (dev->id & RVM_HAL_SHADOW_PORT_MASK) {
        uart_shadow_list[port] = 1;
    }

    if (aos_event_new(&UART(dev)->event_write_read, 0) != 0) {
        goto error2;
    }

    UART(dev)->recv_buf = (char *)aos_malloc(UART_RB_SIZE);

    if (UART(dev)->recv_buf == NULL) {
        goto error1;
    }

    ringbuffer_create(&UART(dev)->read_buffer, UART(dev)->recv_buf, UART_RB_SIZE);

    int ret = csi_uart_init(&UART(dev)->handle, port);
    if (ret != CSI_OK) {
        goto error0;
    }

    UART(dev)->dma_en = 0;

    if (UART(dev)->config.baud_rate != 0) {
        ret = uart_csky_config(dev, &UART(dev)->config);
        if (ret != 0) {
            goto error0;
        }
    }

    ret = csi_uart_attach_callback(&UART(dev)->handle, usart_csky_event_cb_fun, dev);
    if (ret != CSI_OK) {
        goto error0;
    }

    return 0;

error0:
    aos_free(UART(dev)->recv_buf);
error1:
    aos_event_free(&UART(dev)->event_write_read);
error2:
    return -1;
}

static int uart_csky_close(rvm_dev_t *dev)
{
    /* shadow dev check */
    uint8_t port = dev->id & (~RVM_HAL_SHADOW_PORT_MASK);
    uart_shadow_list[port] = 0;

    if (UART(dev)->dma_en) {
        csi_uart_link_dma(&UART(dev)->handle, NULL, NULL);
        UART(dev)->dma_en = 0;
    }
    csi_uart_detach_callback(&UART(dev)->handle);
    csi_uart_uninit(&UART(dev)->handle);
    aos_event_free(&UART(dev)->event_write_read);
    aos_free(UART(dev)->recv_buf);
    UART(dev)->recv_buf = NULL;
    UART(dev)->rx_cb = NULL;

    return 0;
}

static int uart_csky_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&UART(dev)->handle.dev);
    } else {
        csi_clk_disable(&UART(dev)->handle.dev);
    }
    return 0;
}

static int uart_csky_config(rvm_dev_t *dev, rvm_hal_uart_config_t *config)
{
    csi_uart_parity_t parity;
    csi_uart_stop_bits_t stop_bits;
    csi_uart_data_bits_t data_bits;
    csi_uart_flowctrl_t flow_control;

    if (!config) {
        return -EINVAL;
    }

    switch (config->parity)
    {
    case PARITY_NONE:
        parity = UART_PARITY_NONE;
        break;
    case PARITY_ODD:
        parity = UART_PARITY_ODD;
        break;
    case PARITY_EVEN:
        parity = UART_PARITY_EVEN;
        break;
    default:
        parity = UART_PARITY_NONE;
        break;
    }

    switch (config->stop_bits)
    {
    case STOP_BITS_1:
        stop_bits = UART_STOP_BITS_1;
        break;
    case STOP_BITS_2:
        stop_bits = UART_STOP_BITS_2;
        break;
    default:
        stop_bits = UART_STOP_BITS_1_5;
        break;
    }

    switch (config->data_width)
    {
    case DATA_WIDTH_5BIT:
        data_bits = UART_DATA_BITS_5;
        break;
    case DATA_WIDTH_6BIT:
        data_bits = UART_DATA_BITS_6;
        break;
    case DATA_WIDTH_7BIT:
        data_bits = UART_DATA_BITS_7;
        break;
    case DATA_WIDTH_8BIT:
        data_bits = UART_DATA_BITS_8;
        break;
    case DATA_WIDTH_9BIT:
        data_bits = UART_DATA_BITS_9;
        break;
    default:
        data_bits = UART_DATA_BITS_8;
        break;
    }

    switch (config->flow_control)
    {
    case FLOW_CONTROL_DISABLED:
        flow_control = UART_FLOWCTRL_NONE;
        break;
    case FLOW_CONTROL_CTS:
        flow_control = UART_FLOWCTRL_CTS;
        break;
    case FLOW_CONTROL_RTS:
        flow_control = UART_FLOWCTRL_RTS;
        break;
    case FLOW_CONTROL_CTS_RTS:
        flow_control = UART_FLOWCTRL_RTS_CTS;
        break;
    default:
        flow_control = UART_FLOWCTRL_NONE;
        break;
    }

    int ret = csi_uart_baud(&UART(dev)->handle, config->baud_rate);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_uart_baud failed.");
        return -EIO;
    }

    ret = csi_uart_format(&UART(dev)->handle, data_bits, parity, stop_bits);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_uart_format failed.");
        return -EIO;
    }

    ret = csi_uart_flowctrl(&UART(dev)->handle, flow_control);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_uart_flowctrl failed.");
        return -EIO;
    }

    memcpy(&UART(dev)->config, config, sizeof(UART(dev)->config));

    return 0;
}

static int uart_csky_config_get(rvm_dev_t *dev, rvm_hal_uart_config_t *config)
{
    if (!config) {
        return -EINVAL;
    }
    memcpy(config, &UART(dev)->config, sizeof(UART(dev)->config));
    return 0;
}

static int uart_csky_set_type(rvm_dev_t *dev, int type)
{
    UART(dev)->type = type;

    return 0;
}

static int uart_csky_set_buffer_size(rvm_dev_t *dev, uint32_t size)
{
    char *tmp = NULL;

    if (size > 0) {
        tmp = aos_malloc(size);

        if (tmp == NULL) {
            return -ENOMEM;
        }
    }

    aos_free(UART(dev)->recv_buf);

    UART(dev)->recv_buf = tmp;
    if (tmp)
        ringbuffer_create(&UART(dev)->read_buffer, tmp, size);

    return 0;
}

static int uart_csky_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout_ms)
{
    int num;
    csi_error_t ret;

    /* if shadow dev inited, normal dev stop working */
    uint8_t port = dev->id & (~RVM_HAL_SHADOW_PORT_MASK);
    if (dev->id & RVM_HAL_SHADOW_PORT_MASK) {
        if (uart_shadow_list[port] == 0) {
            return -1;
        }
    } else {
        if (uart_shadow_list[port]) {
            return -1;
        }
    }

    if (UART(dev)->type == UART_TYPE_SYNC) {
        num = csi_uart_send(&UART(dev)->handle, data, size, timeout_ms);
        if (num != size) {
            return -1;
        }
        return num;
    } else {
        unsigned int actl_flags = 0;
        ret = csi_uart_send_async(&UART(dev)->handle, data, size);
        if (ret != CSI_OK) {
            return -1;
        }
        ret = aos_event_get(&UART(dev)->event_write_read, EVENT_WRITE, AOS_EVENT_OR_CLEAR, &actl_flags, timeout_ms);
        if (ret != 0) {
            return -1;
        }
    }

    return 0;
}

static int uart_csky_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout_ms)
{
    uint32_t     recv_size;
    unsigned int actl_flags = 0;
    int32_t      ret = 0;
    long long    time_enter, used_time;
    void *       temp_buf   = data;
    uint32_t     temp_count = size;

    /* if shadow dev inited, normal dev stop working */
    uint8_t port = dev->id & (~RVM_HAL_SHADOW_PORT_MASK);
    if (dev->id & RVM_HAL_SHADOW_PORT_MASK) {
        if (uart_shadow_list[port] == 0) {
            /* shadow port but not init */
            return 0;
        }
    } else {
        if (uart_shadow_list[port]) {
            aos_msleep(100);
            return 0;
        }
    }

    time_enter = aos_now_ms();

    while (1) {
        if (UART(dev)->recv_buf != NULL) {
            ret = ringbuffer_read(&UART(dev)->read_buffer, (uint8_t *)temp_buf, temp_count);
        } else {
            ret = csi_uart_receive(&UART(dev)->handle, (uint8_t *)temp_buf, temp_count, 0);
        }
        temp_count = temp_count - ret;
        temp_buf   = (uint8_t *)temp_buf + ret;
        used_time  = aos_now_ms() - time_enter;

        if (timeout_ms <= used_time || temp_count == 0 || timeout_ms == 0) {
            break;
        }

        if (aos_event_get(&UART(dev)->event_write_read, EVENT_READ, AOS_EVENT_OR_CLEAR, &actl_flags,
                          timeout_ms - used_time) == -1) {
            break;
        }
    }
    recv_size = size - temp_count;
    return recv_size;
}

static int uart_csky_send_poll(rvm_dev_t *dev, const void *data, uint32_t size)
{
    uint32_t trans_num = 0U;
    uint8_t *ch = (uint8_t *)data;

    while (trans_num < size) {
        csi_uart_putc(&UART(dev)->handle, *ch++);
        trans_num++;
    }

    return trans_num;
}

static int uart_csky_recv_poll(rvm_dev_t *dev, void *data, uint32_t size)
{
    uint8_t *temp_data = (uint8_t *)data;
    uint32_t recv_num = 0U;

    while (recv_num < size) {
        *temp_data = csi_uart_getc(&UART(dev)->handle);
        recv_num++;
        temp_data++;
    }

    return recv_num;
}

static void uart_csky_event(rvm_dev_t *dev, void (*event)(rvm_dev_t *dev, int event_id, void *priv),
                            void * priv)
{
    UART(dev)->priv        = priv;
    UART(dev)->rx_cb       = event;
}

static int uart_csky_trans_dma_enable(rvm_dev_t *dev, bool enable)
{
    csi_error_t ret;

    if (!enable) {
        ret = csi_uart_link_dma(&UART(dev)->handle, NULL, NULL);
        if (ret != CSI_OK) {
            return -1;
        }
        UART(dev)->dma_en = 0;
    } else {
        ret = csi_uart_link_dma(&UART(dev)->handle, &UART(dev)->g_dma_ch_tx, &UART(dev)->g_dma_ch_rx);
        if (ret != CSI_OK) {
            return -1;
        }
        UART(dev)->dma_en = 1;
    }
    return 0;
}

static uart_driver_t uart_driver = {
    .drv = {
        .name   = "uart",
        .init   = uart_csky_init,
        .uninit = uart_csky_uninit,
        .open   = uart_csky_open,
        .close  = uart_csky_close,
        .clk_en = uart_csky_clock
    },
    .config          = uart_csky_config,
    .config_get      = uart_csky_config_get,
    .set_type        = uart_csky_set_type,
    .set_buffer_size = uart_csky_set_buffer_size,
    .send            = uart_csky_send,
    .recv            = uart_csky_recv,
    .send_poll       = uart_csky_send_poll,
    .recv_poll       = uart_csky_recv_poll,
    .set_event       = uart_csky_event,
    .trans_dma_enable = uart_csky_trans_dma_enable,
};

void rvm_uart_drv_register(int uart_idx)
{
    rvm_driver_register(&uart_driver.drv, NULL, uart_idx);
}
