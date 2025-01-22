 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <aos/aos.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <aos/console_uart.h>
#include <devices/uart.h>

static char console_devname[16] = {0};
static uint16_t g_console_buf_size = 128;
static aos_mutex_t g_console_mutex_handle;
static rvm_dev_t *g_console_handle;

void console_init(int idx, uint32_t baud, uint16_t buf_size)
{
    rvm_hal_uart_config_t config;

    aos_mutex_new(&g_console_mutex_handle);
    snprintf(console_devname, sizeof(console_devname), "uart%d", idx);
    g_console_handle = rvm_hal_uart_open(console_devname);
    if (g_console_handle) {
        config.baud_rate = baud;
        config.mode = MODE_TX_RX;
        config.flow_control = FLOW_CONTROL_DISABLED;
        config.stop_bits = STOP_BITS_1;
        config.parity = PARITY_NONE;
        config.data_width = DATA_WIDTH_8BIT;
        rvm_hal_uart_config(g_console_handle, &config);
        g_console_buf_size = buf_size;
        return;
    }
    console_devname[0] = 0;
}

void console_deinit(void)
{
    if (g_console_handle) {
        rvm_hal_uart_close(g_console_handle);
        g_console_handle = NULL;
    }
    aos_mutex_free(&g_console_mutex_handle);
}

void *console_get_uart(void)
{
    return g_console_handle;
}

const char *console_get_devname(void)
{
    return console_devname;
}

uint16_t console_get_buffer_size(void)
{
    return g_console_buf_size;
}

#if defined(CONFIG_AOS_NEWLINE_SUPPORT) && (CONFIG_AOS_NEWLINE_SUPPORT > 0)
int uart_write(const void *buf, size_t size)
{
    CHECK_PARAM(g_console_handle, -1);
    if (aos_irq_context() || aos_is_irq_disable()) {
        /* FIXME: use aos_debug_printf as printk instead when in irq-context or irq save context */
        char *error = "can't use printf in irq-context or irq disable context\r\n";
        rvm_hal_uart_send_poll(g_console_handle, error, strlen(error));
    } else if (aos_is_sched_disable()) {
        /* may be sched is off when lpm prepare */
        if((size >= 2 && ((char *)buf)[size - 1] == '\n' && ((char *)buf)[size - 2] != '\r')) {
            rvm_hal_uart_send_poll(g_console_handle, buf, size - 1);
            rvm_hal_uart_send_poll(g_console_handle, "\r\n", 2);
        } else if(size == 1 && ((char *)buf)[0] == '\n') {
            rvm_hal_uart_send_poll(g_console_handle, "\r\n", 2);
        } else {
            rvm_hal_uart_send_poll(g_console_handle, buf, size);
        }
    } else {
        /* normal state, called by application */
        if((size >= 2 && ((char *)buf)[size - 1] == '\n' && ((char *)buf)[size - 2] != '\r')) {
            rvm_hal_uart_send(g_console_handle, buf, size - 1, AOS_WAIT_FOREVER);
            rvm_hal_uart_send(g_console_handle, "\r\n", 2, AOS_WAIT_FOREVER);
        } else if(size == 1 && ((char *)buf)[0] == '\n') {
            rvm_hal_uart_send(g_console_handle, "\r\n", 2, AOS_WAIT_FOREVER);
        } else {
            rvm_hal_uart_send(g_console_handle, buf, size, AOS_WAIT_FOREVER);
        }
    }
    return size;
}
#else
int uart_write(const void *buf, size_t size)
{
    CHECK_PARAM(g_console_handle, -1);
    if (aos_irq_context() || aos_is_irq_disable()) {
        /* FIXME: use aos_debug_printf as printk instead when in irq-context or irq save context */
        char *error = "can't use printf in irq-context or irq disable context\r\n";
        rvm_hal_uart_send_poll(g_console_handle, error, strlen(error));
    } else if (aos_is_sched_disable()) {
        /* may be sched is off when lpm prepare */
        rvm_hal_uart_send_poll(g_console_handle, buf, size);
    } else {
        /* normal state, called by application */
        rvm_hal_uart_send(g_console_handle, buf, size, AOS_WAIT_FOREVER);
    }

    return size;
}
#endif /*CONFIG_AOS_NEWLINE_SUPPORT*/

int uart_putc(int ch)
{
    return uart_write((const void*)&ch, 1);
}

int uart_getc(void)
{
    if (g_console_handle != NULL) {
        char ch = 0;
        rvm_hal_uart_recv(g_console_handle, &ch, 1, AOS_WAIT_FOREVER);
        return (int)ch;
    }
    return 0;
}

__attribute__((weak)) int os_critical_enter(unsigned int *lock)
{
    int ret;
    CHECK_PARAM(g_console_handle, -1);
    ret = aos_mutex_lock(&g_console_mutex_handle,10000);
    return ret;
}

__attribute__((weak)) int os_critical_exit(unsigned int *lock)
{
    int ret;
    CHECK_PARAM(g_console_handle, -1);
    ret = aos_mutex_unlock(&g_console_mutex_handle);
    return ret;
}
