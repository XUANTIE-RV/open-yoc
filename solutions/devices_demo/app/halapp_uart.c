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
#define UART_POLL_TIMEOUT 5000

/* data buffer */
static char uart_data_buf_send[] = "I am uart!";
const uint8_t str_recv[] = "uart demo!";

typedef struct uart_task_dev_info {
    uint32_t *task_pins;
    rvm_dev_t *uart_dev;
}uart_task_dev_info;
uart_task_dev_info  uart_task_dev[2];
extern const csi_pinmap_t uart_pinmap[];
extern uint8_t is_uart_task;

static int g_uart_tasks_quit;

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

int hal_uart_init(rvm_dev_t **uart_dev_handle, uint32_t *gpio_pins)
{
    int ret     = -1;
    int buf_size = 512;

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

    ret = rvm_hal_uart_config(uart_dev, &config);
    if (ret < 0) {
        printf("rvm_hal_uart_config fail\n");
    }
    ret = rvm_hal_uart_set_buffer_size(uart_dev, buf_size);
    if (ret < 0) {
        printf("rvm_hal_uart_set_buffer_size failed.\n");
        goto failure;
    }
    *uart_dev_handle = uart_dev;
    return 0;
failure:
    rvm_hal_uart_close(uart_dev);
    return -1;
}

int hal_uart_demo(uint32_t *gpio_pins)
{
    int ret = -1;
    rvm_dev_t *uart_dev = NULL;
    char uart_data_buf_recv[UART_BUF_SIZE] = {0};
    if (!is_uart_task) {
        ret = hal_uart_init(&uart_dev, gpio_pins);
        if (ret) {
            printf("hal_uart_init fail\n");
            return -1;
        }
    } else {
        if (uart_task_dev[0].task_pins == gpio_pins) {
            uart_dev = uart_task_dev[0].uart_dev;
        } else {
            uart_dev = uart_task_dev[1].uart_dev;
        }
    }
    /* send data by task_uart */
    printf("# task: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);
    rvm_hal_uart_send(uart_dev, uart_data_buf_send, sizeof(uart_data_buf_send), AOS_WAIT_FOREVER);

    printf("\n\n\n# task: Start receive buffer, please iput: %s\n", str_recv);
    rvm_hal_uart_recv(uart_dev, uart_data_buf_recv, sizeof(str_recv) + 1, AOS_WAIT_FOREVER);
    uart_data_buf_recv[10] = '\0';
    if (0 == memcmp(str_recv, uart_data_buf_recv, sizeof(str_recv))) {
        printf("task: uart data received succeed !\n");
    } else {
        printf("uart_data_buf_recv:%s\n",uart_data_buf_recv);
        printf("task: uart test failed !\n");
        goto failure;
    }

    ret = rvm_hal_uart_close(uart_dev);

    if (ret < 0) {
        printf("task: uart_close error !\n");
        goto failure;
    }
    
    printf("task: uart test successfully\n");

    return 0;
failure:
    rvm_hal_uart_close(uart_dev);
    return -1;
}

int hal_uart_poll_demo(uint32_t *gpio_pins)
{
    printf("hal_uart_poll_demo not support now!\n");
    return 0;
}


void uart_task(void *priv)
{
    printf("uart_task start\n");
    uint32_t *port = (uint32_t *)priv;
    hal_uart_demo(port);
    printf("uart_task end\n");
    g_uart_tasks_quit |= 1 << 0; 
    
}


void uart_task1(void *priv)
{
    printf("uart_task1 start\n");
    uint32_t *port = (uint32_t *)priv;
    hal_uart_demo(port);
    printf("uart_task1 end\n");
    g_uart_tasks_quit |= 1 << 1;
}

int hal_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins)
{
    int ret = -1;
    int i = 0;
    aos_task_t task;
    g_uart_tasks_quit = 0;
    uart_task_dev[0].task_pins = task1_pins;
    uart_task_dev[1].task_pins = task2_pins;
    for (i=0;i<2;i++) {
        ret = hal_uart_init(&uart_task_dev[i].uart_dev, uart_task_dev[i].task_pins);
        if (ret) {
            printf("hal uart task init fail\n");
            return -1;
        }
    }
    aos_task_new_ext(&task, "uart_task", uart_task, (void *)task1_pins, 4096, 32);
    aos_task_new_ext(&task, "uart_task1", uart_task1, (void *)task2_pins, 4096, 32);
    
    while (g_uart_tasks_quit != 0x03) {
        aos_msleep(100);
    }

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

typedef struct devfs_uart_task_dev_info {
    uint32_t *task_pins;
    int fd;
}devfs_uart_task_dev_info;
devfs_uart_task_dev_info  devfs_uart_task_dev[2];

void uart_demo_event(rvm_dev_t *dev, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ) {
        aos_event_set(&event_write_read, EVENT_UART_READ, AOS_EVENT_OR);
    } else if (event_id == USART_EVENT_WRITE) {
        aos_event_set(&event_write_read, EVENT_UART_WRITE, AOS_EVENT_OR);
    }
}

int devfs_uart_init(int*devfs_fd, uint32_t *gpio_pins)
{
    int ret = -1;
    printf("devfs_uart_init...\n");

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
    *devfs_fd = fd;
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
    return 0;

failure:
    close(fd);
    return -1;
}

int devfs_uart_demo(uint32_t *gpio_pins)
{
    int ret = -1;
    unsigned int actl_flags = 0;
    char uart_data_buf_recv[UART_BUF_SIZE] = {0};
    aos_event_new(&event_write_read, 0);
    printf("task: devfs uart test start...\n");
    int fd = 0;
    if (!is_uart_task) {
       ret = devfs_uart_init(&fd, gpio_pins);
       if (ret) {
           printf("devfs_uart_init fail\n");
           return -1;
       }
    } else {
        if (devfs_uart_task_dev[0].task_pins == gpio_pins) {
            fd = devfs_uart_task_dev[0].fd;
        } else {
            fd = devfs_uart_task_dev[1].fd;
        }
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
        goto failure;
    }

    printf("\n\n\n# task: Start receive buffer, please iput: %s\n", str_recv);

    memset(uart_data_buf_recv, 0, sizeof(str_recv));
    read(fd, uart_data_buf_recv, sizeof(str_recv) + 1);
    ret = aos_event_get(&event_write_read, EVENT_UART_READ, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);
    if (ret != 0) {
        goto failure;
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
    int fd = -1;
    char devfs_uart_buf_recv[UART_BUF_SIZE] = {0};
    if (!is_uart_task) {
        ret = devfs_uart_init(&fd, gpio_pins);
        if (ret) {
            printf("devfs_uart_init fail\n");
            return -1;
        }
    } else {
        if (devfs_uart_task_dev[0].task_pins == gpio_pins) {
            fd = devfs_uart_task_dev[0].fd;
        } else {
            fd = devfs_uart_task_dev[1].fd;
        }
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
    msg_recv.size = sizeof(str_recv)+1;
    msg_recv.buffer = devfs_uart_buf_recv;

    ret = ioctl(fd, UART_IOC_RECV_POLL, &msg_recv);
    if (ret < 0) {
        printf("UART_IOC_RECV_POLL fail !\n");
        goto failure;
    }

    devfs_uart_buf_recv[10] = '\0';
    if (0 == memcmp(str_recv, devfs_uart_buf_recv, sizeof(str_recv))) {
        printf("devfs_task1: devfs uart data received succeed !\n");
    } else {
        printf("devfs_task1: devfs uart test failed !\n");
        goto failure;
    }

    close(fd);
    printf("devfs_task1: devfs uart test successfully\n");
    return 0;
failure:
    close(fd);
    return -1;
}

void devfs_uart_task(void *priv)
{
    uint32_t *port = (uint32_t *)priv;
    printf("devfs_uart_task start\n");
    devfs_uart_demo(port);
    printf("devfs_uart_task end\n");
    g_uart_tasks_quit |= 1 << 0; 
}


void devfs_uart_task1(void *priv)
{
    printf("devfs_uart_task1 start\n");
    uint32_t *port = (uint32_t *)priv;
    devfs_uart_demo1(port);
    printf("devfs_uart_task1 end\n");
    g_uart_tasks_quit |= 1 << 1; 
}

int devfs_task_uart_demo(uint32_t *task1_pins, uint32_t *task2_pins)
{
    int ret = -1;
    int i = 0;
    aos_task_t task;
    g_uart_tasks_quit = 0;
    devfs_uart_task_dev[0].task_pins = task1_pins;
    devfs_uart_task_dev[1].task_pins = task2_pins;
    for (i=0;i<2;i++) {
        ret = devfs_uart_init(&devfs_uart_task_dev[i].fd, devfs_uart_task_dev[i].task_pins);
        if (ret) {
            printf("hal uart task init fail\n");
            return -1;
        }
    }

    aos_task_new_ext(&task, "uart_task", devfs_uart_task, (void *)task1_pins, 4096, 32);
    aos_task_new_ext(&task, "uart_task1", devfs_uart_task1, (void *)task2_pins, 4096, 32);

    while (g_uart_tasks_quit != 0x03) {
        aos_msleep(100);
    }
    return 0;
}

int devfs_uart_poll_demo(uint32_t *gpio_pins) 
{
    int fd = -1;
    int ret = -1;
    int poll_time = 0;
    char poll_recv_buf[20] = {0};
    static struct pollfd fds[1] = {0};
    printf("devfs_uart_poll_demo \n");
    if (devfs_uart_init(&fd, gpio_pins)) {
        printf("devfs_uart_init is error\n");
        return -1;
    }

    fds[0].fd = fd;
    fds[0].events = POLLOUT|POLLERR;
    
    ret = poll(fds, 1, UART_POLL_TIMEOUT);
    if (ret == -1) {
        close(fd);
        printf("poll is error...\n");
        return -1;
    }

    if (fds[0].revents & POLLERR) {
        close(fd);
        printf("devfs poll file status is error.... !\n");
        return -1;
    } else if (fds[0].revents & POLLOUT) {
        printf("file is writable!!!\n");
        printf("# task: Start send buffer\n# when you see :  %s\n# It means send over\n\n\n", uart_data_buf_send);
        write(fd, uart_data_buf_send, sizeof(uart_data_buf_send));
    } else {
        printf("poll time out exit...\n");
    }

    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;
	while (1)
    {  
        ret = poll(fds, 1, UART_POLL_TIMEOUT);
        if (ret == -1) {
            close(fd);
            printf("poll is error...\n");
            return -1;
        }
        
        if (fds[0].revents & POLLERR) {
            close(fd);
            printf("devfs poll file status is error.... !\n");
            return -1;
        } else if (fds[0].revents & POLLIN) {
            printf(" file is readable!!!!\n");
            read(fd,poll_recv_buf,sizeof(str_recv)+1);
            poll_recv_buf[10] =  '\0';
            if (0 == memcmp(str_recv, poll_recv_buf, sizeof(str_recv))) {
                printf("devfs poll uart data received succeed... !\n");
            } else {
                printf("devfs poll uart test failed.... !\n");
                close(fd);
                return -1;
            }
        } else {
            if(poll_time++ == 2) {
                printf("poll time out exit...\n");
                break;
            }
            printf("%d seconds elapsed...\n", UART_POLL_TIMEOUT/1000);
        }
    }
    close(fd);
    return 0;
}
#endif