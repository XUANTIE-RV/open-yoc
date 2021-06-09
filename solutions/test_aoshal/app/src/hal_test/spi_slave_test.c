/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>

#include <aos/hal/spi.h>
#include <drv/gpio.h>
#include <soc.h>
#include <ulog/ulog.h>
#include <yoc/atserver.h>

#define SPI1_PORT_NUM  2
#define SPI_BUF_SIZE   5
#define SPI_BUF_SIZE2   1
#define SPI_TX_TIMEOUT 10000
#define SPI_RX_TIMEOUT 10000

#define TAG "TEST_HAL_SPI"

#define AT_BACK_READY()                       atserver_send("\r\nREADY\r\n")

/* define dev */
spi_dev_t spi1;

static void aos_hal_spi_slave_send(uint16_t size)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret3     = -1;
    int i       = 0;
    int n        = 0;
    char spi_data_buf[size];
    spi1.config.freq = 100000;

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }


    for (i = 0; i < size; i++) {
        spi_data_buf[i] = i + 3;
    }

    while(n<10) {
        ret2 = hal_spi_send(&spi1, spi_data_buf, size, SPI_TX_TIMEOUT);
        // aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("slave send is %d\n",spi_data_buf[i]);
        }
        if (ret2 == 0) {
            printf("spi1 data send succeed !\n");
        }
        n++;
    };


    ret3 = hal_spi_finalize(&spi1);
    if (ret3 != 0) {
        printf("spi1 finalize error !\n");
    }

    if (ret1 == 0 && ret2 == 0 && ret3 == 0) {
        AT_BACK_OK();
    }
    else{
        AT_BACK_ERR();
    }
}

// static void aos_hal_spi_slave_recv(uint16_t size)
// {
//     int ret1     = -1;
//     int ret2     = -1;
//     int ret2_result     = 0;
//     int ret3     = -1;
//     int n        = 0;
//     char spi_data_buf1[SPI_BUF_SIZE]={0};
//     spi1.config.freq = frequency;

//     ret1 = hal_spi_init(&spi1);
//     if (ret1 != 0) {
//         printf("spi1 init error !\n");
//     }

//     while(n<10) {
//         ret2 = hal_spi_recv(&spi1, spi_data_buf1, size, SPI_RX_TIMEOUT);
//         // aos_msleep(50);
//         if(size == 2 || size == 10){
//             for (int i = 0; i < size/2; i++)
//             {
//                 printf("slave recv is %d\n",spi_data_buf1[i]);
//                 if(spi_data_buf1[i] != i + 2){
//                     ret2_result += 1;
//                 }
//             }
//             for (int i = 0; i < size/2; i++)
//             {
//                 printf("slave recv is %d\n",spi_data_buf1[i]);
//                 if(spi_data_buf1[i] != i + 2){
//                     ret2_result += 1;
//                 }
//             }
//         }else{
//             for (int i = 0; i < size; i++)
//             {
//                 printf("slave recv is %d\n",spi_data_buf1[i]);
//                 if(spi_data_buf1[i] != i + 2){
//                     ret2_result += 1;
//                 }
//             }
//         }
        
//         if (ret2 == 0) {
//             printf("spi1 data recv succeed !\n");
//         }
//         else
//         {
//             printf("spi1 data recv error !\n");
//         }
//         n++;
//     };

//     ret3 = hal_spi_finalize(&spi1);
//     if (ret3 != 0) {
//         printf("spi1 finalize error !\n");
//     }

//     if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
//         AT_BACK_OK();
//     }
//     else{
//         AT_BACK_ERR();
//     }
// }

static void aos_hal_spi_slave_recv(uint16_t size, uint32_t frequency, uint32_t rx_time)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret2_result     = 0;
    int ret3     = -1;
    int n        = 0;
    char spi_data_buf1[SPI_BUF_SIZE]={0};
    spi1.config.freq = frequency;

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }

    while(n<10) {
        ret2 = hal_spi_recv(&spi1, spi_data_buf1, size, rx_time);
        // aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("slave recv is %d\n",spi_data_buf1[i]);
            if(spi_data_buf1[i] != i + 2){
                ret2_result += 1;
            }
        }
        
        if (ret2 == 0) {
            printf("spi1 data recv succeed !\n");
        }
        else
        {
            printf("spi1 data recv error !\n");
        }
        n++;
    };

    ret3 = hal_spi_finalize(&spi1);
    if (ret3 != 0) {
        printf("spi1 finalize error !\n");
    }

    if(rx_time == 0){
        if (ret2_result != 0) {
            AT_BACK_OK();
            return;
        }
        else{
            AT_BACK_ERR();
            return;
        }
    }

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
        AT_BACK_OK();
    }
    else{
        AT_BACK_ERR();
    }
}

static void aos_hal_spi_slave_send_recv(uint16_t size)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret2_result     = 0;
    int ret3     = -1;
    int n        = 0;
    int i       = 0;
    char spi_data_buf[size];
    char spi_data_buf1[SPI_BUF_SIZE]={0};
    spi1.config.freq = 100000;

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }

    for (i = 0; i < size; i++) {
        spi_data_buf[i] = i + 3;
    }
    while(n<10) {
        ret2 = hal_spi_send_recv(&spi1,spi_data_buf,spi_data_buf1,size, SPI_RX_TIMEOUT);
        // aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("slave send is %d\n",spi_data_buf[i]);
            printf("slave recv is %d\n",spi_data_buf1[i]);
            if(spi_data_buf1[i] != i + 2){
                ret2_result += 1;
            }
        }
        
        if (ret2 == 0) {
            printf("spi1 data recv succeed !\n");
        }
        else
        {
            printf("spi1 data recv error !\n");
        }
        n++;
    };

    ret3 = hal_spi_finalize(&spi1);
    if (ret3 != 0) {
        printf("spi1 finalize error !\n");
    }

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0) {
        AT_BACK_OK();
    }
    else{
        AT_BACK_ERR();
    }
}

// static void aos_hal_spi_slave_recv_and_send(uint16_t size, uint32_t frequency, uint32_t tx_time, uint32_t rx_time)
// {
//     int ret1     = -1;
//     int ret2     = -1;
//     int ret2_result     = 0;
//     int ret3     = -1;
//     int ret4     = -1;
//     int n        = 0;
//     int i       = 0;
//     char spi_data_buf[size*2];
//     char spi_data_buf1[SPI_BUF_SIZE*2]={0};
//     spi1.config.freq = frequency;

//     ret1 = hal_spi_init(&spi1);
//     if (ret1 != 0) {
//         printf("spi1 init error !\n");
//     }

//     for (i = 0; i < size; i++) {
//         spi_data_buf[i] = i + 3;
//     }
//     for (i = size; i < size*2; i++) {
//         spi_data_buf[i] = i + 3 - size;
//     }
//     while(n<10) {
//         ret2 = hal_spi_recv(&spi1, spi_data_buf1, size *2, SPI_RX_TIMEOUT);
//         for (int i = 0; i < size; i++)
//         {
//             printf("slave recv is %d\n",spi_data_buf1[i]);
//             if(spi_data_buf1[i] != i + 2){
//                 ret2_result += 1;
//             }
//         }
//         if (ret2 == 0) {
//             printf("spi1 data recv succeed !\n");
//         }else
//         {
//             printf("spi1 data recv error !\n");
//         }
//         n++;
//     };
//     n = 0;

//     while(n<10) {
//         ret3 = hal_spi_send(&spi1, spi_data_buf, size *2, SPI_TX_TIMEOUT);
//         for (int i = 0; i < size*2; i++)
//         {
//             printf("slave send is %d\n",spi_data_buf[i]);
//         }
//         if (ret3 == 0) {
//             printf("spi1 data send succeed !\n");
//         }else
//         {
//             printf("spi1 data send error !\n");
//         }
//         n++;
//     };

//     ret4 = hal_spi_finalize(&spi1);
//     if (ret4 != 0) {
//         printf("spi1 finalize error !\n");
//     }

//     if (ret1 == 0 && ret2 == 0 && ret2_result == 0 && ret3 == 0 && ret4 == 0) {
//         AT_BACK_OK();
//     }
//     else{
//         AT_BACK_ERR();
//     }
// }

// static void aos_hal_spi_slave_timeout(int time)
// {
//     int ret     = -1;
//     int ret_result     = 0;
//     int i       = 0;
//     int n        = 0;
//     char spi_data_buf1[SPI_BUF_SIZE]={0};

//     spi1.config.freq = 100000;

//     hal_spi_init(&spi1);
    
//     while(n<10) {
//         ret = hal_spi_recv(&spi1, spi_data_buf1, SPI_BUF_SIZE, time);
//         // aos_msleep(50);
//         for (int i = 0; i < SPI_BUF_SIZE; i++)
//         {
//             printf("slave recv is %d\n",spi_data_buf1[i]);
//             if(spi_data_buf1[i] != i + 2){
//                 ret_result += 1;
//             }
//         }
        
//         if (ret == 0) {
//             printf("spi1 data recv succeed !\n");
//         }
//         else
//         {
//             printf("spi1 data recv error !\n");
//         }
//         n++;
//     };

//     hal_spi_finalize(&spi1);

//     if(time == HAL_WAIT_FOREVER){
//         if (ret == 0 && ret_result == 0) {
//             AT_BACK_OK();
//         }
//         else{
//             AT_BACK_ERR();
//         }
//     }else if(time == 0){
//         if (ret_result != 0) {
//             AT_BACK_OK();
//         }
//         else{
//             AT_BACK_ERR();
//         }
//     }
// }

void test_hal_spi_slave(char *cmd, int type, char *data)
{
    csi_pin_set_mux(PB28,1);
    csi_pin_set_mux(PB29,1);
    csi_pin_set_mux(PB30,1);
    csi_pin_set_mux(PB31,1);

    spi1.port = SPI1_PORT_NUM;
    spi1.config.mode  = HAL_SPI_MODE_SLAVE;

    if (strcmp((const char *)data, "'send_multi'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_send(SPI_BUF_SIZE);
    }else if (strcmp((const char *)data, "'send_single'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_send(SPI_BUF_SIZE2);
    }else if (strcmp((const char *)data, "'recv_multi'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_recv(SPI_BUF_SIZE, 100000, SPI_RX_TIMEOUT);
    }else if (strcmp((const char *)data, "'recv_single'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_recv(SPI_BUF_SIZE2, 100000, SPI_RX_TIMEOUT);
    }else if (strcmp((const char *)data, "'multi_send_recv'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_send_recv(SPI_BUF_SIZE);
    }else if (strcmp((const char *)data, "'single_send_recv'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_send_recv(SPI_BUF_SIZE2);
    }
    // else if (strcmp((const char *)data, "'multi_recv_and_send'\0") == 0) {
    //     AT_BACK_READY();
    //     aos_hal_spi_slave_recv_and_send(SPI_BUF_SIZE, 100000, SPI_TX_TIMEOUT, SPI_RX_TIMEOUT);
    // }else if (strcmp((const char *)data, "'single_recv_and_send'\0") == 0) {
    //     AT_BACK_READY();
    //     aos_hal_spi_slave_recv_and_send(SPI_BUF_SIZE2, 100000, SPI_TX_TIMEOUT, SPI_RX_TIMEOUT);
    // }else if (strcmp((const char *)data, "'multi_recv_and_recv'\0") == 0) {
    //     AT_BACK_READY();
    //     aos_hal_spi_slave_recv(10);
    // }else if (strcmp((const char *)data, "'single_recv_and_recv'\0") == 0) {
    //     AT_BACK_READY();
    //     aos_hal_spi_slave_recv(2);
    // }
    else if (strcmp((const char *)data, "'freq_100000'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_recv(SPI_BUF_SIZE, 100000, SPI_RX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_1M'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_recv(SPI_BUF_SIZE, 1000000, SPI_RX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_10M'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_recv(SPI_BUF_SIZE, 10000000, SPI_RX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_3M'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_recv(SPI_BUF_SIZE, 3000000, SPI_RX_TIMEOUT);
    }else if (strcmp((const char *)data, "'timeout_HAL_WAIT_FOREVER'\0") == 0) {
        AT_BACK_READY();
        aos_hal_spi_slave_recv(SPI_BUF_SIZE, 100000, HAL_WAIT_FOREVER);
    }
    else if (strcmp((const char *)data, "'timeout_0'\0") == 0) {
        aos_hal_spi_slave_recv(SPI_BUF_SIZE, 100000, 0);
    }
}