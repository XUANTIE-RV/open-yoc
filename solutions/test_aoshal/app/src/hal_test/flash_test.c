/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>

#include <drv/gpio.h>
#include <soc.h>
#include "app_main.h"
#include <aos/hal/flash.h>
#include <ulog/ulog.h>
#include <yoc/atserver.h>

#define TAG "TEST_HAL_EFLASH"

//新建分区，定义owner/起始地址/大小
const hal_logic_partition_t hal_partitions[] =
{
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner      = HAL_FLASH_SPI,
        .partition_description  = "yoc7.4",
        .partition_start_addr  = 0x8A48000,
        .partition_length     = 0x001000, //4096
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

static void aos_hal_flash_info_get(){
    int32_t ret = -1;
    hal_logic_partition_t partition_info = {0};

    ret = hal_flash_info_get(HAL_PARTITION_APPLICATION,&partition_info);
    switch (partition_info.partition_owner){
        case 0:
            printf("partition ower is: HAL_FLASH_EMBEDDED\n");
            break;
        case 1:
            printf("partition ower is: HAL_FLASH_SPI\n");
            break;
        case 2:
            printf("partition ower is: HAL_FLASH_QSPI\n");
            break;
        case 3:
            printf("partition ower is: HAL_FLASH_MAX\n");
        case 4:
            printf("partition ower is: HAL_FLASH_NONE\n");
            break;
        default:
            break;
    }
    printf("partitiondescription is: %s\n",partition_info.partition_description);
    printf("partition start addr is: %x\n",partition_info.partition_start_addr);
    printf("partition length is: %d\n",partition_info.partition_length);
    printf("partition op is: PAR_OPT_READ_EN & PAR_OPT_WRITE_EN\n");

    if (ret == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_erase_write(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    uint32_t off = 0;
    char buf[4096] = {0};

    memset(buf,0,4096);
    ret1 = hal_flash_erase_write(HAL_PARTITION_APPLICATION,&off,buf,4096);
    memset(buf,0,4096);
    ret2 = hal_flash_erase_write(HAL_PARTITION_APPLICATION,&off,buf,0);
    memset(buf,0,2048);
    ret3 = hal_flash_erase_write(HAL_PARTITION_APPLICATION,&off,buf,2048);

    if (ret1 == 0 && ret2 == 0 && ret3 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_erase(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;
    
    //偏移量为0，擦除字节数为0
    ret1 = hal_flash_erase(HAL_PARTITION_APPLICATION,0,0);

    //偏移量为0，擦除字节数为最大值
    ret2 = hal_flash_erase(HAL_PARTITION_APPLICATION,0,4096);

    //偏移量为23，擦除字节数为26
    ret3 = hal_flash_erase(HAL_PARTITION_APPLICATION,23,2048);

    //偏移量为最大值，擦除字节数为0
    ret4 = hal_flash_erase(HAL_PARTITION_APPLICATION,4095,0);

    if (ret1 == 0 && ret2 == 0 && ret3 == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_write(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    char     tx_buf[4096] = {0};
    uint32_t off = 0;
    memset(tx_buf, 0xaa, 4096);

    ret1 = hal_flash_write(HAL_PARTITION_APPLICATION,&off,tx_buf,4096);
    
    ret2 = hal_flash_write(HAL_PARTITION_APPLICATION,&off,tx_buf,512);

    ret3 = hal_flash_write(HAL_PARTITION_APPLICATION,&off,tx_buf,0);

    if (ret1 == 0 && ret2 == 0 && ret3 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_read(){
    int32_t ret = -1;
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;
    int i;
    char     tx_buf[100] = {0};
    char     rx_buf[100] = {0};
    uint32_t off = 0;
    printf("start test\n");

    ret = hal_flash_erase(HAL_PARTITION_APPLICATION,0,4096);
    if (ret != 0) {
        printf("flash erase error !\n");
        AT_BACK_ERR();
        return;
    }

    memset(tx_buf, 10, 100);
    ret = hal_flash_write(HAL_PARTITION_APPLICATION,&off,tx_buf,100);
    if (ret != 0) {
        printf("flash write error !\n");
        AT_BACK_ERR();
        return;
    }
    //写入之后，off变更，需重新置0
    off = 0;
    ret1 = hal_flash_read(HAL_PARTITION_APPLICATION,&off,rx_buf,100);
    for(i=0;i<100;i++){
        printf("tx_buf[%d] =%d,rx_buf[%d] =%d\n",i,tx_buf[i],i,rx_buf[i]);
    }
    if(memcmp(tx_buf, rx_buf, 100) == 0){
        ret2 = 0;
        LOGD(TAG,"data compare success!");
    } else {
        LOGD(TAG,"read data not equal write data!");
    }

    memset(tx_buf, 11, 100);
    ret = hal_flash_erase(HAL_PARTITION_APPLICATION,0,4096);
    if (ret != 0) {
        printf("flash erase error !\n");
        AT_BACK_ERR();
        return;
    }
    hal_flash_erase_write(HAL_PARTITION_APPLICATION,&off,tx_buf,100);
    off = 0;
    ret3 = hal_flash_read(HAL_PARTITION_APPLICATION,&off,rx_buf,100);
    for(i=0;i<100;i++){
        printf("tx_buf[%d] =%d,rx_buf[%d] =%d\n",i,tx_buf[i],i,rx_buf[i]);
    }
    if(memcmp(tx_buf, rx_buf, 100) == 0){
        ret4 = 0;
        LOGD(TAG,"data compare success!");
    } else {
        LOGD(TAG,"read data not equal write data!");
    }

    if (ret1 == 0 && ret2 == 0 && ret3 == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_flash_erase_write_error(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;
    hal_logic_partition_t partition_info = {0};
    uint32_t off = 0;
    char buf1[4097] = {0};

    ret1 = hal_flash_erase_write(HAL_PARTITION_APPLICATION,&off,buf1,4096+2);
    //擦除字节数超过最大
    ret2 = hal_flash_erase(HAL_PARTITION_APPLICATION,0,4096 + 1);
    //偏移量超过最大
    ret3 = hal_flash_erase(HAL_PARTITION_APPLICATION,4096,5);

    //写入数据为NULL
    ret4 = hal_flash_write(HAL_PARTITION_APPLICATION,&off,NULL,512);

    if (ret1 != 0 && ret2 != 0 && ret3 != 0 && ret4 != 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

void test_hal_spiflash(char *cmd, int type, char *data)
{
    hal_logic_partition_t partition_info = {0};

    hal_flash_info_get(HAL_PARTITION_APPLICATION,&partition_info);
    if (strcmp((const char *)data, "'info_get'\0") == 0) {
        aos_hal_flash_info_get();
    }else if (strcmp((const char *)data, "'erase_write'\0") == 0) {
        aos_hal_flash_erase_write();
    }else if (strcmp((const char *)data, "'toerase'\0") == 0) {
        aos_hal_flash_erase();
    }else if (strcmp((const char *)data, "'towrite'\0") == 0) {
        aos_hal_flash_write();
    }else if (strcmp((const char *)data, "'toread'\0") == 0) {
        aos_hal_flash_read();
    }else if (strcmp((const char *)data, "'error'\0") == 0) {
        aos_hal_flash_erase_write_error();
    }
}