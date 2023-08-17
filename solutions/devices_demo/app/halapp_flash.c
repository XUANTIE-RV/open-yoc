/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/flash.h>
#include <devices/devicelist.h>

// Test address
#if defined(CONFIG_BOARD_CH2601_EVB)
#define FLASH_PROGRAM_ADDR (0x1810B000 - 0x18000000)
#elif defined(CONFIG_BOARD_D1_DOCKPRO_EVB)
#define FLASH_PROGRAM_ADDR 0xB42000
#elif defined(CONFIG_BOARD_F133_EVB)
#define FLASH_PROGRAM_ADDR 0xB42000
#elif defined(CONFIG_BOARD_BL606P_EVB)
#define FLASH_PROGRAM_ADDR 0x39C000
#elif defined(CONFIG_BOARD_CV181XH)
#define FLASH_PROGRAM_ADDR 0x517000
#else
#define FLASH_PROGRAM_ADDR 0xffffffff
#endif

int devfs_flash_demo(void);

int hal_flash_demo(void)
{
    char buf_write[32] = {0x1, 0x5, 0x1, 0x5};
    char buf_read[32] = {0};
    int ret = -1;

    printf("hal flash demo start\n");

    rvm_spiflash_drv_register(0);

    rvm_dev_t *flash_dev = NULL;
    char *filename = "flash0";

    flash_dev = rvm_hal_flash_open(filename);

    rvm_hal_flash_dev_info_t partition_info = {0};

    ret = rvm_hal_flash_get_info(flash_dev, &partition_info);
    if (ret != 0) {
        printf("rvm_hal_flash_get_info fail !\n");
        return -1;
    }

    ret = rvm_hal_flash_erase(flash_dev, FLASH_PROGRAM_ADDR, 1);
    if (ret != 0) {
        printf("rvm_hal_flash_erase fail !\n");
        return -1;
    }

    ret = rvm_hal_flash_program(flash_dev, FLASH_PROGRAM_ADDR, buf_write, sizeof(buf_write));
    if (ret != 0) {
        printf("rvm_hal_flash_program fail !\n");
        return -1;
    }

    ret = rvm_hal_flash_read(flash_dev, FLASH_PROGRAM_ADDR, buf_read, sizeof(buf_write));
    if (ret != 0) {
        printf("rvm_hal_flash_read fail !\n");
        return -1;
    }

    if (0 == memcmp(buf_write, buf_read, sizeof(buf_write))) {
        printf("flash write and read successfully !\n");
    } else {
        printf("flash operate failed !\n");
        return -1;
    }

    printf("flash demo successfully !\n");

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
    printf("---------------------------------------\n");
    return devfs_flash_demo();
#endif
    return 0;
}

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int devfs_flash_demo(void)
{
    char buf_write[32] = {0x1, 0x5, 0x1, 0x5};
    char buf_read[32] = {0};
    int ret = -1;

    printf("devfs flash demo start\n");

    rvm_spiflash_drv_register(0);

    char *flashdev = "/dev/flash0";
    int fd = open(flashdev, O_RDWR);
    printf("open flash0 fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", flashdev, fd);
        return -1;
    }
    rvm_hal_flash_dev_info_t flash_info;
    ret = ioctl(fd, FLASH_IOC_GET_INFO, &flash_info);
    if (ret < 0) {
        printf("FLASH_IOC_GET_INFO fail !\n");
        goto failure;
    }
    off_t offset = FLASH_PROGRAM_ADDR;
    int32_t sector_cnt = 1;
    ret = lseek(fd, offset, SEEK_SET);
    if (ret < 0) {
        printf("lseek fail !\n");
        goto failure;
    }
    ret = ioctl(fd, FLASH_IOC_ERASE, &sector_cnt);
    if (ret < 0) {
        printf("FLASH_IOC_ERASE fail !\n");
        goto failure;
    }
    ret = write(fd, buf_write, sizeof(buf_write));
    if (ret < 0) {
        printf("write fail !\n");
        goto failure;
    }
    ret = lseek(fd, offset, SEEK_SET);
    if (ret < 0) {
        printf("lseek fail !\n");
        goto failure;
    }
    ret = read(fd, buf_read, sizeof(buf_read));
    if (ret < 0) {
        printf("read fail !\n");
        goto failure;
    }
    if (0 == memcmp(buf_write, buf_read, sizeof(buf_write))) {
        printf("flash write and read successfully !\n");
    } else {
        printf("flash operate failed !\n");
        goto failure;
    }
    close(fd);
    printf("devfs flash demo successfully !\n");
    return 0;
failure:
    close(fd);
    return -1;
}

#endif