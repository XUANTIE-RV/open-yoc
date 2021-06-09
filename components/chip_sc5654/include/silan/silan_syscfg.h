/*
 * silan_syscfg.h
 */

#ifndef __SILAN_SYSCFG_H__
#define __SILAN_SYSCFG_H__

#include "silan_syscfg_regs.h"
#include "ap1508_datatype.h"
#if defined(__CSKY__)
#include "csi_core.h"
#endif

typedef enum {
    CLK_OFF = 0,
    CLK_ON
} silan_clk_onff_t;

typedef enum {
    CLKSYS_DIV1 = 0,
    CLKSYS_DIV2,
    CLKSYS_DIV3,
    CLKSYS_DIV4
} silan_presys_clksel_t;

/*
silan mcu cclk config
*/
typedef enum {
    MCU_CCLK_DIV1 = 0,
    MCU_CCLK_DIV2,
    MCU_CCLK_DIV4
} silan_mcu_cclksel_t;

typedef enum {
    DCACHE_MODE_DC = 0,
    DCACHE_MODE_MEM
} silan_dcache_mode_t;

typedef enum {
    DSP_BOOT_FLASH = 0,
    DSP_BOOT_SDRAM
} silan_dsp_boot_t;

#if defined(__XCC__)
static inline uint32_t cpu_to_dma(uint32_t addr)
{
    /*
     * dsp inner sram addr is 0x30XX_XXXX,
     * but dmac accesses them at 0x23XX_XXXX.
     *
     * dmac can't access SDRAM at 0x02XX_XXXX,
     * but accesses them at 0x22XX_XXXX.
     * TODO, SDRAM maybe on 0x00XX_XXXX.
     */
    switch (addr & (0xff000000)) {
        case 0x30000000 :
            return 0x23000000 | (addr & 0x00ffffff);

        case 0x02000000 :
            return 0x22000000 | (addr & 0x00ffffff);

        default:
            return addr;
    }
}

static inline uint32_t dma_to_cpu(uint32_t addr)
{
    /*
     * dsp inner sram addr is 0x30XX_XXXX,
     * but dmac accesses them at 0x23XX_XXXX.
     *
     * dmac can't access SDRAM at 0x02XX_XXXX,
     * but accesses them at 0x22XX_XXXX.
     * TODO, SDRAM maybe on 0x00XX_XXXX.
     */
    switch (addr & (0xff000000)) {
        case 0x23000000 :
            return 0x30000000 | (addr & 0x00ffffff);

        case 0x22000000 :
            return 0x02000000 | (addr & 0x00ffffff);

        default:
            return addr;
    }
}
static inline void dcache_writeback(void *addr, uint32_t size)
{
    xthal_dcache_region_writeback(addr, size);
}

static inline void dcache_invalidate(void *addr, uint32_t size)
{
    xthal_dcache_region_invalidate(addr, size);
}
#endif

#if defined(__CC_ARM)
static inline uint32_t cpu_to_dma(uint32_t addr)
{
    return addr;
}

static inline uint32_t dma_to_cpu(uint32_t addr)
{
    return addr;
}

static inline void dcache_writeback(void *addr, uint32_t size)
{
    // do nothing!
}

static inline void dcache_invalidate(void *addr, uint32_t size)
{
    // do nothing!
}
#endif

#if defined(__CSKY__)
static inline uint32_t cpu_to_dma(uint32_t addr)
{
    switch (addr & (0xff000000)) {
        case 0x02000000 :
            return 0x22000000 | (addr & 0x00ffffff);

        default:
            return addr;
    }
}

static inline uint32_t dma_to_cpu(uint32_t addr)
{
    switch (addr & (0xff000000)) {
        case 0x22000000 :
            return 0x02000000 | (addr & 0x00ffffff);

        default:
            return addr;
    }
}

static inline void dcache_writeback(void *addr, uint32_t size)
{
    csi_dcache_clean_range(addr, size);
}

static inline void dcache_invalidate(void *addr, uint32_t size)
{
    csi_dcache_invalid_range(addr, size);
}
#endif

static inline uint32_t *ptr_cpu_to_dma(uint32_t *addr)
{
    return (uint32_t *)cpu_to_dma((uint32_t)addr);
}

static inline uint32_t *ptr_dma_to_cpu(uint32_t *addr)
{
    return (uint32_t *)dma_to_cpu((uint32_t)addr);
}

static inline void silan_dsp_dcache_cfg(int mode)
{
    if (mode == DCACHE_MODE_DC) {
        __REG32(SILAN_SYSCFG_SOC1) |= (0x1);
    } else {
        __REG32(SILAN_SYSCFG_SOC1) &= ~(0x1);
    }
}

static inline void silan_risc_debug_open(void)
{
    __REG32(SILAN_SYSCFG_REG11) |= (0x1 << 21);
}

static inline void silan_risc_debug_close(void)
{
    __REG32(SILAN_SYSCFG_REG11) &= ~(0x1 << 21);
}

static inline void silan_dsp_debug_open(void)
{
    __REG32(SILAN_SYSCFG_REG11) |= (0x1 << 22);
}

static inline void silan_dsp_debug_close(void)
{
    __REG32(SILAN_SYSCFG_REG11) &= ~(0x1 << 22);
}

static inline void silan_mcu_debug_open(void)
{
    __REG32(SILAN_SYSCFG_REG11) |= (0x1 << 20);
}

static inline void silan_mcu_debug_close(void)
{
    __REG32(SILAN_SYSCFG_REG11) &= ~(0x1 << 20);
}

static inline void silan_halt_dsp(void)
{
    __REG32(SILAN_SYSCFG_SOC0) |= (0x1 << 0);
}

static inline void silan_run_dsp(void)
{
    __REG32(SILAN_SYSCFG_SOC0) &= ~(0x1 << 0);
}


void silan_otp_cclk_config(int clksel, silan_clk_onff_t onoff);
void silan_mcu_cclk_onoff(silan_clk_onff_t onoff);
void silan_dsp_cclk_onoff(silan_clk_onff_t onoff);
void silan_risc_cclk_onoff(silan_clk_onff_t onoff);
void silan_dsp_boot_from(int media);
void silan_risc_boot(void);
void silan_dsp_reboot(void);
void silan_risc_reboot(void);

/*
 * return 0: request attachs to dmac0(ADMAC).
 *        1: request attachs to dmac1(SDMAC).
 */
int silan_syscfg_get_dmac_reqid(int id, int *dmac_req);

/*
silan misc clk config
*/
typedef enum {
    SILAN_SPDIF = 0,
    SILAN_SDMMC,
    SILAN_SDIO,
    SILAN_OTP,
    SILAN_CODEC1,
    SILAN_CODEC2,
    SILAN_PWM,
    SILAN_ADC,
    SILAN_TIMER,
    SILAN_RTC,
    SILAN_USBFS,
    SILAN_PDB
} silan_misc_clk_t;

/*
silan mcu clk config
*/
typedef enum {
    MCUCLK_BUS_1 = 0,
    MCUCLK_BUS_2,
    MCUCLK_BUS_4,
} silan_mcu_clk_t;


/*
if_div:
0: sysclk
1: sysclk/2
2: pll_ref/2
3: rcl/2

but:
SILAN_SPDIF
*/
void silan_system_misc_clk_config(silan_misc_clk_t misc_mode, int if_div);
void silan_mcuclk_div_config(silan_mcu_clk_t bus_div);
void silan_adc_cclk_config(int clksel, silan_clk_onff_t onoff);
void silan_pwm_cclk_config(int clksel, silan_clk_onff_t onoff);
void silan_vlsp_cclk_config(silan_clk_onff_t onoff);
void silan_usbfs_cclk_config(silan_clk_onff_t onoff);
void silan_codec1_cclk_config(silan_clk_onff_t onoff);
void silan_codec2_cclk_config(silan_clk_onff_t onoff);
void silan_sdio_cclk_config(int clksel, silan_clk_onff_t onoff);
void silan_sd_cclk_config(int clksel, silan_clk_onff_t onoff);
void silan_risc_wdog_cclk_config(silan_clk_onff_t onoff);

void silan_mcu_cclksel(silan_mcu_cclksel_t bus_div);
void silan_timer_cclk_config(int clksel, silan_clk_onff_t onoff);
void silan_bootup_dsp(uint32_t load_addr, uint32_t len, int media);
void silan_bootup_risc(uint32_t load_addr, uint32_t len);

uint32_t silan_get_timer_cclk(void);
uint32_t silan_get_mcu_cclk(void);
uint32_t silan_get_dsp_cclk(void);
uint32_t silan_get_bus_cclk(void);
uint32_t silan_get_sdio_cclk(void);
uint32_t silan_get_sd_cclk(void);

#define    SOFT_RST_SF1                  0
#define    SOFT_RST_SR1                  1
#define    SOFT_RST_SHDW                 2
#define    SOFT_RST_OTP                  (  3+(  4<<8))
#define    SOFT_RST_SDRAM                (  5+(  6<<8))
#define    SOFT_RST_SPER                 7
#define    SOFT_RST_APER                 8
#define    SOFT_RST_SCFG                 9
#define    SOFT_RST_PCFG                 10
#define    SOFT_RST_SDMMC                11
#define    SOFT_RST_SDIO                 12
#define    SOFT_RST_SDMAC                13
#define    SOFT_RST_OSPDIF               ( 14+( 15<<8))
#define    SOFT_RST_ISPDIF               ( 16+( 17<<8))
#define    SOFT_RST_I1_I2S               ( 18+( 19<<8))
#define    SOFT_RST_I2_I2S               ( 20+( 21<<8))
#define    SOFT_RST_I3_I2S               ( 22+( 23<<8))
#define    SOFT_RST_O1_I2S               ( 24+( 25<<8))
#define    SOFT_RST_O2_I2S               ( 26+( 27<<8))
#define    SOFT_RST_O3_I2S               ( 28+( 29<<8))
#define    SOFT_RST_PCM_I2S              ( 30+( 31<<8))
#define    SOFT_RST_PDM                  ( 32+( 33<<8))
#define    SOFT_RST_ADMAC                34
#define    SOFT_RST_GPIO1                35
#define    SOFT_RST_GPIO2                36
#define    SOFT_RST_SPI                  37
#define    SOFT_RST_UART1                38
#define    SOFT_RST_UART2                39
#define    SOFT_RST_UART3                40
#define    SOFT_RST_UART4                41
#define    SOFT_RST_I2C1                 42
#define    SOFT_RST_I2C2                 43
#define    SOFT_RST_CXC                  44
#define    SOFT_RST_IIR                  45
#define    SOFT_RST_TIMER                ( 46+( 47<<8))
#define    SOFT_RST_PMU                  48
#define    SOFT_RST_RTC                  49
#define    SOFT_RST_CODEC1               50
#define    SOFT_RST_CODEC2               51
#define    SOFT_RST_PDP                  ( 52+( 53<<8))
#define    SOFT_RST_PWM                  ( 54+( 55<<8))
#define    SOFT_RST_ADC                  ( 56+( 57<<8))

void silan_soft_rst(unsigned int mod);

#define    CLK_SDMMC                0
#define    CLK_SDIO                 1
#define    CLK_USBHS                2
#define    CLK_USBFS                3
#define    CLK_SDMAC                4
#define    CLK_OSPDIF               5
#define    CLK_ISPDIF               6
#define    CLK_I1_I2S               7
#define    CLK_I2_I2S               8
#define    CLK_I3_I2S               9
#define    CLK_O1_I2S               10
#define    CLK_O2_I2S               11
#define    CLK_O3_I2S               12
#define    CLK_PCM_I2S              13
#define    CLK_PDM                  14
#define    CLK_ADMAC                15
#define    CLK_GPIO1                16
#define    CLK_GPIO2                17
#define    CLK_SPI                  18
#define    CLK_UART1                19
#define    CLK_UART2                20
#define    CLK_UART3                21
#define    CLK_UART4                22
#define    CLK_I2C1                 23
#define    CLK_I2C2                 24
#define    CLK_PMU                  25
#define    CLK_CXC                  26
#define    CLK_IIR                  27
#define    CLK_TIMER                28
#define    CLK_PDP                  29
#define    CLK_PWM                  30
#define    CLK_ADC                  31
#define    CLK_SSP                  32

#endif  // __SILAN_SYSCFG_H__

