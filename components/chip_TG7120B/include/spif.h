/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     spiflash_w25q64fv.h
 * @brief    head file for spiflash_w25q64fv
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _SPIFLASH_W25Q64FV_H_
#define _SPIFLASH_W25Q64FV_H_

//#include "drv_spiflash.h"
//#include "soc.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

//todo
#ifndef         __IOM
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */
#endif

#define CONFIG_SPIFLASH_NUM 1
#define DEBUG_EN            1
//#define SPIF_ADDR_START       0x11005000
//#define SPIF_ADDR_END         0x11010000
//#define SPIF_SECTOR_COUNT     11

#define SPIF_ADDR_START         0x11000000
#define SPIF_ADDR_END         0x11080000//0x11010000
#define SPIF_SECTOR_COUNT     128

#define SPIF_SECTOR_SIZE      0x1000
#define SPIF_PAGE_SIZE        0x100

//xip flash read instrcution
#define XFRD_FCMD_READ          0x801000B
#define XFRD_FCMD_READ_DUAL     0x801003B
#define XFRD_FCMD_READ_QUAD     0x801006B


#define FCMD_RESET              0x99  //reset
#define FCMD_ENRST              0x66  //enable reset
#define FCMD_WREN               0x06  //write enable
#define FCMD_WRDIS              0x04  //write disable
#define FCMD_VSRWREN            0x50  //Volatile SR Write Enable


#define FCMD_CERASE             0x60  //(or 0xC7)chip erase
#define FCMD_SERASE             0x20  //sector erase
#define FCMD_BERASE32           0x52  //block erease 32k
#define FCMD_BERASE64           0xD8

#define FCMD_DPWRDN             0xB9  //deep power down
#define FCMD_RLSDPD             0xAB  //release from powerdown(and read device id)
#define FCMD_WRST               0x01  //write status
#define FCMD_RDID               0x9F  //read ID
#define FCMD_RDST               0x05  //read status
#define FCMD_RDST_H             0x35  //read status high byte
#define FCMD_PPROG              0x02  //page program
#define FCMD_READ               0x03  //read
#define FCMD_READF              0x0B  //fast read
#define FCMD_READDO             0x3B  //dual output fast read
#define FCMD_READDIO            0xBB  //dual I/O fast read
#define FCMD_READQO             0x6B  //quad output fast read
#define FCMD_READQIO            0xeB  //quad I/O fast read
#define FCMD_READQIOW           0xe7  //quad I/O fast read word

#ifdef __cplusplus
}
#endif

#endif
