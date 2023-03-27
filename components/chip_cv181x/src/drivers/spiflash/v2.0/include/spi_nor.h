/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/
#ifndef __SPI_NOR_H__
#define __SPI_NOR_H__

#include <drv/common.h>

#ifndef BIT
#define BIT(nr)                 (1 << (nr))
#endif

#define SPI_NOR_MAX_ID_LEN      8
#define SPI_NOR_MAX_ADDR_WIDTH  4
#define SPI_NOR_MAX_CMD_SIZE    8

#define SNOR_MFR_ATMEL          0x1f
#define SNOR_MFR_GIGADEVICE     0xc8
#define SNOR_MFR_INTEL          0x89
#define SNOR_MFR_MICRON         0x20 /* ST Micro <--> Micron */
#define SNOR_MFR_XMC            0x20
#define SNOR_MFR_MACRONIX       0xc2
#define SNOR_MFR_SPANSION       0x01
#define SNOR_MFR_SST            0xbf
#define SNOR_MFR_WINBOND        0xef /* Also used by some Spansion */
#define SNOR_MFR_EON            0x1c
#define SNOR_MFR_XTX            0x0b
#define SNOR_MFR_FM             0xf8
#define SNOR_MFR_JUYANG         0x4a
#define SNOR_MFR_ZBIT           0x5e
#define SNOR_MFR_WINBOND        0xef
#define SNOR_MFR_ISSI           0x9d

#define JEDEC_MFR(info)               ((info)->id[0])

/* Status Register bits. */
#define CVI_SR_WIP                  BIT(0)  /* Write in progress */
#define CVI_SR_WEL                  BIT(1)  /* Write enable latch */
/* meaning of other SR_* bits may differ between vendors */
#define CVI_SR_BP0                  BIT(2)  /* Block protect 0 */
#define CVI_SR_BP1                  BIT(3)  /* Block protect 1 */
#define CVI_SR_BP2                  BIT(4)  /* Block protect 2 */
#define CVI_SR_TB                   BIT(5)  /* Top/Bottom protect */
#define CVI_SR_SRWD                 BIT(7)  /* SR write protect */
/* Spansion/Cypress specific status bits */
#define CVI_SR_E_ERR                BIT(5)
#define CVI_SR_P_ERR                BIT(6)

#define CVI_SR_QUAD_EN_MX           BIT(6)  /* Macronix Quad I/O */

/* Enhanced Volatile Configuration Register bits */
#define CVI_EVCR_QUAD_EN_MICRON     BIT(7)  /* Micron Quad I/O */

/* Used for Macronix and Winbond flashes. */
/* Enter 4-byte mode */
#define CVI_SPINOR_OP_EN4B          0xb7   
 /* Exit 4-byte mode */ 
#define CVI_SPINOR_OP_EX4B          0xe9  

/* Used for SST flashes only. */
 /* Byte program */
#define CVI_SPINOR_OP_BP            0x02   
 /* Write disable */
#define CVI_SPINOR_OP_WRDI          0x04   
/* Auto address increment word program */
#define CVI_SPINOR_OP_AAI_WP        0xad     

#define CVI_SPINOR_OP_READ_1_1_1_DTR_4B     0x0e
#define CVI_SPINOR_OP_READ_1_2_2_DTR_4B     0xbe
#define CVI_SPINOR_OP_READ_1_4_4_DTR_4B     0xee

/* Double Transfer Rate opcodes - defined in JEDEC JESD216B. */
#define CVI_SPINOR_OP_READ_1_1_1_DTR        0x0d
#define CVI_SPINOR_OP_READ_1_2_2_DTR        0xbd
#define CVI_SPINOR_OP_READ_1_4_4_DTR        0xed
/* Flash opcodes. */
 /* Write status register 1 byte */
#define CVI_SPINOR_OP_WRSR          0x01 
/* Page program (up to 256 bytes) */  
#define CVI_SPINOR_OP_PP            0x02   
/* Read data bytes (low frequency) */ 
#define CVI_SPINOR_OP_READ          0x03  
 /* Read status register */  
#define CVI_SPINOR_OP_RDSR          0x05   
 /* Write enable */
#define CVI_SPINOR_OP_WREN          0x06 
/* Read data bytes (high frequency) */ 
#define CVI_SPINOR_OP_READ_FAST     0x0b 
/* Erase 4KiB block */  
#define CVI_SPINOR_OP_BE_4K         0x20  
/* Write status register 2 */ 
#define CVI_SPINOR_OP_WRSR2         0x31  
/* Quad page program */ 
#define CVI_SPINOR_OP_PP_1_1_4      0x32 
/* Read configuration register */
#define CVI_SPINOR_OP_RDCR          0x35 
/* Quad page program */  
#define CVI_SPINOR_OP_PP_1_4_4      0x38    
/* Read data bytes (Dual Output SPI) */
#define CVI_SPINOR_OP_READ_1_1_2    0x3b 
 /* Read status register 2 */
#define CVI_SPINOR_OP_RDSR2         0x3f  
/* Clear flag status register */  
#define CVI_SPINOR_OP_CLFSR         0x50  
/* Erase 32KiB block */  
#define CVI_SPINOR_OP_BE_32K        0x52 
 /* Read SFDP */
#define CVI_SPINOR_OP_RDSFDP        0x5a   
 /* Read data bytes (Quad Output SPI) */  
#define CVI_SPINOR_OP_READ_1_1_4    0x6b 
/* Read flag status register */   
#define CVI_SPINOR_OP_RDFSR         0x70 
/* Read JEDEC ID */  
#define CVI_SPINOR_OP_RDID          0x9f 
/* Write Extended Address Register */   
#define CVI_SPINOR_OP_WREAR         0xc5    
 /* Erase whole flash chip */
#define CVI_SPINOR_OP_CHIP_ERASE    0xc7 
/* Read Extended Address Register */ 
#define CVI_SPINOR_OP_RDEAR         0xc8 
/* Read data bytes (Dual I/O SPI) */
#define CVI_SPINOR_OP_READ_1_2_2    0xbb  
/* Erase 4KiB block on PMC chips */  
#define CVI_SPINOR_OP_BE_4K_PMC     0xd7 
 /* Sector erase (usually 64KiB) */  
#define CVI_SPINOR_OP_SE            0xd8 
/* Read data bytes (Quad I/O SPI) */   
#define CVI_SPINOR_OP_READ_1_4_4    0xeb  
 


/* 4-byte address opcodes - used on Spansion and some Macronix flashes. */
/* Read data bytes (high frequency) */  
#define CVI_SPINOR_OP_READ_FAST_4B  0x0c 
/* Page program (up to 256 bytes) */  
#define CVI_SPINOR_OP_PP_4B         0x12 
/* Read data bytes (low frequency) */
#define CVI_SPINOR_OP_READ_4B       0x13  
 /* Erase 4KiB block */  
#define CVI_SPINOR_OP_BE_4K_4B      0x21 
 /* Quad page program */  
#define CVI_SPINOR_OP_PP_1_1_4_4B   0x34  
/* Read data bytes (Dual Output SPI) */   
#define CVI_SPINOR_OP_READ_1_1_2_4B 0x3c  
/* Quad page program */ 
#define CVI_SPINOR_OP_PP_1_4_4_4B   0x3e 
 /* Erase 32KiB block */  
#define CVI_SPINOR_OP_BE_32K_4B     0x5c 
 /* Read data bytes (Quad Output SPI) */
#define CVI_SPINOR_OP_READ_1_1_4_4B 0x6c   
 /* Read data bytes (Dual I/O SPI) */  
#define CVI_SPINOR_OP_READ_1_2_2_4B 0xbc   
/* Sector erase (usually 64KiB) */  
#define CVI_SPINOR_OP_SE_4B         0xdc  
/* Read data bytes (Quad I/O SPI) */
#define CVI_SPINOR_OP_READ_1_4_4_4B 0xec  
 



#define		_16M				(0x1000000UL)
#define 	_4K					(0x1000UL)

struct flash_info {
	char            *name;
	uint8_t         id[SPI_NOR_MAX_ID_LEN];
	uint8_t         id_len;
	unsigned        sector_size;
	uint16_t        n_sectors;
	uint16_t        page_size;
	uint32_t        flags;

	/* 0-7 bit for read */
#define RD_SINGLE		BIT(0)
#define RD_DUAL			BIT(1)
#define RD_DUALIO		BIT(2)
#define RD_QUAD			BIT(3)
#define RD_QUADIO		BIT(4)
	/* 8-15 bit for write */
#define WR_SINGLE		BIT(8)
#define WR_QPP			BIT(9)
#define WR_QUAD			BIT(10)
#define WR_QUADIO		BIT(11)

#define SECT_4K			BIT(15)
#define NOR_4B_OPCODES		BIT(16)
#define _10_DUMMY_CYCLE          BIT(17)

#define RD_FULL			(RD_SINGLE | RD_DUAL | RD_DUALIO |\
		RD_QUAD | RD_QUADIO)
#define WR_FULL			(WR_SINGLE | WR_QPP | WR_QUADIO)
};

typedef struct spi_transmit_configuration {
	struct {
		uint8_t buswidth;
		uint8_t opcode;
	} cmd;

	struct {
		uint8_t nbytes;
		uint8_t buswidth;
		uint64_t val;
	} addr;

	struct {
		uint8_t clks;
		uint8_t buswidth;
	} dummy;

	struct {
		uint8_t buswidth;
		uint32_t nbytes;
		union {
			void *in;
			const void *out;
		} buf;
	} data;
} spi_tran_conf_t;

struct spi_nor {
	struct flash_info *info;
	uint32_t					 erase_size;
	uint32_t					 page_size;
	uint8_t                      addr_width;
	uint8_t                      erase_opcode;
	spi_tran_conf_t              read_op;
	spi_tran_conf_t              write_op;
	uint8_t                      cmd_buf[SPI_NOR_MAX_CMD_SIZE];
	uint32_t                     flags;

	int (*read_reg)(struct spi_nor *nor, uint8_t opcode, uint8_t *buf, int len);
	int (*write_reg)(struct spi_nor *nor, uint8_t opcode, uint8_t *buf, int len);

	int (*read)(struct spi_nor *nor, uint64_t from,
			uint32_t len, void *read_buf);
	int (*write)(struct spi_nor *nor, uint32_t to,
			uint32_t len, const void *write_buf);
	int (*erase)(struct spi_nor *nor, uint32_t offs, uint32_t len);

	int (*flash_lock)(struct spi_nor *nor, uint32_t ofs, uint64_t len);
	int (*flash_unlock)(struct spi_nor *nor, uint32_t ofs, uint64_t len);
	int (*flash_is_locked)(struct spi_nor *nor, uint32_t ofs, uint64_t len);

	void *priv;
};

enum write_type {
	ERASE = 0,
	WRITE = 1
};

int spi_nor_rescan(struct spi_nor *nor);
int cvi_spif_erase(struct spi_nor *nor, uint32_t to, uint32_t len);
int write_enable(struct spi_nor *nor);
int spi_nor_write(struct spi_nor *nor, uint32_t to, const void *buf, uint32_t len);
#endif

