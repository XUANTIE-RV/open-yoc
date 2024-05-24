#ifndef CVI_SPINAND_H
#define CVI_SPINAND_H
#include <io.h>

#define SPI_NAND_READ_FROM_CACHE_MODE_X1 0
#define SPI_NAND_READ_FROM_CACHE_MODE_X2 1
#define SPI_NAND_READ_FROM_CACHE_MODE_X4 2

#define SPI_NAND_CMD_WREN			0x06
#define SPI_NAND_CMD_WRDI			0x04
#define SPI_NAND_CMD_GET_FEATURE		0x0F
#define SPI_NAND_CMD_SET_FEATURE		0x1F
#define SPI_NAND_CMD_PAGE_READ_TO_CACHE		0x13
#define SPI_NAND_CMD_READ_FROM_CACHE		0x03
#define SPI_NAND_CMD_READ_FROM_CACHE2		0x0B
#define SPI_NAND_CMD_READ_FROM_CACHEX2		0x3B
#define SPI_NAND_CMD_READ_FROM_CACHEX4		0x6B
#define SPI_NAND_CMD_READ_FROM_DUAL_IO		0xBB
#define SPI_NAND_CMD_READ_FROM_QUAL_IO		0xEB
#define SPI_NAND_CMD_READ_ID			0x9F
#define SPI_NAND_CMD_PROGRAM_LOAD		0x02
#define SPI_NAND_CMD_PROGRAM_LOADX4		0x32
#define SPI_NAND_CMD_PROGRAM_EXECUTE		0x10
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATA		0x84
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATAX4		0xC4
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATA_QUAD_IO	0x72

#define SPI_NAND_CMD_4K_SUBSECTOR_ERASE		0x20
#define SPI_NAND_CMD_32K_SUBSECTOR_ERASE	0x52
#define SPI_NAND_CMD_BLOCK_ERASE		0xD8
#define SPI_NAND_CMD_RESET			0xFF

#define SPI_NAND_FEATURE_ECC_CONFIG	(0x90)
#define SPI_NAND_FEATURE_CFG_ECC_EN	(0x01 << 4)

#define SPI_NAND_FEATURE_PROTECTION	(0xA0)
#define FEATURE_PROTECTION_NONE			0
#define SPI_NAND_PROTECTION_CMP			(0x01 << 1)
#define SPI_NAND_PROTECTION_INV			(0x01 << 2)
#define SPI_NAND_PROTECTION_BP0			(0x01 << 3)
#define SPI_NAND_PROTECTION_BP1			(0x01 << 4)
#define SPI_NAND_PROTECTION_BP2			(0x01 << 5)
#define SPI_NAND_PROTECTION_BRWD		(0x01 << 7)


#define SPI_NAND_FEATURE_FEATURE0	(0xB0)
#define SPI_NAND_FEATURE0_QE			(0x01 << 0)
#define SPI_NAND_FEATURE0_ECC_EN		(0x01 << 4)
#define SPI_NAND_FEATURE0_OTP_EN		(0x01 << 6)
#define SPI_NAND_FEATURE0_OTP_PRT		(0x01 << 7)

#define SPI_NAND_FEATURE_STATUS0	(0xC0)
#define SPI_NAND_STATUS0_OIP			(0x01 << 0)
#define SPI_NAND_STATUS0_WEL			(0x01 << 1)
#define SPI_NAND_STATUS0_E_FAIL			(0x01 << 2)
#define SPI_NAND_STATUS0_P_FAIL			(0x01 << 3)
#define SPI_NAND_STATUS0_ECCS0			(0x01 << 4)
#define SPI_NAND_STATUS0_ECCS1			(0x01 << 5)

#define SPI_NAND_FEATURE_FEATURE1	(0xD0)
#define SPI_NAND_FEATURE1_DS_S0			(0x01 << 5)
#define SPI_NAND_FEATURE1_DS_S1			(0x01 << 6)

#define SPI_NAND_FEATURE_STATUS1	(0xF0)
#define SPI_NAND_STATUS1_ECCSE0			(0x01 << 4)
#define SPI_NAND_STATUS1_ECCSE1			(0x01 << 5)






// /*1880V2 spi nand definitions */
#define REG_SPI_NAND_TRX_CTRL0  0x000
#define BIT_REG_TRX_START           (0x01 << 0)
#define BIT_REG_TRX_SW_RST          (0x01 << 1)
#define BIT_REG_TRX_RST_DONE        (0x01 << 16)

#define REG_SPI_NAND_TRX_CTRL1  0x004
#define BIT_REG_TRX_TIME_START      (0x01 << 0)
#define BIT_REG_TRX_TIME_TA         (0x01 << 4)
#define BIT_REG_TRX_TIME_END        (0x01 << 4)
#define BIT_REG_TRX_TIME_WAIT       (0x01 << 8)
#define BIT_REG_TRX_SCK_H           (0x01 << 16)
#define BIT_REG_TRX_SCK_L           (0x01 << 20)
#define BIT_REG_IO_CPOL             (0x01 << 24)
#define BIT_REG_IO_CPHA             (0x01 << 25)

#define REG_SPI_NAND_TRX_CTRL2  0x008

#define REG_SPI_NAND_TRX_CTRL3  0x00C
#define BIT_REG_IO_SIZE_MODE        (0x01 << 0)
#define SPI_NAND_CTRL3_IO_TYPE_X1_MODE      0
#define SPI_NAND_CTRL3_IO_TYPE_X2_MODE      2
#define SPI_NAND_CTRL3_IO_TYPE_X4_MODE      3

#define BIT_REG_TRX_RW              (0x01 << 16) // 1 for write, 0 for read
#define BIT_REG_TRX_DUMMY_HIZ       (0x01 << 17)
#define BIT_REG_TRX_DMA_EN          (0x01 << 18)
#define BIT_REG_RSP_CHK_EN          (0x01 << 19)

#define REG_SPI_NAND_INT_EN         0x010
#define BIT_REG_TRX_DONE_INT_EN         (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_EN    (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_EN      (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_EN       (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_EN      (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_EN       (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_EN         (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_EN       (0x01 << 9)

#define REG_SPI_NAND_INT_CLR        0x014
#define BIT_REG_TRX_DONE_INT_CLR        (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_CLR   (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_CLR     (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_CLR      (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_CLR     (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_CLR      (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_CLR        (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_CLR      (0x01 << 9)
#define BITS_SPI_NAND_INT_CLR_ALL         (BIT_REG_TRX_DONE_INT_CLR | BIT_REG_TRX_EXCEPTION_INT_CLR \
		| BIT_REG_TX_PUSH_ERR_INT_CLR | BIT_REG_TX_POP_ERR_INT_CLR \
		| BIT_REG_RX_PUSH_ERR_INT_CLR | BIT_REG_RX_POP_ERR_INT_CLR \
		| BIT_REG_DMA_DONE_INT_CLR | BIT_REG_DMA_E_TERM_INT_CLR)

#define REG_SPI_NAND_INT_MASK       0x018
#define BIT_REG_TRX_DONE_INT_MSK        (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_MSK   (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_MSK     (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_MSK      (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_MSK     (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_MSK      (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_MSK        (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_MSK      (0x01 << 9)
#define BITS_SPI_NAND_INT_MASK_ALL      (BIT_REG_TRX_DONE_INT_MSK | BIT_REG_TRX_EXCEPTION_INT_MSK \
		| BIT_REG_TX_PUSH_ERR_INT_MSK | BIT_REG_TX_POP_ERR_INT_MSK \
		| BIT_REG_RX_PUSH_ERR_INT_MSK | BIT_REG_RX_POP_ERR_INT_MSK \
		| BIT_REG_DMA_DONE_INT_MSK | BIT_REG_DMA_E_TERM_INT_MSK)

#define REG_SPI_NAND_INT            0x01C
#define BIT_REG_TRX_DONE_INT            (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT       (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT         (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT          (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT         (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT          (0x01 << 7)
#define BIT_REG_DMA_DONE_INT            (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT          (0x01 << 9)
#define BITS_REG_TRX_DMA_DONE_INT       (BIT_REG_TRX_DONE_INT | BIT_REG_DMA_DONE_INT)

#define REG_SPI_NAND_INT_RAW            0x020
#define BIT_REG_TRX_DONE_INT_RAW            (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_RAW       (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_RAW         (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_RAW          (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_RAW         (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_RAW          (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_RAW            (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_RAW          (0x01 << 9)

#define REG_SPI_NAND_BOOT_CTRL          0x024
#define BIT_REG_BOOT_PRD                    (0x01 << 0)
#define BIT_REG_RSP_DLY_SEL                 (0x01 << 8)
#define BIT_REG_RSP_NEG_SEL                 (0x01 << 12)

#define REG_SPI_NAND_IO_CTRL            0x028
#define BIT_REG_CSN0_OUT_OW_EN              (0x01 << 0)
#define BIT_REG_CSN1_OUT_OW_EN              (0x01 << 1)
#define BIT_REG_SCK_OUT_OW_EN               (0x01 << 3)
#define BIT_REG_MOSI_OUT_OW_EN              (0x01 << 4)
#define BIT_REG_MISO_OUT_OW_EN              (0x01 << 5)
#define BIT_REG_WPN_OUT_OW_EN               (0x01 << 6)
#define BIT_REG_HOLDN_OUT_OW_EN             (0x01 << 7)
#define BIT_REG_CSN0_OUT_OW_VAL             (0x01 << 8)
#define BIT_REG_CSN1_OUT_OW_VAL             (0x01 << 9)
#define BIT_REG_SCK_OUT_OW_VAL              (0x01 << 11)
#define BIT_REG_MOSI_OUT_OW_VAL             (0x01 << 12)
#define BIT_REG_MISO_OUT_OW_VAL             (0x01 << 13)
#define BIT_REG_WPN_OUT_OW_VAL              (0x01 << 14)
#define BIT_REG_HOLDN_OUT_OW_VAL            (0x01 << 15)
#define BIT_REG_CSN0_OEN_OW_EN              (0x01 << 16)
#define BIT_REG_CSN1_OEN_OW_EN              (0x01 << 17)
#define BIT_REG_SCK_OEN_OW_EN               (0x01 << 19)
#define BIT_REG_MOSI_OEN_OW_EN              (0x01 << 20)
#define BIT_REG_MISO_OEN_OW_EN              (0x01 << 21)
#define BIT_REG_WPN_OEN_OW_EN               (0x01 << 22)
#define BIT_REG_HOLDN_OEN_OW_EN             (0x01 << 23)
#define BIT_REG_CSN0_OEN_OW_VAL             (0x01 << 24)
#define BIT_REG_CSN1_OEN_OW_VAL             (0x01 << 25)
#define BIT_REG_SCK_OEN_OW_VAL              (0x01 << 27)
#define BIT_REG_MOSI_OEN_OW_VAL             (0x01 << 28)
#define BIT_REG_MISO_OEN_OW_VAL             (0x01 << 29)
#define BIT_REG_WPN_OEN_OW_VAL              (0x01 << 30)
#define BIT_REG_HOLDN_OEN_OW_VAL            (0x01 << 31)

#define REG_SPI_NAND_IO_STATUS      0x02C
#define BIT_REG_CSN0_VAL                (0x01 << 0)
#define BIT_REG_CSN1_VAL                (0x01 << 1)
#define BIT_REG_SCK_VAL                 (0x01 << 3)
#define BIT_REG_MOSI_VAL                (0x01 << 4)
#define BIT_REG_MISO_VAL                (0x01 << 5)
#define BIT_REG_WPN_VAL                 (0x01 << 6)
#define BIT_REG_HOLDN_VAL               (0x01 << 7)

#define REG_SPI_NAND_TRX_CMD0       0x30
#define BIT_REG_TRX_CMD_IDX             (0x01 << 0)
#define BIT_REG_TRX_CMD_CONT0           (0x01 << 8)
#define TRX_CMD_CONT0_SHIFT             (8)

#define REG_SPI_NAND_TRX_CMD1           0x034
//#define BIT_REG_TRX_CMD_CONT1               (0x01 << 0)

#define REG_SPI_NAND_TRX_CS             0x3C
#define BIT_REG_TRX_CS_SEL                  (0x01 << 0)

#define REG_SPI_NAND_TRX_DMA_CTRL       0x40
#define BIT_REG_DMA_WT_TH                   (0x01 << 0)
#define BIT_REG_DMA_RD_TH                   (0x01 << 8)
#define BIT_REG_DMA_REQ_SIZE                (0x01 << 16)
#define BIT_REG_DMA_TX_EMPTY_SEL            (0x01 << 24)
#define BIT_REG_DMA_RX_FULL_SEL             (0x01 << 25)

#define REG_SPI_NAND_TRX_DMA_STATUS     0x44
#define BIT_REG_DMA_REQ                     (0x01 << 0)
#define BIT_REG_DMA_SINGLE                  (0x01 << 1)
#define BIT_REG_DMA_LAST                    (0x01 << 2)
#define BIT_REG_DMA_ACK                     (0x01 << 3)
#define BIT_REG_DMA_FINISH                  (0x01 << 4)

#define REG_SPI_NAND_TRX_DMA_SW         0x48
#define BIT_REG_DMA_SW_MODE                 (0x01 << 0)
#define BIT_REG_DMA_SW_ACK                  (0x01 << 8)
#define BIT_REG_DMA_SW_FINISH               (0x01 << 9)

#define REG_SPI_NAND_TX_FIFO_STATUS     0x50
#define BIT_REG_TX_PUSH_EMPTY               (0x01 << 0)
#define BIT_REG_TX_PUSH_AE                  (0x01 << 1)
#define BIT_REG_TX_PUSH_HF                  (0x01 << 2)
#define BIT_REG_TX_PUSH_AF                  (0x01 << 3)
#define BIT_REG_TX_PUSH_FULL                (0x01 << 4)
#define BIT_REG_TX_PUSH_ERROR               (0x01 << 5)
#define BIT_REG_TX_PUSH_WORD_COUNT          (0x01 << 8)
#define BIT_REG_TX_POP_EMPTY                (0x01 << 16)
#define BIT_REG_TX_POP_AE                   (0x01 << 17)
#define BIT_REG_TX_POP_HF                   (0x01 << 18)
#define BIT_REG_TX_POP_AF                   (0x01 << 19)
#define BIT_REG_TX_POP_FULL                 (0x01 << 20)
#define BIT_REG_TX_POP_ERROR                (0x01 << 21)
#define BIT_REG_TX_POP_WORD_COUNT           (0x01 << 24)

#define REG_SPI_NAND_RX_FIFO_STATUS     0x54
#define BIT_REG_RX_PUSH_EMPTY               (0x01 << 0)
#define BIT_REG_RX_PUSH_AE                  (0x01 << 1)
#define BIT_REG_RX_PUSH_HF                  (0x01 << 2)
#define BIT_REG_RX_PUSH_AF                  (0x01 << 3)
#define BIT_REG_RX_PUSH_FULL                (0x01 << 4)
#define BIT_REG_RX_PUSH_ERROR               (0x01 << 5)
#define BIT_REG_RX_PUSH_WORD_COUNT          (0x01 << 8)
#define BIT_REG_RX_POP_EMPTY                (0x01 << 16)
#define BIT_REG_RX_POP_AE                   (0x01 << 17)
#define BIT_REG_RX_POP_HF                   (0x01 << 18)
#define BIT_REG_RX_POP_AF                   (0x01 << 19)
#define BIT_REG_RX_POP_FULL                 (0x01 << 20)
#define BIT_REG_RX_POP_ERROR                (0x01 << 21)
#define BIT_REG_RX_POP_WORD_COUNT           (0x01 << 24)

#define REG_SPI_NAND_CMPLT_BYTE_CNT     0x58
#define BIT_REG_CMPLT_CNT                   (0x01 << 0)

#define REG_SPI_NAND_TX_DATA            0x60
#define BIT_REG_TX_DATA                     (0x01 << 0)

#define REG_SPI_NAND_RX_DATA            0x64
#define BIT_REG_RX_DATA                     (0x01 << 0)

#define REG_SPI_NAND_RSP_POLLING        0x68
#define BIT_REG_RSP_EXP_MSK                 (0x01 << 0)
#define BIT_REG_RSP_EXP_VAL                 (0x01 << 8)
#define BIT_REG_RSP_WAIT_TIME_OFFSET        (16)

#define REG_SPI_NAND_SPARE0             0x70
#define BIT_REG_SPARE0                      (0x01 << 0)

#define REG_SPI_NAND_SPARE1             0x74
#define BIT_REG_SPARE1                      (0x01 << 0)

#define REG_SPI_NAND_SPARE_RO           0x78
#define BIT_REG_SPARE_RO                    (0x01 << 0)

#define REG_SPI_NAND_TX_FIFO            0x800
#define BIT_REG_TX_FIFO                     (0x01 << 0)

#define REG_SPI_NAND_RX_FIFO            0xC00
#define BIT_REG_RX_FIFO                     (0x01 << 0)


typedef struct {
    union {
        uint32_t raw;
        struct {
            uint32_t reg_trx_cmd_cont_size   : 3;
            uint32_t reserve0                : 1;
            uint32_t reg_trx_dummy_size      : 2;
            uint32_t reserve1                : 10;
            uint32_t reg_trx_data_size       : 13;
        }bit;
    }reg_trx_size;  // 0x08

    union {
        uint32_t raw;
        struct {
            uint32_t reg_io_size_mode   : 3;
            uint32_t reserve0           : 5;
            uint32_t reg_rx_wait_zero   : 1;
            uint32_t reserve1           : 7;
            uint32_t reg_trx_rw         : 1;
            uint32_t reg_trx_dummy_hiz  : 1;
            uint32_t reg_trx_dma_en     : 1;
            uint32_t reg_rsp_chk_en     : 1;
            uint32_t reg_trx_crc_en     : 1;
        }bit;
    }reg_mode_ctrl0;  // 0x0C

    union {
        uint32_t raw;
        struct {
            uint32_t reg_trx_cmd_idx    : 8;
            uint32_t reg_trx_cmd_cont0  : 24;
        }bit;
    }reg_cont0;  // 0x30

}spinand_reg_t;

typedef enum{
	SPINAND_ERR 	= 0x0001,
	SPINAND_WARN	= 0x0002,
	SPINAND_INFO	= 0x0004,
	SPINAND_DBG 	= 0x0008,
}spinand_msg_prio_e;

extern spinand_msg_prio_e SPINAND_PRINTK_LEVEL;
#define spinand_printf(level, fmt, arg...) \
		do { \
			if (level <= SPINAND_PRINTK_LEVEL) \
				printf("[%s():%d] " fmt, __FUNCTION__, __LINE__, ## arg); \
		} while (0)

#define spinand_writel(addr, val) (writel(val, addr))
#define spinand_readl(addr) (readl(addr))
#define spinand_setbit_32(addr, bit)	spinand_writel(addr, readl(addr) | bit)

csi_error_t cvi_spinand_op(csi_spinand_t *spinand, spi_mem_op_t *op);
#endif /* CVI_SPINAND_H */

