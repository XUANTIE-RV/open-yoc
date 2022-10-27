/**************************************************************************************************

  Phyplus Microelectronics Limited confidential and proprietary.
  All rights reserved.

  IMPORTANT: All rights of this software belong to Phyplus Microelectronics
  Limited ("Phyplus"). Your use of this Software is limited to those
  specific rights granted under  the terms of the business contract, the
  confidential agreement, the non-disclosure agreement and any other forms
  of agreements as a customer or a partner of Phyplus. You may not use this
  Software unless you agree to abide by the terms of these agreements.
  You acknowledge that the Software may not be modified, copied,
  distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy
  (BLE) integrated circuit, either as a product or is integrated into your
  products.  Other than for the aforementioned purposes, you may not use,
  reproduce, copy, prepare derivative works of, modify, distribute, perform,
  display or sell this Software and/or its documentation for any purposes.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

**************************************************************************************************/

#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

#include "types.h"

/*gd25q16 cmd define*/
#define FLASH_WREN                      0x06
#define FLASH_WRDIS                     0x04
#define FLASH_CE                            0x60
//#define FLASH_CE                      0xC7

#define FLASH_DP                            0xB9
#define FLASH_RDI                           0xAB
#define FLASH_SE                            0x20
#define FLASH_BE_32KB                   0x52
#define FLASH_BE_64KB                   0xD8
#define FLASH_WRSR                      0x01
#define FLASH_RDID                      0x9F
#define FLASH_RDSR_LOW              0x05
#define FLASH_RDSR_HIGH             0x35
#define FLASH_PP                            0x02
#define FLASH_READ                      0x03

#define FLASH_PES                                               0x75
#define FLASH_PER                                               0x7A
//#define FLASH_BE_128KB                                    0xD2
//#define FLASH_CRS                                             0xFF
//#define FLASH_DUAL_FAST_READ                      0x3B
//#define FLASH_DUAD_FAST_READ                      0x6B
//#define FLASH_DUAL_IO_FAST_READ                   0xBB
//#define FLASH_DUAD_IO_FAST_READ                   0xEB
//#define FALSH_QUAD_IO_WORD_FAST_READ      0xE7
//#define FLASH_HPM                                             0xA3
//#define FLASH_FAST_READ                                   0x0B
//#define FLASH_REMS                        0x90

typedef enum {
    FLASH_ERROR = 0,
    FLASH_IDLE = 1,
    FLASH_ERASING = 2,
    FALSH_DATA_WRITING = 3,
    FLASH_STATUS_WRITEING = 4,
    FLASH_READING = 5
} FLASH_STATUS_e;

/*gd25q16 api for hardware debug*/
uint32_t spiflash_read_identification(void);
uint16_t spiflash_read_status_register(uint8_t bitsSel);
bool spiflash_bus_busy(void);
void spiflash_program_erase_suspend(void);
void spiflash_program_erase_resume(void);
void spiflash_deep_powerdown(void);
void spiflash_release_from_powerdown(void);

void spiflash_write_enable(void);
void spiflash_write_disable(void);
void spiflash_chip_erase(void);
void spiflash_sector_erase(uint32_t addr);
void spiflash_block_erase_32KB(uint32_t addr);
void spiflash_block_erase_64KB(uint32_t addr);
void spiflash_write_status_register(uint8_t data);

void spiflash_write(uint32_t addr, uint8_t *tx_buf, uint16_t tx_len);
void spiflash_read(uint32_t addr, uint8_t *rx_buf, uint16_t rx_len);

/*gd25q16 api for user develop*/
int GD25_init(void);
int GD25_read(uint32_t addr, uint8_t *data, uint8_t len);
int GD25_erase(uint32_t addr, uint32_t len);
int GD25_write(uint32_t addr, const uint8_t *data, uint8_t len);
#endif
