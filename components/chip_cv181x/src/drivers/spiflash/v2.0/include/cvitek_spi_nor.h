/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/
#ifndef __CVITEK_SPI_NOR_H_
#define __CVITEK_SPI_NOR_H_

/* for mdelay or udelay */
#include <drv/common.h>
#include "spi_nor.h"
#include "io.h"
#ifndef CONFIG_KERNEL_NONE
#include "drv/dma.h"
#endif

#ifndef CONFIG_KERNEL_NONE
#define		CONFIG_DMA_SUPPORT
#endif

//#define DMMR_DMA_MODE
#define SPI_NOR_REGBASE         0x10000000
#define DMA_MIN_THLD		1024
#define DMMR_MAP_SIZE		0x2000000

/* Cvitek SPIF registers, bits and macros */
#define SPI_FLASH_BLOCK_SIZE             256
#define SPI_TRAN_CSR_ADDR_BYTES_SHIFT    8
#define SPI_MAX_FIFO_DEPTH               8

/* register definitions */
#define REG_SPI_CTRL                     0x000
#define REG_SPI_CE_CTRL                  0x004
#define REG_SPI_DLY_CTRL                 0x008
#define REG_SPI_DMMR                     0x00C
#define REG_SPI_TRAN_CSR                 0x010
#define REG_SPI_TRAN_NUM                 0x014
#define REG_SPI_FIFO_PORT                0x018
#define REG_SPI_FIFO_PT                  0x020
#define REG_SPI_INT_STS                  0x028
#define REG_SPI_INT_EN                   0x02C

/* bit definition */
#define BIT_SPI_CTRL_CPHA                    (0x01 << 12)
#define BIT_SPI_CTRL_CPOL                    (0x01 << 13)
#define BIT_SPI_CTRL_HOLD_OL                 (0x01 << 14)
#define BIT_SPI_CTRL_WP_OL                   (0x01 << 15)
#define BIT_SPI_CTRL_LSBF                    (0x01 << 20)
#define BIT_SPI_CTRL_SRST                    (0x01 << 21)
#define BIT_SPI_CTRL_SCK_DIV_SHIFT           0
#define BIT_SPI_CTRL_FRAME_LEN_SHIFT         16
#define BIT_SPI_CTRL_SCK_DIV_MASK            0x7FF

#define BIT_SPI_CE_CTRL_CEMANUAL             (0x01 << 0)
#define BIT_SPI_CE_CTRL_CEMANUAL_EN          (0x01 << 1)

#define BIT_SPI_CTRL_FM_INTVL_SHIFT          0
#define BIT_SPI_CTRL_CET_SHIFT               8

#define BIT_SPI_DMMR_EN                      (0x01 << 0)

#define BIT_SPI_TRAN_CSR_TRAN_MODE_RX        (0x01 << 0)
#define BIT_SPI_TRAN_CSR_TRAN_MODE_TX        (0x01 << 1)
#define BIT_SPI_TRAN_CSR_CNTNS_READ          (0x01 << 2)
#define BIT_SPI_TRAN_CSR_FAST_MODE           (0x01 << 3)
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_1_BIT     (0x0 << 4)
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_2_BIT     (0x01 << 4)
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_4_BIT     (0x02 << 4)
#define BIT_SPI_TRAN_CSR_DMA_EN              (0x01 << 6)
#define BIT_SPI_TRAN_CSR_MISO_LEVEL          (0x01 << 7)
#define BIT_SPI_TRAN_CSR_ADDR_BYTES_NO_ADDR  (0x0 << 8)
#define BIT_SPI_TRAN_CSR_WITH_CMD            (0x01 << 11)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_1_BYTE (0x0 << 12)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_2_BYTE (0x01 << 12)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_4_BYTE (0x02 << 12)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_8_BYTE (0x03 << 12)
#define BIT_SPI_TRAN_CSR_GO_BUSY             (0x01 << 15)
#define SPI_TRAN_CSR_DUMMY_MASK              (0xf << 16)

#define BIT_SPI_TRAN_CSR_TRAN_MODE_MASK      0x0003
#define BIT_SPI_TRAN_CSR_ADDR_BYTES_MASK     0x0700
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_MASK   0x3000
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_MASK     (0x3 << 4)

#define BIT_SPI_INT_TRAN_DONE                (0x01 << 0)
#define BIT_SPI_INT_RD_FIFO                  (0x01 << 2)
#define BIT_SPI_INT_WR_FIFO                  (0x01 << 3)
#define BIT_SPI_INT_RX_FRAME                 (0x01 << 4)
#define BIT_SPI_INT_TX_FRAME                 (0x01 << 5)

#define BIT_SPI_INT_TRAN_DONE_EN             (0x01 << 0)
#define BIT_SPI_INT_RD_FIFO_EN               (0x01 << 2)
#define BIT_SPI_INT_WR_FIFO_EN               (0x01 << 3)
#define BIT_SPI_INT_RX_FRAME_EN              (0x01 << 4)
#define BIT_SPI_INT_TX_FRAME_EN              (0x01 << 5)

#define SPI_NOR_MAX_ADDR_WIDTH		4

#define SPI_FLASH_3B_ADDR_LEN           3
#define SPI_FLASH_4B_ADDR_LEN           4
#define SPI_FLASH_BUF_LEN               (1 + SPI_FLASH_4B_ADDR_LEN)
#define SPI_FLASH_16MB_BOUN             0x1000000

struct cvi_spif {
        void  *io_base;
        int irq;
        uint32_t org_ce_ctrl;
        uint32_t org_tran_csr;
        struct spi_nor nor;
#ifdef CONFIG_DMA_SUPPORT
        csi_dma_ch_config_t dma_chan_config;
#endif
        void *priv;
};

struct dmmr_reg_t {
        uint8_t read_cmd;
        uint32_t dummy_clock;
        uint32_t reg_set;
};

int cvi_spif_clk_setup(struct cvi_spif *spif, uint32_t sck_div);
int cvi_spif_read(struct spi_nor *nor, uint64_t from, uint32_t len, void *buf);
int cvi_spif_write(struct spi_nor *nor, uint32_t to, uint32_t len, const void *buf);
int cvi_spif_setup_flash(struct cvi_spif *spif);
int cvi_spif_read_reg(struct spi_nor *nor, uint8_t opcode, uint8_t *buf, int len);
int cvi_spif_write_reg(struct spi_nor *nor, uint8_t opcode, uint8_t *buf, int len);
#ifdef SPI_NOR_DEBUG
        #define pr_debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
        #define pr_debug(fmt, ...)
#endif


#endif


