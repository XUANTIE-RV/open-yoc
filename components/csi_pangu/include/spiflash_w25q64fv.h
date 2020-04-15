/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     spiflash_w25q64fv.h
 * @brief    head file for spiflash_w25q64fv
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _SPIFLASH_W25Q64FV_H_
#define _SPIFLASH_W25Q64FV_H_

#include "drv/spiflash.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_JEDEC_ID                0x9F
#define W25Q64FV_READ_STATUS        0x05
#define W25Q64FV_WRITE_EN           0x6
#define W25Q64FV_ERASE_SECTOR       0x20
#define W25Q64FV_ERASE_CHIP         0x60
#define W25Q64FV_WRITE_COMMAND      0x2
#define W25Q64FV_READ_COMMAND       0x3
#define W25Q64FV_POWER_DOWN_COMMAND 0xB9
#define W25Q64FV_POWER_UP_COMMAND   0xAB

#define W25Q64FV_BLOCK_SIZE         0x10000
#define W25Q64FV_SECTOR_SIZE        0x1000
#define W25Q64FV_PAGE_SIZE          0x100
#define W25Q64FV_BASEADDR           0x0
#define W25Q64FV_END                0x800000
#define W25Q64FV_SECTOR_NUM         0x800
#define IS_W25Q64FV_ADDR(addr) \
    ((addr >= 0x00000000) && (addr < 0x00800000))
#define W25Q64FV_NUM    1

/* four line read mode */
#define QSPI_CONFIGURATION_4        0x100081
#define QSPI_CONFIGURATION_1        0x80780081
#define QSPI_READ_INSTRUCTION_4     0x41220eb
#define QSPI_READ_INSTRUCTION_2     0x01110bb
#define QSPI_READ_INSTRUCTION_1     0x3
#define QSPI_CONFUGURATION_ADDR     CSKY_QSPIC0_BASE
#define QSPI_READ_INSTRUCTION_ADDR  (CSKY_QSPIC0_BASE + 4)

#define WRITE_DATA_NUM_OFFSET       12
#define READ_DATA_NUM_OFFSET        20
#define QSPI_FLASH_OPCODE_OFFSET    24
#define ADDR_BYTE_OFFSET            16
#define COMMAND_ADDR_EN             (1U << 19)
#define QSPI_IS_BUSY                (1U << 31)
#define QSPI_FLASH_IS_BUSY          (1U << 1)
#define QSPI_FLASH_EXECUTE_COMMAND_EN   1
#define QSPI_READ_DATA_EN           (1U << 23)
#define QSPI_WRITE_DATA_EN          (1U << 15)


#define W25Q64FV_CMD0               (CSKY_QSPIC0_BASE + 0x200)
#define W25Q64FV_CMD1               (CSKY_QSPIC0_BASE + 0x204)
#define W25Q64FV_CMD2               (CSKY_QSPIC0_BASE + 0x208)
#define W25Q64FV_CMD3               (CSKY_QSPIC0_BASE + 0x20c)
#define W25Q64FV_CMD4               (CSKY_QSPIC0_BASE + 0x210)
#define W25Q64FV_CMD5               (CSKY_QSPIC0_BASE + 0x214)
#define W25Q64FV_CMD6               (CSKY_QSPIC0_BASE + 0x218)
#define W25Q64FV_CMD7               (CSKY_QSPIC0_BASE + 0x21c)

typedef struct {
    __IOM uint32_t QSPI_CONFIGURATION;      /* Offset: 0x000 (R/W)  Control register 0 */
    __IOM uint32_t DEVICE_READ_INSTRUCTION; /* Offset: 0x004 (R/W)  device read instruction register */
    uint32_t RESERVED0[34];
    __IOM uint32_t FLASH_COMMAND_CONTROL;   /* Offset: 0x090 (R/W)  Control register 0 */
    __IOM uint32_t FLASH_COMMAND_ADDR;      /* Offset: 0x094 (R/W)  Control register 0 */
    uint32_t RESERVED1[2];
    __IOM uint32_t FLASH_COMMAND_READL;     /* Offset: 0x0a0 (R/W)  Control register 0 */
    __IOM uint32_t FLASH_COMMAND_READH;     /* Offset: 0x0a4 (R/W)  Control register 0 */
    __IOM uint32_t FLASH_COMMAND_WRITEL;    /* Offset: 0x0a8 (R/W)  Control register 0 */
    __IOM uint32_t FLASH_COMMAND_WRITEH;    /* Offset: 0x0ac (R/W)  Control register 0 */
} cd_qspi_reg_t;


typedef enum {
    QSPI_ADDRESS_BYTE_1 = 0,
    QSPI_ADDRESS_BYTE_2 = 1,
    QSPI_ADDRESS_BYTE_3 = 2,
    QSPI_ADDRESS_BYTE_4 = 3,
    QSPI_ADDRESS_BYTE_NONE = 4
} qspi_address_size_e;

typedef enum {
    QSPI_ALTERNATE_BYTE_1 = 0,
    QSPI_ALTERNATE_BYTE_2 = 1,
    QSPI_ALTERNATE_BYTE_3 = 2,
    QSPI_ALTERNATE_BYTE_4 = 3,
    QSPI_ALTERNATE_BYTE_5 = 4,
    QSPI_ALTERNATE_BYTE_6 = 5,
    QSPI_ALTERNATE_BYTE_7 = 6,
    QSPI_ALTERNATE_BYTE_8 = 7,
    QSPI_ALTERNATE_BYTE_NONE = 8
} qspi_alternate_byte_size_e;

typedef struct {
    uint8_t instruction;
    qspi_address_size_e address_size;
    uint32_t address;
    qspi_alternate_byte_size_e alternate_byte_size;
} qspi_command_t;

#ifdef __cplusplus
}
#endif

#endif
