/*
 * bl_dma.h    // TODO:后面可以根据需要更改该文件
 *
 */

#ifndef COMPONENTS_CHIP_BL606P_BL_DRV_BL_DMA_BL_DMA_H_
#define COMPONENTS_CHIP_BL606P_BL_DRV_BL_DMA_BL_DMA_H_
#include "dma_reg.h"

/**
 *  @brief DMA port type definition
 */
typedef enum {
    bl_dma0_id,    /*!< DMA0 port define,WLSYS,8 channels */
    bl_dma1_id,    /*!< DMA1 port define,WLSYS,4 channels */
    bl_dma2_id,    /*!< DMA2 port define,MMSYS,8 channels */
    bl_dma_id_max, /*!< DMA MAX ID define */
} bl_dma_id_type;

/**
 *  @brief DMA channel type definition
 */
typedef enum {
    bl_dma_ch0 = 0, /*!< DMA channel 0 */
    bl_dma_ch1,     /*!< DMA channel 1 */
    bl_dma_ch2,     /*!< DMA channel 2 */
    bl_dma_ch3,     /*!< DMA channel 3 */
    bl_dma_ch4,     /*!< DMA channel 4 */
    bl_dma_ch5,     /*!< DMA channel 5 */
    bl_dma_ch6,     /*!< DMA channel 6 */
    bl_dma_ch7,     /*!< DMA channel 7 */
    bl_dma_ch_max,  /*!<  */
} bl_dma_chl_type;

/**
 *  @brief dma each data byte is send out LSB-first or MSB-first type definiton
 */
typedef enum {
    bl_dma_lsb_first,                      /*!< dma each byte is send out LSB-first */
    bl_dma_msb_first,                      /*!< dma each byte is send out MSB-first */
} bl_dma_endian_type;

typedef enum {
    bl_dma_req_uart0_rx = 0,               /*!< DMA request peripheral:UART0 RX, DMA0 and DMA1 support */
    bl_dma_req_uart0_tx = 1,               /*!< DMA request peripheral:UART0 TX, DMA0 and DMA1 support */
    bl_dma_req_uart1_rx = 2,               /*!< DMA request peripheral:UART1 RX, DMA0 and DMA1 support */
    bl_dma_req_uart1_tx = 3,               /*!< DMA request peripheral:UART1 TX, DMA0 and DMA1 support */
    bl_dma_req_uart2_rx = 4,               /*!< DMA request peripheral:UART2 RX, DMA0 and DMA1 support */
    bl_dma_req_uart2_tx = 5,               /*!< DMA request peripheral:UART2 TX, DMA0 and DMA1 support */
    bl_dma_req_i2c0_rx  = 6,               /*!< DMA request peripheral:I2C0 RX, DMA0 and DMA1 support */
    bl_dma_req_i2c0_tx  = 7,               /*!< DMA request peripheral:I2C0 TX, DMA0 and DMA1 support */
    bl_dma_req_ir_tx    = 8,               /*!< DMA request peripheral:IR TX, DMA0 and DMA1 support */
    bl_dma_req_gpio_tx  = 9,               /*!< DMA request peripheral:GPIO TX, DMA0 and DMA1 support */
    bl_dma_req_spi0_rx  = 10,              /*!< DMA request peripheral:SPI0 RX, DMA0 and DMA1 support */
    bl_dma_req_spi0_tx  = 11,              /*!< DMA request peripheral:SPI0 TX, DMA0 and DMA1 support */
    bl_dma_req_audio_rx = 12,              /*!< DMA request peripheral:AUDIO RX, DMA0 and DMA1 support */
    bl_dma_req_audio_tx = 13,              /*!< DMA request peripheral:AUDIO TX, DMA0 and DMA1 support */
    bl_dma_req_i2c1_rx  = 14,              /*!< DMA request peripheral:I2C1 RX, DMA0 and DMA1 support */
    bl_dma_req_i2c1_tx  = 15,              /*!< DMA request peripheral:I2C1 TX, DMA0 and DMA1 support */
    bl_dma_req_i2s_rx   = 16,              /*!< DMA request peripheral:I2S RX, DMA0 and DMA1 support */
    bl_dma_req_i2s_tx   = 17,              /*!< DMA request peripheral:I2S TX, DMA0 and DMA1 support */
    bl_dma_req_pdm_rx   = 18,              /*!< DMA request peripheral:PDM RX, DMA0 and DMA1 support */
    bl_dma_req_gpadc_rx = 22,              /*!< DMA request peripheral:GPADC RX, DMA0 and DMA1 support */
    bl_dma_req_gpadc_tx = 23,              /*!< DMA request peripheral:GPADC TX, DMA0 and DMA1 support */
    bl_dma_req_uart3_rx = 0,               /*!< DMA request peripheral:UART3 RX, only DMA2 support */
    bl_dma_req_uart3_tx = 1,               /*!< DMA request peripheral:UART3 TX, only DMA2 support */
    bl_dma_req_spi1_rx  = 2,               /*!< DMA request peripheral:SPI1 RX, only DMA2 support */
    bl_dma_req_spi1_tx  = 3,               /*!< DMA request peripheral:SPI1 TX, only DMA2 support */
    bl_dma_req_uart4_rx = 4,               /*!< DMA request peripheral:UART4 RX, only DMA2 support */
    bl_dma_req_uart4_tx = 5,               /*!< DMA request peripheral:UART4 TX, only DMA2 support */
    bl_dma_req_i2c2_rx  = 6,               /*!< DMA request peripheral:I2C2 RX, only DMA2 support */
    bl_dma_req_i2c2_tx  = 7,               /*!< DMA request peripheral:I2C2 TX, only DMA2 support */
    bl_dma_req_i2c3_rx  = 8,               /*!< DMA request peripheral:I2C3 RX, only DMA2 support */
    bl_dma_req_i2c3_tx  = 9,               /*!< DMA request peripheral:I2C3 TX, only DMA2 support */
    bl_dma_req_dsi_rx   = 10,              /*!< DMA request peripheral:DSI RX, only DMA2 support */
    bl_dma_req_dsi_tx   = 11,              /*!< DMA request peripheral:DSI TX, only DMA2 support */
    bl_dma_req_dbi_tx   = 22,              /*!< DMA request peripheral:DBI TX, only DMA2 support */
    bl_dma_req_none     = 0,               /*!< DMA request peripheral:None */
} bl_dma_periph_req_type;

typedef enum {
    bl_dma_trns_width_8bits = 0,            /*!< dma transfer width:8 bits */
    bl_dma_trns_width_16bits,               /*!< dma transfer width:16 bits */
    bl_dma_trns_width_32bits,               /*!< dma transfer width:32 bits */
    bl_dma_trns_width_64bits,               /*!< dma transfer width:64 bits, only for dma2 channel 0 and channel 1, others should not use this */
} bl_dma_trans_width_type;

typedef enum {
    bl_dma_burst_size_1 = 0,                  /*!< dma burst size:1 * transfer width */
    bl_dma_burst_size_4,                      /*!< dma burst size:4 * transfer width */
    bl_dma_burst_size_8,                      /*!< dma burst size:8 * transfer width */
    bl_dma_burst_size_16,                     /*!< dma burst size:16 * transfer width */
} bl_dma_burst_size_type;

typedef enum {
    bl_dma_trns_m2m = 0,                     /*!< DMA transfer tyep:memory to memory */
    bl_dma_trns_m2p,                         /*!< DMA transfer tyep:memory to peripheral */
    bl_dma_trns_p2m,                         /*!< DMA transfer tyep:peripheral to memory */
    bl_dma_trns_p2p,                         /*!< DMA transfer tyep:peripheral to peripheral */
} bl_dma_trans_dir_type;
/**
 *  @brief DMA channel Configuration Structure type definition
 */
typedef struct
{
    uint32_t src_addr;                          /*!< Source address of DMA transfer */
    uint32_t dst_addr;                          /*!< Destination address of DMA transfer */

    /* reg: ctrl */
    uint32_t transf_length;                     /*!< transfer length[11:0], 0~4095, this is burst count */

    bl_dma_trans_width_type src_transf_width;
    bl_dma_trans_width_type dst_transf_width;

    bl_dma_burst_size_type src_burst_size;     /*!< (srcTransfWidth * srcBurstSize) should <= ch fifo size*/
    bl_dma_burst_size_type dst_burst_size;

    bl_fun_flg_type dst_add_mode;              /*!<  */
    bl_fun_flg_type dst_min_mode;              /*!<  */

    uint8_t fix_cnt;                           /*!<  */
    bl_set_flg_type src_addr_inc;              /*!< Source address increment. 0: No change, 1: Increment */
    bl_set_flg_type dst_addr_inc;              /*!< Destination address increment. 0: No change, 1: Increment */

    /* reg: config */
    bl_dma_trans_dir_type dir;                /*!< Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */

    bl_dma_periph_req_type src_periph;        /*!< Source peripheral select */
    bl_dma_periph_req_type dst_periph;        /*!< Destination peripheral select */
} bl_dma_chan_ctrl_type;

//typedef struct {
//    bl_dma_id_type          dma_id;
//    addr_t                  dma_base;
//    bl_dma_chl_type         dma_ch;
//}bl_dma_dev_info;
//typedef bl_dma_dev_info* pbl_dma_dev_info;

typedef struct {
    bl_dma_id_type          dma_id;
    addr_t                  dma_base;
    bl_dma_chl_type         dma_ch;

    bl_dma_endian_type      dma_inverse_bit;
    bl_dma_chan_ctrl_type   dma_config;
} bl_dma_ctrl_type;
typedef bl_dma_ctrl_type* pbl_dma_ctrl_type;

typedef union {
    struct
    {
        uint32_t TransferSize : 12; /* [11: 0],        r/w,        0x0 */
        uint32_t SBSize       : 2;  /* [13:12],        r/w,        0x1 */
        uint32_t dst_min_mode : 1;  /* [   14],        r/w,        0x0 */
        uint32_t DBSize       : 2;  /* [16:15],        r/w,        0x1 */
        uint32_t dst_add_mode : 1;  /* [   17],        r/w,        0x0 */
        uint32_t SWidth       : 2;  /* [19:18],        r/w,        0x2 */
        uint32_t reserved_20  : 1;  /* [   20],       rsvd,        0x0 */
        uint32_t DWidth       : 2;  /* [22:21],        r/w,        0x2 */
        uint32_t fix_cnt      : 2;  /* [24:23],        r/w,        0x0 */
        uint32_t SLargerD     : 1;  /* [   25],        r/w,        0x0 */
        uint32_t SI           : 1;  /* [   26],        r/w,        0x1 */
        uint32_t DI           : 1;  /* [   27],        r/w,        0x1 */
        uint32_t Prot         : 3;  /* [30:28],        r/w,        0x0 */
        uint32_t I            : 1;  /* [   31],        r/w,        0x0 */
    } bits;
    uint32_t word;
} bl_dma_control_type;

/**
 *  @brief DMA LLI configuration structure type definition
 */
typedef struct
{
    bl_dma_trans_dir_type dir;        /*!< Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */
    bl_dma_periph_req_type src_periph; /*!< Source peripheral select */
    bl_dma_periph_req_type dst_periph; /*!< Destination peripheral select */
} bl_dma_lli_cfg_type;

typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nextlli;
    bl_dma_control_type cfg;
} bl_dma_lli_ctrl_type;

/**
 *  @brief DMA interrupt type definition
 */
typedef enum {
    bl_dma_int_tc_completed = 0, /*!< dma completed interrupt */
    bl_dma_int_err,            /*!< dma error interrupt */
    bl_dma_int_all,            /*!< all the interrupt */
} bl_dma_irq_type;


/** @defgroup  DMA_CHAN_TYPE
 *  @{
 */
#define is_bl_dma_chan_type(type) (((type) == bl_dma_ch0) || \
                                   ((type) == bl_dma_ch1) || \
                                   ((type) == bl_dma_ch2) || \
                                   ((type) == bl_dma_ch3) || \
                                   ((type) == bl_dma_ch4) || \
                                   ((type) == bl_dma_ch5) || \
                                   ((type) == bl_dma_ch6) || \
                                   ((type) == bl_dma_ch7) || \
                                   ((type) == bl_dma_ch_max))

/**
 *  @brief uart config cmd type definition
 */
typedef enum {
    bl_dma_set_src_dma_addr_cmd,
    bl_dma_set_dst_dma_addr_cmd,
    bl_dma_set_endian_type_cmd,
    bl_dma_set_transport_lth_cmd,
    bl_dma_set_src_auto_inc_cmd,
    bl_dma_set_dst_auto_inc_cmd,
    bl_dma_set_src_width_cmd,
    bl_dma_set_dst_width_cmd,
    bl_dma_set_src_burst_width_cmd,
    bl_dma_set_dst_burst_width_cmd,
    bl_dma_set_direction_cmd,
    bl_dma_set_src_periph_cmd,
    bl_dma_set_dst_periph_cmd,
    bl_dma_channel_is_busy_cmd,
    bl_dma_lli_init_cmd,
    bl_dma_lli_add_list_cmd,
    bl_dma_lli_get_counter_cmd,
} bl_dma_cfg_type;

#define BL_DMA0_ADDR        DMA0_BASE
#define BL_DMA0_IRQn        DMA0_ALL_IRQn

#define BL_DMA1_ADDR        DMA1_BASE
#define BL_DMA1_IRQn        DMA1_ALL_IRQn

#define BL_DMA2_ADDR        DMA2_BASE
#define BL_DMA2_IRQn        DMA2_INT0_IRQn   // TODO: dma2 irq num is different to 'dma0 and dma1'
#define BL_DMA2_IRQn1        DMA2_INT1_IRQn
#define BL_DMA2_IRQn2        DMA2_INT2_IRQn
#define BL_DMA2_IRQn3        DMA2_INT3_IRQn
#define BL_DMA2_IRQn4        DMA2_INT4_IRQn
#define BL_DMA2_IRQn5        DMA2_INT5_IRQn
#define BL_DMA2_IRQn6        DMA2_INT6_IRQn
#define BL_DMA2_IRQn7        DMA2_INT7_IRQn

uint32_t bl_dma_get_base(bl_dma_id_type dma_id);
uint32_t bl_dma_get_irq_num(bl_dma_id_type dma_id);

void bl_dma_init(bl_dma_ctrl_type* chCfg);
//uint32_t bl_dma_config(pbl_dma_dev_info dev_base, bl_dma_cfg_type cfg_set, uint32_t value);
uint32_t bl_dma_config(addr_t base, uint32_t dma_ch, bl_dma_cfg_type cfg_set, uint32_t value);
void bl_dma_transport_start(addr_t dma_base, uint8_t dma_ch);
void bl_dma_transport_stop(addr_t dma_base, uint8_t dma_ch);
void bl_dma_uinit(addr_t dma_base);

void bl_dma_lli_init(addr_t dma_base, uint8_t dma_ch, bl_dma_lli_cfg_type * lli_cfg);
void bl_dma_lli_add_list(addr_t dma_base, uint8_t dma_ch, bl_dma_lli_ctrl_type * lli_cfg);
uint32_t bl_dma_lli_get_counter(addr_t dma_base,  uint8_t dma_ch);
void bl_dma_lli_start(addr_t dma_base, uint8_t dma_ch);
void bl_dma_lli_stop(addr_t dma_base, uint8_t dma_ch);
void bl_dma_lli_uninit(addr_t dma_base);

void bl_dma_irq_enable(addr_t dma_base, bl_dma_chl_type dma_ch, bl_dma_irq_type dam_irq_type);
void bl_dma_irq_disable(addr_t dma_base, bl_dma_chl_type dma_ch, bl_dma_irq_type dam_irq_type);
uint32_t bl_dma_irq_get_status(addr_t dma_base, bl_dma_irq_type dam_irq_type);
void bl_dma_irq_clear(addr_t dma_base, bl_dma_irq_type dam_irq_type, uint32_t value);

#endif /* COMPONENTS_CHIP_BL606_BL_DRV_BL_DMA_BL_DMA_H_ */
