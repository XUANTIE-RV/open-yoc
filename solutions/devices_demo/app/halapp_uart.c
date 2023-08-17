/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/uart.h>
#include <devices/devicelist.h>

#define UART_BUF_SIZE   20
#define UART_TX_TIMEOUT 1000
#define UART_RX_TIMEOUT 1000
#define UART_TEST_CNT   6

/* data buffer */
static char uart_data_buf_send[] = "I am uart!";
static char uart_data_buf_recv[UART_BUF_SIZE];
const uint8_t str_recv[] = "uart demo!";
static char uart_data_buf_send1[] = "I am uart!";
static char uart_data_buf_recv1[UART_BUF_SIZE];

extern const csi_pinmap_t uart_pinmap[];

static uint32_t get_uart_pin_func(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFF;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin) {
            ret = (uint32_t)map->pin_func;
            break;
        }

        map++;
    }

    return ret;
}

static uint8_t get_uart_idx(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint8_t ret = 0xFFU;
    uint8_t enum_len = sizeof(map->pin_name);
    printf("sizeof(map->pin_name):%d\n",enum_len);
    if (1 == enum_len) {
        while ((uint8_t)map->pin_name != 0xFFU) {
            if (map->pin_name == gpio_pin) {
                ret = (uint8_t)map->idx;
                break;
            }
        map++;
        }
    }else if (4 == enum_len) {
        while ((uint32_t)map->pin_name != 0xFFFFFFFFU) {
            if (map->pin_name == gpio_pin) {
                ret = (uint8_t)map->idx;
                break;
            }
        map++;
        }
    }
    return ret;
}

int hal_uart_demo(uint32_t *gpio_pins)
{
    int ret     = -1;

    printf("task: uart test start...\n");

    uint8_t uart_idx = get_uart_idx(gpio_pins[0], uart_pinmap);

    printf("uart_idx:%d\n", uart_idx);

    if (uart_idx > UART_TEST_CNT - 1) {
        printf("uart_idx is overflow. uart test failed.\n");
        return -1;
    }
#if !defined(CONFIG_CHIP_BL606P_E907) // 博流的板子UART的复用在board里面做掉了
    uint32_t pin0_func = get_uart_pin_func(gpio_pins[0], uart_pinmap);
    uint32_t pin1_func = get_uart_pin_func(gpio_pins[1], uart_pinmap);

    csi_pin_set_mux(gpio_pins[0], pin0_func);
    csi_pin_set_mux(gpio_pins[1], pin1_func);
#endif

    rvm_uart_drv_register(uart_idx);

    char file[8];
    sprintf(file, "uart%d", uart_idx);

    rvm_dev_t *uart_dev = NULL;

    uart_dev = rvm_hal_uart_open(file);
    if (uart_dev == NULL) {
        return -1;
    }

    rvm_hal_uart_config_t config;
    config.baud_rate    = 115200;
    config.data_width   = DATA_WIDTH_8BIT;
    config.parity       = PARITY_NONE;
    config.stop_bits    = STOP_BITS_1;
    config.flow_control = FLOW_CONTROL_DISABLED;
    config.mode         = MODE_TX_RX;
    

    rvm_hal_uart_config(uart_dev, &config);

    uint32_t buf_size = 1024;
    ret = rvm_hal_uart_set_buffer_size(uart_dev, buf_size);
    if (ret < 0) {
        printf("rvm_hal_uart_set_buffer_size failed.\n");
        return -1;
    }

    /* send data by task_uart */
    printf("# task: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);
    rvm_hal_uart_send(uart_dev, uart_data_buf_send, sizeof(uart_data_buf_send), AOS_WAIT_FOREVER);

    printf("\n\n\n# task: Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv, 0, sizeof(str_recv));
    /* receive data */
    rvm_hal_uart_recv(uart_dev, uart_data_buf_recv, sizeof(str_recv) + 1, 0xffffffff);
    uart_data_buf_recv[10] = '\0';

    if (0 == memcmp(str_recv, uart_data_buf_recv, sizeof(str_recv))) {
        printf("task: uart data received succeed !\n");
    } else {
        printf("task: uart test failed !\n");
        return -1;
    }

    ret = rvm_hal_uart_close(uart_dev);

    if (ret < 0) {
        printf("task: uart_close error !\n");
    }
    
    printf("task: uart test successfully\n");

    return 0;
}

int hal_uart_demo1(uint32_t *gpio_pins)
{
    int ret     = -1;

    printf("task1: uart test start...\n");

    uint8_t uart_idx = get_uart_idx(gpio_pins[0], uart_pinmap);

    printf("uart_idx:%d\n", uart_idx);

    if (uart_idx > UART_TEST_CNT - 1) {
        printf("uart_idx is overflow. uart test failed.\n");
        return -1;
    }

#if !defined(CONFIG_CHIP_BL606P_E907) // 博流的板子UART的复用在board里面做掉了
    uint32_t pin0_func = get_uart_pin_func(gpio_pins[0], uart_pinmap);
    uint32_t pin1_func = get_uart_pin_func(gpio_pins[1], uart_pinmap);

    csi_pin_set_mux(gpio_pins[0], pin0_func);
    csi_pin_set_mux(gpio_pins[1], pin1_func);
#endif

    rvm_uart_drv_register(uart_idx);

    char file[8];
    sprintf(file, "uart%d", uart_idx);

    rvm_dev_t *uart_dev = NULL;

    uart_dev = rvm_hal_uart_open(file);
    if (uart_dev == NULL) {
        return -1;
    }
    
    uint32_t buf_size = 1024;
    ret = rvm_hal_uart_set_buffer_size(uart_dev, buf_size);
    if (ret < 0) {
        printf("rvm_hal_uart_set_buffer_size failed.\n");
        return -1;
    }

    /* send data by task_uart */
    printf("# task1: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send1);
    rvm_hal_uart_send(uart_dev, uart_data_buf_send1, sizeof(uart_data_buf_send1), AOS_WAIT_FOREVER);

    printf("\n\n\n# task1: Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv1, 0, sizeof(str_recv));
    /* receive data */
    rvm_hal_uart_recv(uart_dev, uart_data_buf_recv1, sizeof(str_recv) + 1, 0xffffffff);
    uart_data_buf_recv1[10] = '\0';

    aos_msleep(10);

    if (0 == memcmp(str_recv, uart_data_buf_recv1, sizeof(str_recv))) {
        printf("task1: uart data received succeed !\n");
    } else {
        printf("task1: uart test failed !\n");
        return -1;
    }

    ret = rvm_hal_uart_close(uart_dev);

    if (ret < 0) {
        printf("task1: uart_close error !\n");
    }
    
    printf("task1: uart test successfully\n");

    return 0;
}

void uart_task(void *priv)
{
    uint32_t *port = (uint32_t *)priv;

    hal_uart_demo(port);
    aos_msleep(500);
}


void uart_task1(void *priv)
{
    uint32_t *port = (uint32_t *)priv;

    hal_uart_demo1(port);
    aos_msleep(500);
}

int hal_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins)
{
    aos_task_t task;

    aos_task_new_ext(&task, "uart_task", uart_task, (void *)task1_pins, 4096, 32);
    aos_task_new_ext(&task, "uart_task1", uart_task1, (void *)task2_pins, 4096, 32);

    return 0;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static aos_event_t event_write_read;
#define EVENT_UART_WRITE 0x00000001
#define EVENT_UART_READ  0x00000002

void uart_demo_event(rvm_dev_t *dev, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ) {
        aos_event_set(&event_write_read, EVENT_UART_READ, AOS_EVENT_OR);
    } else if (event_id == USART_EVENT_WRITE) {
        aos_event_set(&event_write_read, EVENT_UART_WRITE, AOS_EVENT_OR);
    }
}

int devfs_uart_demo(uint32_t *gpio_pins)
{
    int ret = -1;
    unsigned int actl_flags = 0;

    aos_event_new(&event_write_read, 0);

    printf("task: devfs uart test start...\n");

    uint8_t uart_idx = get_uart_idx(gpio_pins[0], uart_pinmap);

#if !defined(CONFIG_CHIP_BL606P_E907) // 博流的板子UART的复用在board里面做掉了
    uint32_t pin0_func = get_uart_pin_func(gpio_pins[0], uart_pinmap);
    uint32_t pin1_func = get_uart_pin_func(gpio_pins[1], uart_pinmap);

    csi_pin_set_mux(gpio_pins[0], pin0_func);
    csi_pin_set_mux(gpio_pins[1], pin1_func);
#endif

    rvm_uart_drv_register(uart_idx);

    char uartdev[16];
    sprintf(uartdev, "/dev/uart%d", uart_idx);

    int fd = open(uartdev, O_RDWR);
    printf("open %s fd:%d\n", uartdev, fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", uartdev, fd);
        return -1;
    }

    rvm_hal_uart_config_t config;

    ret = ioctl(fd, UART_IOC_GET_DEFAULT_CONFIG, &config);
    if (ret < 0) {
        printf("UART_IOC_GET_DEFAULT_CONFIG fail !\n");
        goto failure;
    }

    printf(
        "default config:\n"
        "               config.baud_rate        = %d\n"
        "               config.data_width       = %d\n"
        "               config.parity           = %d\n"
        "               config.stop_bits        = %d\n"
        "               config.flow_control     = %d\n"
        "               config.mode             = %d\n",
        config.baud_rate, config.data_width, config.parity, config.stop_bits, config.flow_control, config.mode
    );

    config.baud_rate    = 115200;
    config.data_width   = DATA_WIDTH_8BIT;
    config.parity       = PARITY_NONE;
    config.stop_bits    = STOP_BITS_1;
    config.flow_control = FLOW_CONTROL_DISABLED;
    config.mode         = MODE_TX_RX;

    ret = ioctl(fd, UART_IOC_SET_CONFIG, &config);
    if (ret < 0) {
        printf("UART_IOC_SET_CONFIG fail !\n");
        goto failure;
    }

    enum rvm_hal_uart_type_t type = UART_TYPE_ASYNC;

    ret = ioctl(fd, UART_IOC_SET_TYPE, &type);
    if (ret < 0) {
        printf("UART_IOC_SET_TYPE fail !\n");
        goto failure;
    }

    uint32_t buf_size = 1024;
    ret = ioctl(fd, UART_IOC_SET_BUFFER_SIZE, &buf_size);
    if (ret < 0) {
        printf("UART_IOC_SET_BUFFER_SIZE fail !\n");
        goto failure;
    }

    rvm_uart_dev_msg_t msg;
    msg.event = uart_demo_event;
    msg.priv = NULL;

    ret = ioctl(fd, UART_IOC_SET_EVENT, &msg);
    if (ret < 0) {
        printf("UART_IOC_SET_EVENT fail !\n");
        goto failure;
    }

    /* send data by task_uart */
    printf("# task: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);
    write(fd, uart_data_buf_send, sizeof(uart_data_buf_send));

    ret = aos_event_get(&event_write_read, EVENT_UART_WRITE, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);
    if (ret != 0) {
        return -1;
    }

    printf("\n\n\n# task: Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv, 0, sizeof(str_recv));
    /* receive data */
    read(fd, uart_data_buf_recv, sizeof(str_recv) + 1);

    ret = aos_event_get(&event_write_read, EVENT_UART_READ, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);
    if (ret != 0) {
        return -1;
    }

    uart_data_buf_recv[10] = '\0';

    if (0 == memcmp(str_recv, uart_data_buf_recv, sizeof(str_recv))) {
        printf("task: devfs uart data received succeed !\n");
    } else {
        printf("task: devfs uart test failed !\n");
        goto failure;
    }

    close(fd);
    printf("task: devfs uart test successfully\n");
    return 0;
failure:
    close(fd);
    return -1;
}

int devfs_uart_demo1(uint32_t *gpio_pins)
{
    int ret = -1;
    char devfs_uart_buf_recv[UART_BUF_SIZE] = {0};

    printf("task1: devfs uart test start...\n");

    uint8_t uart_idx = get_uart_idx(gpio_pins[0], uart_pinmap);

#if !defined(CONFIG_CHIP_BL606P_E907) // 博流的板子UART的复用在board里面做掉了
    uint32_t pin0_func = get_uart_pin_func(gpio_pins[0], uart_pinmap);
    uint32_t pin1_func = get_uart_pin_func(gpio_pins[1], uart_pinmap);

    csi_pin_set_mux(gpio_pins[0], pin0_func);
    csi_pin_set_mux(gpio_pins[1], pin1_func);
#endif

    rvm_uart_drv_register(uart_idx);

    char uartdev[16];
    sprintf(uartdev, "/dev/uart%d", uart_idx);

    int fd = open(uartdev, O_RDWR);
    printf("open %s fd:%d\n", uartdev, fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", uartdev, fd);
        return -1;
    }

    /* send data by task_uart */
    printf("# task1: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);

    rvm_uart_dev_msg_t msg_send;
    msg_send.size = sizeof(uart_data_buf_send);
    msg_send.buffer = uart_data_buf_send;

    ret = ioctl(fd, UART_IOC_SEND_POLL, &msg_send);
    if (ret < 0) {
        printf("UART_IOC_SEND_POLL fail !\n");
        goto failure;
    }

    printf("\n\n\n# task1: Start receive buffer, please iput: %s\n", str_recv);

    memset(devfs_uart_buf_recv, 0, sizeof(str_recv));

    /* receive data */
    rvm_uart_dev_msg_t msg_recv;
    msg_recv.size = sizeof(str_recv) + 1;
    msg_recv.buffer = devfs_uart_buf_recv;

    ret = ioctl(fd, UART_IOC_RECV_POLL, &msg_recv);
    if (ret < 0) {
        printf("UART_IOC_RECV_POLL fail !\n");
        goto failure;
    }

    devfs_uart_buf_recv[10] = '\0';

    if (0 == memcmp(str_recv, devfs_uart_buf_recv, sizeof(str_recv))) {
        printf("task1: devfs uart data received succeed !\n");
    } else {
        printf("task1: devfs uart test failed !\n");
        goto failure;
    }

    close(fd);
    printf("task1: devfs uart test successfully\n");
    return 0;
failure:
    close(fd);
    return -1;
}

void devfs_uart_task(void *priv)
{
    uint32_t *port = (uint32_t *)priv;

    devfs_uart_demo(port);
    aos_msleep(500);
}


void devfs_uart_task1(void *priv)
{
    uint32_t *port = (uint32_t *)priv;

    devfs_uart_demo1(port);
    aos_msleep(500);
}

int devfs_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins)
{
    aos_task_t task;

    aos_task_new_ext(&task, "uart_task", devfs_uart_task, (void *)task1_pins, 4096, 32);
    aos_task_new_ext(&task, "uart_task1", devfs_uart_task1, (void *)task2_pins, 4096, 32);

    return 0;
}

#endif