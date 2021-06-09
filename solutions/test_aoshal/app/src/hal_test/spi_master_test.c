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

/* define dev */
spi_dev_t spi1;
csi_gpio_t gpio;

static void aos_hal_spi_master_send(uint16_t size, uint32_t frequency, uint32_t time)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret3     = -1;
    int i       = 0;
    int n        = 0;
    char spi_data_buf[size];
    spi1.config.freq = frequency;
    csi_pin_set_mux(PB31,1);

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }


    for (i = 0; i < size; i++) {
        spi_data_buf[i] = i + 2;
    }


    while(n<10) {
        ret2 = hal_spi_send(&spi1, spi_data_buf, size, time);
        aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("master send is %d\n",spi_data_buf[i]);
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

static void aos_hal_spi_master_recv(uint16_t size)
{
    int ret1     = -1;
    int ret2     = -1;
    int ret2_result     = 0;
    int ret3     = -1;
    int n        = 0;
    char spi_data_buf1[SPI_BUF_SIZE]={0};
    spi1.config.freq = 100000;
    csi_pin_set_mux(PB31,1);

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }


    while(n<10) {
        ret2 = hal_spi_recv(&spi1, spi_data_buf1, size, SPI_RX_TIMEOUT);
        aos_msleep(50);
        for (int i = 0; i < size; i++)
        {
            printf("master recv is %d\n",spi_data_buf1[i]);
            if(spi_data_buf1[i] != i + 3){
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

static void aos_hal_spi_master_send_recv(uint16_t size)
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
    csi_pin_set_mux(PB31,1);

    ret1 = hal_spi_init(&spi1);
    if (ret1 != 0) {
        printf("spi1 init error !\n");
    }

    for (i = 0; i < size; i++) {
        spi_data_buf[i] = i + 2;
    }
    while(n<10) {
        ret2 = hal_spi_send_recv(&spi1,spi_data_buf,spi_data_buf1,size, SPI_RX_TIMEOUT);
        aos_msleep(500);
        for (int i = 0; i < size; i++)
        {
            printf("master send is %d\n",spi_data_buf[i]);
            printf("master recv is %d\n",spi_data_buf1[i]);
            if(spi_data_buf1[i] != i + 3){
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

// static void aos_hal_spi_master_send_and_recv(uint16_t size, uint32_t frequency, uint32_t time)
// {
//     int ret1     = -1;
//     int ret2     = -1;
//     int ret2_result     = 0;
//     int ret3     = -1;
//     int n        = 0;
//     int i       = 0;
//     char spi_data_buf[size];
//     char spi_data_buf1[SPI_BUF_SIZE]={0};
//     spi1.config.freq = frequency;
//     csi_pin_set_mux(PB31,4);
//     csi_gpio_write(&gpio, 1 << 31, 1);

//     ret1 = hal_spi_init(&spi1);
//     if (ret1 != 0) {
//         printf("spi1 init error !\n");
//     }

//     for (i = 0; i < size; i++) {
//         spi_data_buf[i] = i + 2;
//     }
//     while(n<10) {
//         csi_gpio_write(&gpio, 1 << 31, 0);
//         ret2 = hal_spi_send_and_recv(&spi1, spi_data_buf, size, spi_data_buf1, size, time);
//         csi_gpio_write(&gpio, 1 << 31, 1);
//         aos_msleep(50);
//         for (int i = 0; i < size; i++)
//         {
//             printf("master send is %d\n",spi_data_buf[i]);
//         }
        
//         if (ret2 == 0) {
//             printf("spi1 data send succeed !\n");
//         }
//         else
//         {
//             printf("spi1 data send error !\n");
//         }
//         n++;
//     };
//     n = 0;

//     while(n<10) {
//         csi_gpio_write(&gpio, 1 << 31, 0);
//         ret2 = hal_spi_send_and_recv(&spi1, spi_data_buf, size, spi_data_buf1, size, time);
//         csi_gpio_write(&gpio, 1 << 31, 1);
//         aos_msleep(50);
//         for (int i = 0; i < size; i++)
//         {
//             printf("master recv is %d\n",spi_data_buf1[i]);
//             if(spi_data_buf1[i] != i + 3){
//                 ret2_result += 1;
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

// static void aos_hal_spi_master_send_and_send(uint16_t size)
// {
//     int ret1     = -1;
//     int ret2     = -1;
//     int ret3     = -1;
//     int n        = 0;
//     int i       = 0;
//     char spi_data_buf[size];
//     spi1.config.freq = 100000;
//     csi_pin_set_mux(PB31,4);
//     csi_gpio_write(&gpio, 1 << 31, 1);

//     ret1 = hal_spi_init(&spi1);
//     if (ret1 != 0) {
//         printf("spi1 init error !\n");
//     }

//     for (i = 0; i < size; i++) {
//         spi_data_buf[i] = i + 2;
//     }
//     while(n<10) {
//         csi_gpio_write(&gpio, 1 << 31, 0);
//         ret2 = hal_spi_send_and_send(&spi1, spi_data_buf, size, spi_data_buf, size, SPI_TX_TIMEOUT);
//         csi_gpio_write(&gpio, 1 << 31, 1);
//         aos_msleep(50);
//         if (ret2 == 0) {
//             printf("spi1 data send succeed !\n");
//             for (int i = 0; i < size; i++)
//             {
//                 printf("master send is %d\n",spi_data_buf[i]);
//             }
//         }
//         else
//         {
//             printf("spi1 data send error !\n");
//         }
//         n++;
//     };

//     ret3 = hal_spi_finalize(&spi1);
//     if (ret3 != 0) {
//         printf("spi1 finalize error !\n");
//     }

//     if (ret1 == 0 && ret2 == 0 && ret3 == 0) {
//         AT_BACK_OK();
//     }
//     else{
//         AT_BACK_ERR();
//     }
// }

void test_hal_spi_master(char *cmd, int type, char *data)
{
    csi_gpio_init(&gpio, 1);
    csi_gpio_dir(&gpio, 1 << 31, GPIO_DIRECTION_OUTPUT);
    
    
    csi_pin_set_mux(PB28,1);
    csi_pin_set_mux(PB29,1);
    csi_pin_set_mux(PB30,1);

    spi1.port = SPI1_PORT_NUM;
    spi1.config.mode  = HAL_SPI_MODE_MASTER;

    if (strcmp((const char *)data, "'send_multi'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 100000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'send_single'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE2, 100000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'recv_multi'\0") == 0) {
        aos_hal_spi_master_recv(SPI_BUF_SIZE);
    }else if (strcmp((const char *)data, "'recv_single'\0") == 0) {
        aos_hal_spi_master_recv(SPI_BUF_SIZE2);
    }else if (strcmp((const char *)data, "'multi_send_recv'\0") == 0) {
        aos_hal_spi_master_send_recv(SPI_BUF_SIZE);
    }else if (strcmp((const char *)data, "'single_send_recv'\0") == 0) {
        aos_hal_spi_master_send_recv(SPI_BUF_SIZE2);
    }
    // else if (strcmp((const char *)data, "'multi_send_and_recv'\0") == 0) {
    //     aos_hal_spi_master_send_and_recv(SPI_BUF_SIZE, 100000, SPI_TX_TIMEOUT);
    // }else if (strcmp((const char *)data, "'single_send_and_recv'\0") == 0) {
    //     aos_hal_spi_master_send_and_recv(SPI_BUF_SIZE2, 100000, SPI_TX_TIMEOUT);
    // }else if (strcmp((const char *)data, "'multi_send_and_send'\0") == 0) {
    //     aos_hal_spi_master_send_and_send(SPI_BUF_SIZE);
    // }else if (strcmp((const char *)data, "'single_send_and_send'\0") == 0) {
    //     aos_hal_spi_master_send_and_send(SPI_BUF_SIZE2);
    // }
    else if (strcmp((const char *)data, "'freq_100000'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 100000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_1M'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 1000000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_10M'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 10000000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'freq_3M'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 3000000, SPI_TX_TIMEOUT);
    }else if (strcmp((const char *)data, "'timeout_HAL_WAIT_FOREVER'\0") == 0) {
        aos_hal_spi_master_send(SPI_BUF_SIZE, 100000, HAL_WAIT_FOREVER);
    }
}
