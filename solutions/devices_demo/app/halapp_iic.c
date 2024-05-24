/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "soc.h"
#include <drv/pin.h>
#include <aos/aos.h>
#include <drv/common.h>
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/iic.h>
#include <devices/devicelist.h>

#define TAG "HALAPP_IIC"

#define I2C_BUF_SIZE                8
#define I2C_TX_TIMEOUT              10000
#define I2C_RX_TIMEOUT              10000
#define I2C_TASK_SLEEP_TIME         10000
#define I2C_SLAVE_DEV_ADDR          0x50
#define I2C_MEM_DEV_ADDR            0x20

extern uint8_t is_iic_task;
extern const csi_pinmap_t iic_pinmap[];
static aos_sem_t slave_sem;
static aos_sem_t master_sem;
static rvm_dev_t *g_iic_dev;
static int g_slave_recv_ok;
static int g_master_send_ok;
static int g_iic_tasks_quit;

typedef struct 
{
  uint8_t gpio_pin[2];
  APP_TEST_IIC_FUNCTION function;
}iic_pin_info_t;

static uint32_t get_iic_pin_func(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
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

static uint8_t get_iic_idx(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin) {
            ret = (uint32_t)map->idx;
            break;
        }

        map++;
    }

    return ret;
}

static void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}

static int __iic_dev_init(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    int ret;
    int iic_idx;

    /* 管脚复用 */
#if !defined(CONFIG_CHIP_BL606P_E907)
    uint32_t pin0_func = get_iic_pin_func(gpio_pin0, iic_pinmap);
    uint32_t pin1_func = get_iic_pin_func(gpio_pin1, iic_pinmap);

    csi_pin_set_mux(gpio_pin0, pin0_func); // ch2601: PA2 PA3这一组不能用，无时序输出
    csi_pin_set_mux(gpio_pin1, pin1_func);
#else
    // 待BL完善 csi_pin_set_mux 接口后删除此 GLB_GPIO_Init 代码片段
    GLB_GPIO_Cfg_Type gpio_cfg;
    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioFun = GPIO_FUN_I2C0;

    gpio_cfg.gpioPin = gpio_pin0;
    GLB_GPIO_Init(&gpio_cfg);

    gpio_cfg.gpioPin = gpio_pin1;
    GLB_GPIO_Init(&gpio_cfg);
#endif
    /* 配置设备参数 */
    iic_idx = get_iic_idx(gpio_pin0, iic_pinmap);

    rvm_iic_drv_register(iic_idx);

    rvm_dev_t *iic_dev = NULL;
    char filename[8] = {0};
    sprintf(filename, "iic%d", iic_idx);

    iic_dev = rvm_hal_iic_open(filename);
    if (iic_dev == NULL) {
        LOGE(TAG, "rvm_hal_iic_open %s fail !", filename);
        return -1;
    }
    rvm_hal_iic_config_t config = {0};
    if ((function == APP_TEST_IIC_MASTER_SEND) || (function == APP_TEST_IIC_MASTER_RECV)
         || (function == APP_TEST_IIC_MEM_WRITE_READ)) {
        config.mode          = MODE_MASTER;
    } else {
        config.mode          = MODE_SLAVE;
        config.slave_addr    = I2C_SLAVE_DEV_ADDR;
    }
    config.speed     = BUS_SPEED_STANDARD;
    config.addr_mode = ADDR_7BIT;

    ret = rvm_hal_iic_config(iic_dev, &config);
    if (ret != 0) {
        LOGE(TAG, "rvm_hal_iic_config fail !");
        rvm_hal_iic_close(iic_dev);
        return -1;
    }
    g_iic_dev = iic_dev;
    return 0;
}

int hal_iic_demo(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    // i2c_dev_t i2c;
    uint8_t i2c_data_recv[I2C_BUF_SIZE], i2c_data_send[I2C_BUF_SIZE];
    bool master_check_recv = true;
    int ret = -1;

    LOGD(TAG, "hal_iic_demo start.");

    if (!is_iic_task) {
        ret = __iic_dev_init(function, gpio_pin0, gpio_pin1);
        if (ret != 0) {
            return ret;
        }
    }

    /* 初始化收发数据*/
    for (ret = 0; ret < I2C_BUF_SIZE; ret++) {
        i2c_data_send[ret] = ret + 1;
        i2c_data_recv[ret] = 0xff;
    }

    rvm_dev_t *iic_dev = g_iic_dev;
    switch (function)
    {
    case APP_TEST_IIC_MASTER_SEND:
        master_check_recv = false;
        LOGD(TAG, "start to master send.");
        ret = rvm_hal_iic_master_send(iic_dev,  I2C_SLAVE_DEV_ADDR, (uint8_t *)i2c_data_send, I2C_BUF_SIZE, I2C_TX_TIMEOUT);
        if (ret) {
            g_master_send_ok = 0;
            if (is_iic_task)
                aos_sem_signal(&master_sem);
            LOGE(TAG, "master send error !");
            goto fail;
        }
        LOGD(TAG, "master send ok.");
        dump_data(i2c_data_send, I2C_BUF_SIZE);
        g_master_send_ok = 1;
        if (is_iic_task) {
            aos_sem_signal(&master_sem);
        }
        break;
    case APP_TEST_IIC_SLAVE_RECV:
        LOGD(TAG, "slave recv...");
        ret = rvm_hal_iic_slave_recv(iic_dev, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_RX_TIMEOUT);
        if (ret) {
            g_slave_recv_ok = 0;
            LOGE(TAG, "slave recv error !");
            if (is_iic_task)
                aos_sem_signal(&slave_sem);
            goto fail;
        }
        LOGD(TAG, "slave recv ok.");
        dump_data(i2c_data_recv, I2C_BUF_SIZE);
        g_slave_recv_ok = 1;
        if (is_iic_task) {
            LOGD(TAG, "start to send data.");
            aos_sem_signal(&slave_sem);
        }
        break;
    case APP_TEST_IIC_MASTER_RECV:
        LOGD(TAG, "master recv..");
        if (is_iic_task) {
            LOGD(TAG, "master recv waiting...");
            aos_sem_wait(&master_sem, AOS_WAIT_FOREVER);
            aos_msleep(2000);
            if (g_master_send_ok == 0) {
                LOGE(TAG, "master recv abort.");
                ret = -1;
                goto fail;
            }
        }
        LOGD(TAG, "start to master recv.");
        if (rvm_hal_iic_master_recv(iic_dev, I2C_SLAVE_DEV_ADDR, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_RX_TIMEOUT)) {
            LOGE(TAG, "master recv error !");
            ret = -1;
            goto fail;
        }
        LOGD(TAG, "master recv ok.");
        dump_data(i2c_data_recv, I2C_BUF_SIZE);
        break;
    case APP_TEST_IIC_SLAVE_SEND:
        master_check_recv = false;
        if (is_iic_task) {
            LOGD(TAG, "slave send wating...");
            aos_sem_wait(&slave_sem, AOS_WAIT_FOREVER);
            if (g_slave_recv_ok == 0) {
                LOGE(TAG, "slave recv failed, send abort.");
                goto fail;
            }
        }
        LOGD(TAG, "start to slave send.");
        if (rvm_hal_iic_slave_send(iic_dev, (uint8_t *)i2c_data_send, I2C_BUF_SIZE, I2C_TX_TIMEOUT)) {
            LOGE(TAG, "slave send error !");
            ret = -1;
            goto fail;
        }
        LOGD(TAG, "slave send ok.");
        dump_data(i2c_data_send, I2C_BUF_SIZE);
        break;
#if 0
    case APP_TEST_IIC_MEM_WRITE_READ:
        aos_msleep(1000);
        if (rvm_hal_iic_mem_write(iic_dev, I2C_SLAVE_DEV_ADDR, I2C_MEM_DEV_ADDR, I2C_MEM_ADDR_SIZE_16BIT, (uint8_t *)i2c_data_send, I2C_BUF_SIZE, I2C_TX_TIMEOUT)) {
            LOGE(TAG, "mem write error !");
            goto fail;
        }
        aos_msleep(200);
        if (rvm_hal_iic_mem_read(iic_dev, I2C_SLAVE_DEV_ADDR, I2C_MEM_DEV_ADDR, I2C_MEM_ADDR_SIZE_16BIT, (uint8_t *)i2c_data_recv, I2C_BUF_SIZE, I2C_RX_TIMEOUT)) {
            LOGE(TAG, "mem read error !");
            goto fail;
        }
        break;
#endif
    default:
        break;
    }

    if (master_check_recv) {
        if (memcmp(i2c_data_send, i2c_data_recv, I2C_BUF_SIZE) != 0) {
            LOGE(TAG, "transfer error !");
        } else {
            LOGD(TAG, "transfer success !");
        }
    } else {
        LOGI(TAG, "go to the receiver to check the result !");
    }

fail:
    if (!is_iic_task) {
        /* 关闭IIC设备 */
        rvm_hal_iic_close(iic_dev);
    }
    LOGD(TAG, "hal_iic_demo quit.");
    return ret;
}

void iic_task(void *priv)
{
    LOGD(TAG, "iic_task start");
    iic_pin_info_t *pin_info = (iic_pin_info_t *)priv;
    hal_iic_demo(pin_info->function, pin_info->gpio_pin[0], pin_info->gpio_pin[1]);
    LOGD(TAG, "iic_task quit");
    g_iic_tasks_quit = 1;
}

void iic_task1(void *priv)
{
    LOGD(TAG, "iic_task1 start");
    iic_pin_info_t *pin_info = (iic_pin_info_t *)priv;
    hal_iic_demo(pin_info->function, pin_info->gpio_pin[0], pin_info->gpio_pin[1]);
    LOGD(TAG, "iic_task1 quit");
    g_iic_tasks_quit |= 1 << 1;
}

iic_pin_info_t task1_pin_info;
iic_pin_info_t task2_pin_info;
int hal_task_iic_demo(uint8_t is_master, uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    int ret;
    aos_task_t task;
    aos_sem_new(&slave_sem,0);
    aos_sem_new(&master_sem,0);

    task1_pin_info.gpio_pin[0] = gpio_pin0;
    task1_pin_info.gpio_pin[1] = gpio_pin1;
    task2_pin_info.gpio_pin[0] = gpio_pin0;
    task2_pin_info.gpio_pin[1] = gpio_pin1;
    if (is_master) {
        task1_pin_info.function = APP_TEST_IIC_MASTER_SEND;
        task2_pin_info.function = APP_TEST_IIC_MASTER_RECV;
    } else {
        task1_pin_info.function = APP_TEST_IIC_SLAVE_RECV;
        task2_pin_info.function = APP_TEST_IIC_SLAVE_SEND;
    }
    ret = __iic_dev_init(task1_pin_info.function, gpio_pin0, gpio_pin1);
    if (ret != 0) {
        return ret;
    }
    g_iic_tasks_quit = 0;
    aos_task_new_ext(&task, "iic_task", iic_task, (void *)&task1_pin_info, 4096, 32);
    aos_task_new_ext(&task, "iic_task1", iic_task1, (void *)&task2_pin_info, 4096, 32);

    while(g_iic_tasks_quit != 0x03) {
        aos_msleep(100);
    }
    LOGD(TAG, "hal_task_iic_demo finish....");
    rvm_hal_iic_close(g_iic_dev);
    return 0;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int g_devfs_master_send_ok;
static int g_devfs_slave_recv_ok;
static int g_devfs_iic_tasks_quit;
static int g_iic_fd;
static aos_sem_t devfs_slave_sem;
static aos_sem_t devfs_master_sem;
int devfs_iic_test_write_read(int fd, APP_TEST_IIC_FUNCTION function);

static int __devfs_iic_dev_init(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    // i2c_dev_t i2c;
    int ret = -1;
    uint8_t iic_idx = 0xff;
    LOGD(TAG, "devfs_iic_demo start.");

    /* 管脚复用 */
#if !defined(CONFIG_CHIP_BL606P_E907)
    uint32_t pin0_func = get_iic_pin_func(gpio_pin0, iic_pinmap);
    uint32_t pin1_func = get_iic_pin_func(gpio_pin1, iic_pinmap);

    csi_pin_set_mux(gpio_pin0, pin0_func); // ch2601: PA2 PA3这一组不能用，无时序输出
    csi_pin_set_mux(gpio_pin1, pin1_func);
#else
    // 待BL完善 csi_pin_set_mux 接口后删除此 GLB_GPIO_Init 代码片段
    GLB_GPIO_Cfg_Type gpio_cfg;
    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioFun = GPIO_FUN_I2C0;

    gpio_cfg.gpioPin = gpio_pin0;
    GLB_GPIO_Init(&gpio_cfg);

    gpio_cfg.gpioPin = gpio_pin1;
    GLB_GPIO_Init(&gpio_cfg);
#endif
    /* 配置设备参数 */
    iic_idx = get_iic_idx(gpio_pin0, iic_pinmap);

    rvm_iic_drv_register(iic_idx);

    char iicdev[16];
    sprintf(iicdev, "/dev/iic%d", iic_idx);

    int fd = open(iicdev, O_RDWR);
    LOGD(TAG, "open %s . fd:%d !",iicdev, fd);
    if (fd < 0) {
        LOGD(TAG, "open %s failed. fd:%d !",iicdev, fd);
        return -1;
    }

    rvm_hal_iic_config_t config = {0};
    if ((function == APP_TEST_IIC_MASTER_SEND) || (function == APP_TEST_IIC_MASTER_RECV)
         || (function == APP_TEST_IIC_MEM_WRITE_READ)) {
        config.mode          = MODE_MASTER;
    } else {
        config.mode          = MODE_SLAVE;
    }
    config.slave_addr    = I2C_SLAVE_DEV_ADDR;
    config.speed     = BUS_SPEED_STANDARD;
    config.addr_mode = ADDR_7BIT;
    ret = ioctl(fd, IIC_IOC_SET_CONFIG, &config);
    if (ret < 0) {
        LOGD(TAG, "IIC_IOC_SET_CONFIG fail !.");
        close(fd);
         return -1;
    }
    g_iic_fd = fd;
    return 0;   
}

int devfs_iic_demo(APP_TEST_IIC_FUNCTION function, uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    // i2c_dev_t i2c;
    int fd = -1;
    int ret = -1;
    bool master_check_recv = true;
    uint8_t i2c_data_recv[I2C_BUF_SIZE], i2c_data_send[I2C_BUF_SIZE];
    /* 初始化收发数据*/
    if (!is_iic_task) {
        ret = __devfs_iic_dev_init(function, gpio_pin0, gpio_pin1);
        if (ret != 0) {
            return ret;
        }
    }
    fd = g_iic_fd;
    for (ret = 0; ret < I2C_BUF_SIZE; ret++) {
        i2c_data_send[ret] = ret + 1;
        i2c_data_recv[ret] = 0xff;
    }
    
    rvm_iic_dev_msg_t msg_iic;
    switch (function) {
    case APP_TEST_IIC_MASTER_SEND:
        LOGD(TAG, "devfs start to master send.");
        aos_msleep(1000); //保证先调用IIC_IOC_SLAVE_RECV
        master_check_recv = false;
        msg_iic.dev_addr = I2C_SLAVE_DEV_ADDR;
        msg_iic.data = i2c_data_send;
        msg_iic.size = I2C_BUF_SIZE;
        msg_iic.timeout = I2C_TX_TIMEOUT;

        ret = ioctl(fd, IIC_IOC_MASTER_SEND, &msg_iic);
        if (ret) {
            g_devfs_master_send_ok = 0;
            if (is_iic_task)
                aos_sem_signal(&devfs_master_sem);
            LOGE(TAG, "devfs master send error !");
            goto failure;
        }
        LOGD(TAG, "devfs master send ok.");
        dump_data(i2c_data_send, I2C_BUF_SIZE);
        g_devfs_master_send_ok = 1;
        if (is_iic_task) {
            aos_sem_signal(&devfs_master_sem);
        }
        break;
    case APP_TEST_IIC_SLAVE_RECV:
        LOGD(TAG, "devfs_slave recv...");
        msg_iic.data = i2c_data_recv;
        msg_iic.size = I2C_BUF_SIZE;
        msg_iic.timeout = I2C_RX_TIMEOUT;

        ret = ioctl(fd, IIC_IOC_SLAVE_RECV, &msg_iic);
        if (ret) {
            g_devfs_slave_recv_ok = 0;
            LOGE(TAG, "devfs slave recv error !");
            if (is_iic_task)
                aos_sem_signal(&devfs_slave_sem);
            goto failure;
        }
        LOGD(TAG, "devfs slave recv ok.");
        dump_data(i2c_data_recv, I2C_BUF_SIZE);
        g_devfs_slave_recv_ok = 1;
        if (is_iic_task) {
            LOGD(TAG, "devfs start to send data.");
            aos_sem_signal(&devfs_slave_sem);
        }
        break;
    case APP_TEST_IIC_MASTER_RECV:
        aos_msleep(1000); //保证先调用IIC_IOC_SLAVE_RECV
        master_check_recv = true;
        LOGD(TAG, "devfs master recv..");
        if (is_iic_task) {
            LOGD(TAG, "devfs master recv waiting...");
            aos_sem_wait(&devfs_master_sem, AOS_WAIT_FOREVER);
            aos_msleep(2000);
            if (g_devfs_master_send_ok == 0) {
                LOGE(TAG, "devfs master recv abort.");
                ret = -1;
                goto failure;
            }
        }
        LOGD(TAG, "devfs start to master recv.");
        msg_iic.dev_addr = I2C_SLAVE_DEV_ADDR;
        msg_iic.data = i2c_data_recv;
        msg_iic.size = I2C_BUF_SIZE;
        msg_iic.timeout = I2C_RX_TIMEOUT;

        ret = ioctl(fd, IIC_IOC_MASTER_RECV, &msg_iic);
        if (ret < 0) {
            LOGE(TAG, "devfs master recv error !");
            ret = -1;
            goto failure;
        }
        LOGD(TAG, "devfs master recv ok.");
        dump_data(i2c_data_recv, I2C_BUF_SIZE);
        break;
    case APP_TEST_IIC_SLAVE_SEND:
        master_check_recv = false;
        if (is_iic_task) {
            LOGD(TAG, "devfs slave send wating...");
            aos_sem_wait(&devfs_slave_sem, AOS_WAIT_FOREVER);
            if (g_devfs_slave_recv_ok == 0) {
                LOGE(TAG, "devfs slave recv failed, send abort.");
                goto failure;
            }
        }
        LOGD(TAG, "devfs start to slave send.");
        msg_iic.data = i2c_data_send;
        msg_iic.size = I2C_BUF_SIZE;
        msg_iic.timeout = I2C_TX_TIMEOUT;

        ret = ioctl(fd, IIC_IOC_SLAVE_SEND, &msg_iic);
        if (ret < 0) {
            LOGD(TAG, "slave end.");
            LOGE(TAG, "devfs slave send error !");
            ret = -1;
            goto failure;
        }
        LOGD(TAG, "devfs slave send ok.");
        dump_data(i2c_data_send, I2C_BUF_SIZE);
        break;
#if 0
    case APP_TEST_IIC_MEM_WRITE_READ:
        aos_msleep(1000);
        msg_iic.dev_addr = I2C_SLAVE_DEV_ADDR;
        msg_iic.mem_addr = I2C_MEM_DEV_ADDR;
        msg_iic.mem_addr_size = I2C_MEM_ADDR_SIZE_16BIT;
        msg_iic.data = i2c_data_send;
        msg_iic.size = I2C_BUF_SIZE;
        msg_iic.timeout = I2C_TX_TIMEOUT;

        ret = ioctl(fd, IIC_IOC_MEM_WRITE, &msg_iic);
        if (ret < 0) {
            printf("IIC_IOC_MEM_WRITE fail !\n");
            goto failure;
        }
        aos_msleep(200);

        msg_iic.dev_addr = I2C_SLAVE_DEV_ADDR;
        msg_iic.mem_addr = I2C_MEM_DEV_ADDR;
        msg_iic.mem_addr_size = I2C_MEM_ADDR_SIZE_16BIT;
        msg_iic.data = i2c_data_recv;
        msg_iic.size = I2C_BUF_SIZE;
        msg_iic.timeout = I2C_RX_TIMEOUT;

        ret = ioctl(fd, IIC_IOC_MEM_READ, &msg_iic);
        if (ret < 0) {
            printf("IIC_IOC_MEM_READ fail !\n");
            goto failure;
        }
        break;
#endif
    default:
        break;
    }

    if (master_check_recv) {
        if (memcmp(i2c_data_send, i2c_data_recv, I2C_BUF_SIZE) != 0) {
            LOGE(TAG, "transfer error !");
        } else {
            LOGD(TAG, "transfer success !");
        }
    } else {
        LOGI(TAG, "go to the receiver to check the result !");
    }

    if (!is_iic_task) {
        LOGD(TAG, "tdevfs write read api start !");
        ret = devfs_iic_test_write_read(fd, function);
        if (!ret) {
            if (function == APP_TEST_IIC_MASTER_SEND) {
                LOGI(TAG, "devfs iic write  api successful\n");
            } else if (function == APP_TEST_IIC_MASTER_RECV){
                LOGI(TAG, "devfs iic read api successful\n");
            } else {
               LOGI(TAG, "devfs iic slave api successful\n");
            }
        } else {
            LOGI(TAG, "devfs iic write read api fail\n");
            goto failure;
        }
        close(fd);
    }
    return 0;
failure:
    if (!is_iic_task) {
        /* 关闭IIC设备 */
        close(fd);
    }
    return -1;
}

int devfs_iic_test_write_read(int fd, APP_TEST_IIC_FUNCTION function)
{
    int ret = -1;
    rvm_iic_dev_msg_t msg_iic;
    uint8_t i2c_data_recv[I2C_BUF_SIZE], i2c_data_send[I2C_BUF_SIZE];
    /* 初始化收发数据*/
    for (ret = 0; ret < I2C_BUF_SIZE; ret++) {
        i2c_data_send[ret] = ret + 1;
        i2c_data_recv[ret] = 0xff;
    }
    uint32_t timeout = 10000;
    uint32_t is_iic_master = ((function == APP_TEST_IIC_MASTER_SEND) || (function == APP_TEST_IIC_MASTER_RECV)) ? true:false;
    if (is_iic_master)
    {
       aos_msleep(2000); 
        ret = ioctl(fd, IIC_IOC_TIMEOUT, &timeout);
        if (ret) {
            LOGE(TAG, "IIC_IOC_TIMEOUT error !");
            goto failure;
        }
        if (function == APP_TEST_IIC_MASTER_SEND) {
            ret = write(fd, i2c_data_send, I2C_BUF_SIZE);
            if(ret < 0) {
                LOGE(TAG, "iic write error !");
                goto failure;
            }
            dump_data(i2c_data_send, I2C_BUF_SIZE);
            LOGI(TAG, "go to the receiver to check the result !");
            LOGI(TAG, "iic write successfully!");
        }
        if (function == APP_TEST_IIC_MASTER_RECV) {
            ret = read(fd, i2c_data_recv, I2C_BUF_SIZE);
            dump_data(i2c_data_recv, I2C_BUF_SIZE);
            if (ret < 0) {
                LOGE(TAG, "iic read error !");
                goto failure;
            }
            if (memcmp(i2c_data_send, i2c_data_recv, I2C_BUF_SIZE) != 0) {
                ret = -1;
                LOGE(TAG, "iic read transfer error !");
                goto failure;
            } else {
                LOGD(TAG, "iic read  success !");
            }
        }
    } else {
        msg_iic.data = i2c_data_recv;
        msg_iic.size = I2C_BUF_SIZE;
        msg_iic.timeout = I2C_RX_TIMEOUT;
        aos_msleep(1000);
        if (function == APP_TEST_IIC_SLAVE_RECV) {
            ret = ioctl(fd, IIC_IOC_SLAVE_RECV, &msg_iic);
            dump_data(i2c_data_recv, I2C_BUF_SIZE);

            if (memcmp(i2c_data_send, i2c_data_recv, I2C_BUF_SIZE) != 0) {
                ret = -1;
                LOGE(TAG, "iic read transfer error !");
                goto failure;
            } else {
                LOGD(TAG, "iic read transfer success !");
            }
        }
        if (function == APP_TEST_IIC_SLAVE_SEND) {
            msg_iic.data = i2c_data_send;
            msg_iic.size = I2C_BUF_SIZE;
            msg_iic.timeout = I2C_TX_TIMEOUT;
            ret = ioctl(fd, IIC_IOC_SLAVE_SEND, &msg_iic);
            if (ret < 0) {
                LOGE(TAG, "slave end !");
                LOGE(TAG, "devfs slave send error !");
                ret = -1;
                goto failure;
            }
            LOGD(TAG, "devfs slave send ok.");  
            LOGI(TAG, "go to the receiver to check the result !");
        }
    }
    return 0;
failure:
    return ret;
}

void devfs_iic_task(void *priv)
{
    LOGD(TAG, "devfs_iic_task start");
    iic_pin_info_t *pin_info = (iic_pin_info_t *)priv;
    devfs_iic_demo(pin_info->function, pin_info->gpio_pin[0], pin_info->gpio_pin[1]);
    LOGD(TAG, "devfs_iic_task quit");
    g_devfs_iic_tasks_quit = 1;
}

void devfs_iic_task1(void *priv)
{
    LOGD(TAG, "devfs_iic_task1 start");
    iic_pin_info_t *pin_info = (iic_pin_info_t *)priv;
    devfs_iic_demo(pin_info->function, pin_info->gpio_pin[0], pin_info->gpio_pin[1]);
    LOGD(TAG, "devfs_iic_task1 quit");
    g_devfs_iic_tasks_quit |= 1 << 1;
}


iic_pin_info_t devfs_task1_pin_info;
iic_pin_info_t devfs_task2_pin_info;
int devfs_task_iic_demo(uint8_t is_master, uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    int ret;
    aos_task_t task;
    aos_sem_new(&devfs_slave_sem,0);
    aos_sem_new(&devfs_master_sem,0);

    devfs_task1_pin_info.gpio_pin[0] = gpio_pin0;
    devfs_task1_pin_info.gpio_pin[1] = gpio_pin1;
    devfs_task2_pin_info.gpio_pin[0] = gpio_pin0;
    devfs_task2_pin_info.gpio_pin[1] = gpio_pin1;
    if (is_master) {
        devfs_task1_pin_info.function = APP_TEST_IIC_MASTER_SEND;
        devfs_task2_pin_info.function = APP_TEST_IIC_MASTER_RECV;
    } else {
        devfs_task1_pin_info.function = APP_TEST_IIC_SLAVE_RECV;
        devfs_task2_pin_info.function = APP_TEST_IIC_SLAVE_SEND;
    }
    ret = __devfs_iic_dev_init(devfs_task1_pin_info.function, gpio_pin0, gpio_pin1);
    if (ret != 0) {
        return ret;
    }
    g_devfs_iic_tasks_quit = 0;
    aos_task_new_ext(&task, "devfs_iic_task", devfs_iic_task, (void *)&devfs_task1_pin_info, 4096, 32);
    aos_task_new_ext(&task, "devfs_iic_task1", devfs_iic_task1, (void *)&devfs_task2_pin_info, 4096, 32);

    while(g_devfs_iic_tasks_quit != 0x03) {
        aos_msleep(100);
    }
    LOGD(TAG, "devfs_task_iic_demo finish....");
    close(g_iic_fd);
    return 0;
}
#endif