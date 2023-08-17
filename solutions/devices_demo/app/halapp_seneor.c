/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/sensor.h>
#include <devices/devicelist.h>

int hal_sensor_demo(void)
{
    const char *name = "sensor";
    rvm_dev_t *sensor_dev = NULL;
    rvm_hal_sensor_sht20_t sval;
    int ret = -1;

    /* 注册传感器设备驱动 */
    // TODO: 驱动待实现

    /* 打开设备 */
    sensor_dev = rvm_hal_sensor_open(name);
    if (sensor_dev == NULL) {
        printf("rvm_hal_sensor_open err\n");
        return -1;
    }

    /* 读取数据 */
    ret = rvm_hal_sensor_fetch(sensor_dev);
    if (ret < 0) {
        printf("rvm_hal_sensor_fetch err\n");
        return -1;        
    }

    ret = rvm_hal_sensor_getvalue(sensor_dev, (void *)&sval, sizeof(rvm_hal_sensor_sht20_t));
    if (ret < 0) {
        printf("rvm_hal_sensor_fetch err\n");
        return -1;        
    }

    /* 关闭驱动 */
    rvm_hal_sensor_close(sensor_dev);

    printf("hal_sensor_demo successfully!\n");
  
    return 0;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int devfs_sensor_demo(void)
{
    char *sensordev = "/dev/sensor";
    int ret = -1;

    /* 注册传感器设备驱动 */
    // TODO: 驱动待实现

    /* 打开设备 */
    int fd = open(sensordev, O_RDWR);
    printf("open sensor fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", sensordev, fd);
        return -1;
    }

    /* 读取数据 */
    ret = ioctl(fd, SENSOR_IOC_FETCH);
    if (ret < 0) {
        printf("SENSOR_IOC_FETCH fail !\n");
        goto failure;
    }

    rvm_sensor_dev_msg_t msg;
    msg.size = sizeof(rvm_hal_sensor_sht20_t);

    ret = ioctl(fd, SENSOR_IOC_GETVALUE, &msg);
    if (ret < 0) {
        printf("SENSOR_IOC_GETVALUE fail !\n");
        goto failure;
    }

    printf("hal_sensor_demo successfully!\n");

    /* 关闭驱动 */
    close(fd);
    return 0;
failure:
    close(fd);
    return -1;
}

#endif