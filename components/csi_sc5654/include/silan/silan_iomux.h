/*
 * silan_iomux.h
 */

#ifndef __SILAN_IOMUX_H__
#define __SILAN_IOMUX_H__

#include "silan_types.h"

#define    IO_CONFIG_PA0         0
#define    IO_CONFIG_PA1         1
#define    IO_CONFIG_PA2         2
#define    IO_CONFIG_PA3         3
#define    IO_CONFIG_PA4         4
#define    IO_CONFIG_PA5         5
#define    IO_CONFIG_PA6         6
#define    IO_CONFIG_PA7         7
#define    IO_CONFIG_PB0         8
#define    IO_CONFIG_PB1         9
#define    IO_CONFIG_PB2        10
#define    IO_CONFIG_PB3        11
#define    IO_CONFIG_PB4        12
#define    IO_CONFIG_PB5        13
#define    IO_CONFIG_PB6        14
#define    IO_CONFIG_PB7        15
#define    IO_CONFIG_PB8        16
#define    IO_CONFIG_PC0        17
#define    IO_CONFIG_PC1        18
#define    IO_CONFIG_PC2        19
#define    IO_CONFIG_PC3        20
#define    IO_CONFIG_PC4        21
#define    IO_CONFIG_PC5        22
#define    IO_CONFIG_PC6        23
#define    IO_CONFIG_PC7        24
#define    IO_CONFIG_PC8        25
#define    IO_CONFIG_PC9        26
#define    IO_CONFIG_PC10       27
#define    IO_CONFIG_PC11       28
#define    IO_CONFIG_PC12       29
#define    IO_CONFIG_PC13       30
#define    IO_CONFIG_PD0        31
#define    IO_CONFIG_PD1        32
#define    IO_CONFIG_PD2        33
#define    IO_CONFIG_PD3        34
#define    IO_CONFIG_PD4        35
#define    IO_CONFIG_PD5        36
#define    IO_CONFIG_PD6        37
#define    IO_CONFIG_PD7        38
#define    IO_CONFIG_PD8        39
#define    IO_CONFIG_PD9        40
#define    IO_CONFIG_PD10       41

#define IO_FUNC_GPIO		0
#define IO_FUNC_PWM         6
#define IO_FUNC_TCK         7

#define IO_FUNC_SPI         2
#define IO_FUNC_PDB         3
#define IO_FUNC_SDIO        1

#define IO_FUNC_SPDIF_O     5
#define IO_FUNC_SPDIF_I     4

#define IO_FUNC_PCM_I2S     1
#define IO_FUNC_I2S_20      4
#define IO_FUNC_I2S_71      5

// PA1
#define PA1_FUNC_TCK        IO_FUNC_TCK
// PA3
#define PA3_FUNC_SPDIF_O    IO_FUNC_SPDIF_O
// PA4
#define PA4_FUNC_UART2      3
#define PA4_FUNC_SPDIF_IN   IO_FUNC_SPDIF_I
// PA5
#define PA5_FUNC_UART2      3
#define PA5_FUNC_SPDIF_IN   IO_FUNC_SPDIF_I
// PA6
#define PA6_FUNC_I2C1       2
#define PA6_FUNC_UART3      3
#define PA6_FUNC_SPDIF_IN   IO_FUNC_SPDIF_I
#define PA6_FUNC_TCK        IO_FUNC_TCK
// PA7
#define PA7_FUNC_I2C1       2
#define PA7_FUNC_UART3      3
#define PA7_FUNC_SPDIF_IN   IO_FUNC_SPDIF_I
// PB0
#define PB0_FUNC_UART1      1
// PB1
#define PB1_FUNC_UART1      1
// PB2
#define PB2_FUNC_UART1      1
#define PB2_FUNC_I2C1       2
#define PB2_FUNC_UART4      3
#define PB2_FUNC_TCK        IO_FUNC_TCK
// PB3
#define PB3_FUNC_UART1      1
#define PB3_FUNC_I2C1       2
#define PB3_FUNC_UART4      3
#define PB3_FUNC_SPDIF_O    IO_FUNC_SPDIF_O
// PB4
#define PB4_FUNC_PCM_I2S    IO_FUNC_PCM_I2S
#define PB4_FUNC_SPI1       IO_FUNC_SPI
#define PB4_FUNC_PDB        IO_FUNC_PDB
#define PB4_FUNC_I1_I2S_20  IO_FUNC_I2S_20
#define PB4_FUNC_I1_I2S_71  IO_FUNC_I2S_71
// PB5
#define PB5_FUNC_PCM_I2S    IO_FUNC_PCM_I2S
#define PB5_FUNC_SPI1       IO_FUNC_SPI
#define PB5_FUNC_PDB        IO_FUNC_PDB
#define PB5_FUNC_I1_I2S_20  IO_FUNC_I2S_20
#define PB5_FUNC_I1_I2S_71  IO_FUNC_I2S_71
// PB6
#define PB6_FUNC_PCM_I2S    IO_FUNC_PCM_I2S
#define PB6_FUNC_SPI1       IO_FUNC_SPI
#define PB6_FUNC_PDB        IO_FUNC_PDB
#define PB6_FUNC_I1_I2S_20  IO_FUNC_I2S_20
#define PB6_FUNC_I1_I2S_71  IO_FUNC_I2S_71
// PB7
#define PB7_FUNC_PCM_I2S    IO_FUNC_PCM_I2S
#define PB7_FUNC_SPI1       IO_FUNC_SPI
#define PB7_FUNC_PDB        IO_FUNC_PDB
#define PB7_FUNC_I1_I2S_20  IO_FUNC_I2S_20
#define PB7_FUNC_I1_I2S_71  IO_FUNC_I2S_71
// PB8
#define PB8_FUNC_PCM_I2S    IO_FUNC_PCM_I2S
#define PB8_FUNC_SPI1       IO_FUNC_SPI
#define PB8_FUNC_PDB        IO_FUNC_PDB
#define PB8_FUNC_I2_I2S     IO_FUNC_I2S_20
#define PB8_FUNC_I1_I2S_71  IO_FUNC_I2S_71
#define PB8_FUNC_TCK        IO_FUNC_TCK
// PC0
#define PC0_FUNC_PCM_I2S    IO_FUNC_PCM_I2S
#define PC0_FUNC_SD         1
#define PC0_FUNC_PDB        IO_FUNC_PDB
#define PC0_FUNC_I2_I2S     IO_FUNC_I2S_20
#define PC0_FUNC_I1_I2S_71  IO_FUNC_I2S_71
// PC1
#define PC1_FUNC_SD         1
#define PC1_FUNC_PDB        IO_FUNC_PDB
#define PC1_FUNC_I2_I2S     IO_FUNC_I2S_20
#define PC1_FUNC_I1_I2S_71  IO_FUNC_I2S_71
// PC2
#define PC2_FUNC_SD         1
#define PC2_FUNC_PDB        IO_FUNC_PDB
#define PC2_FUNC_I2_I2S     IO_FUNC_I2S_20
// PC3
#define PC3_FUNC_SD         1
#define PC3_FUNC_PDB        IO_FUNC_PDB
#define PC3_FUNC_I3_I2S     IO_FUNC_I2S_20
// PC4
#define PC4_FUNC_SD         1
#define PC4_FUNC_PDB        IO_FUNC_PDB
#define PC4_FUNC_I3_I2S     IO_FUNC_I2S_20
// PC5
#define PC5_FUNC_SD         1
#define PC5_FUNC_PDB        IO_FUNC_PDB
#define PC5_FUNC_I3_I2S     IO_FUNC_I2S_20
// PC6
#define PC6_FUNC_SD         1
#define PC6_FUNC_PDB        IO_FUNC_PDB
#define PC6_FUNC_I3_I2S     IO_FUNC_I2S_20
// PC7
#define PC7_FUNC_SDIO       IO_FUNC_SDIO
#define PC7_FUNC_PDB        IO_FUNC_PDB
#define PC7_FUNC_O1_I2S_20  IO_FUNC_I2S_20
#define PC7_FUNC_O1_I2S_71  IO_FUNC_I2S_71
#define PC7_FUNC_TCK        IO_FUNC_TCK
// PC8
#define PC8_FUNC_SDIO       IO_FUNC_SDIO
#define PC8_FUNC_PDB        IO_FUNC_PDB
#define PC8_FUNC_O1_I2S_20  IO_FUNC_I2S_20
#define PC8_FUNC_O1_I2S_71  IO_FUNC_I2S_71
// PC9
#define PC9_FUNC_SDIO       IO_FUNC_SDIO
#define PC9_FUNC_PDB        IO_FUNC_PDB
#define PC9_FUNC_O1_I2S_20  IO_FUNC_I2S_20
#define PC9_FUNC_O1_I2S_71  IO_FUNC_I2S_71
// PC10
#define PC10_FUNC_SDIO      IO_FUNC_SDIO
#define PC10_FUNC_PDB       IO_FUNC_PDB
#define PC10_FUNC_O1_I2S_20 IO_FUNC_I2S_20
#define PC10_FUNC_O1_I2S_71 IO_FUNC_I2S_71
// PC11
#define PC11_FUNC_SDIO      IO_FUNC_SDIO
#define PC11_FUNC_PDB       IO_FUNC_PDB
#define PC11_FUNC_O2_I2S_20 IO_FUNC_I2S_20
#define PC11_FUNC_O1_I2S_71 IO_FUNC_I2S_71
// PC12
#define PC12_FUNC_SDIO      IO_FUNC_SDIO
#define PC12_FUNC_I2C2      3
#define PC12_FUNC_O2_I2S_20 IO_FUNC_I2S_20
#define PC12_FUNC_O1_I2S_71 IO_FUNC_I2S_71
// PC13
#define PC13_FUNC_SDIO      IO_FUNC_SDIO
#define PC13_FUNC_I2C2      IO_FUNC_PDB
#define PC13_FUNC_O2_I2S_20 IO_FUNC_I2S_20
#define PC13_FUNC_O1_I2S_71 IO_FUNC_I2S_71
// PD0
#define PD0_FUNC_SSP       1
#define PD0_FUNC_SPI2      IO_FUNC_SPI
#define PD0_FUNC_UART2     3
#define PD0_FUNC_O2_I2S_20 IO_FUNC_I2S_20
// PD1
#define PD1_FUNC_SSP       1
#define PD1_FUNC_SPI2      IO_FUNC_SPI
#define PD1_FUNC_UART2     3
#define PD1_FUNC_O3_I2S_20 IO_FUNC_I2S_20
// PD2
#define PD2_FUNC_SSP       1
#define PD2_FUNC_SPI2      IO_FUNC_SPI
#define PD2_FUNC_UART3     3
#define PD2_FUNC_O3_I2S_20 IO_FUNC_I2S_20
// PD3
#define PD3_FUNC_SSP       1
#define PD3_FUNC_SPI2      IO_FUNC_SPI
#define PD3_FUNC_UART3     3
#define PD3_FUNC_O3_I2S_20 IO_FUNC_I2S_20
// PD4
#define PD4_FUNC_SPI2      IO_FUNC_SPI
#define PD4_FUNC_SD        3
#define PD4_FUNC_O3_I2S_20 IO_FUNC_I2S_20
#define PD4_FUNC_TCK       IO_FUNC_TCK
// PD5
#define PD5_FUNC_UART2     1
#define PD5_FUNC_SSP       2
#define PD5_FUNC_SD        3
#define PD5_FUNC_SPDIF_I   IO_FUNC_SPDIF_I
// PD6
#define PD6_FUNC_UART2     1
#define PD6_FUNC_SSP       2
#define PD6_FUNC_SD        3
#define PD6_FUNC_SPDIF_I   IO_FUNC_SPDIF_I
// PD7
#define PD7_FUNC_UART3     1
#define PD7_FUNC_SSP       2
#define PD7_FUNC_SD        3
#define PD7_FUNC_SPDIF_I   IO_FUNC_SPDIF_I
// PD8
#define PD8_FUNC_UART3     1
#define PD8_FUNC_SSP       2
#define PD8_FUNC_SD        3
#define PD8_FUNC_SPDIF_I   IO_FUNC_SPDIF_I
// PD9
#define PD9_FUNC_I2C2      1
#define PD9_FUNC_UART4     2
#define PD9_FUNC_SD        3
#define PD9_FUNC_PDM       4
#define PD9_FUNC_SPDIF_O   IO_FUNC_SPDIF_O
#define PD9_FUNC_TCK       IO_FUNC_TCK
// PD10
#define PD10_FUNC_I2C2     1
#define PD10_FUNC_UART4    2
#define PD10_FUNC_SD       3
#define PD10_FUNC_PDM      4
#define PD10_FUNC_SPDIF_O  IO_FUNC_SPDIF_O

typedef enum {
	SILAN_IOMUX_I2SIN1,
	SILAN_IOMUX_I2SIN2,
	SILAN_IOMUX_I2SIN3,
	SILAN_IOMUX_I2SIN_51,
	SILAN_IOMUX_I2SOUT1,
	SILAN_IOMUX_I2SOUT2,
	SILAN_IOMUX_I2SOUT3,
	SILAN_IOMUX_I2SOUT_51,
	SILAN_IOMUX_I2SPCM,
	SILAN_IOMUX_SDMMC,
	SILAN_IOMUX_SDMMC2,
	SILAN_IOMUX_SDIO,
	SILAN_IOMUX_UART1,
	SILAN_IOMUX_UART2,
	SILAN_IOMUX_UART3,
	SILAN_IOMUX_UART4,
	SILAN_IOMUX_IIC1,
	SILAN_IOMUX_IIC2,
	SILAN_IOMUX_SPI1,
	SILAN_IOMUX_SPI2,
	SILAN_IOMUX_SPDIFIN1,
	SILAN_IOMUX_SPDIFIN2,
	SILAN_IOMUX_SPDIFIN3,
	SILAN_IOMUX_SPDIFIN4,
	SILAN_IOMUX_SPDIFOUT1,
	SILAN_IOMUX_SPDIFOUT2,
	SILAN_IOMUX_SPDIFOUT3,
	SILAN_IOMUX_SPDIFOUT4,
	SILAN_IOMUX_TCK,
	SILAN_IOMUX_PDP,
	SILAN_IOMUX_PDM,
	SILAN_IOMUX_IIC1_2,
	SILAN_IOMUX_IIC2_2,
}silan_iomux_mode_t;


int silan_iomux_adc_open(uint8_t channel);
int silan_iomux_adc_close(uint8_t channel);

void io_config(unsigned int mod,unsigned int func);
//void silan_pad_pullup_open(uint32_t addr, uint32_t pin);
//void silan_pad_pullup_close(uint32_t addr, uint32_t pin);
void silan_gpio_filsel_sel(uint32_t addr,uint32_t pin,int if_open);

void silan_io_pullup_config(unsigned int mod,unsigned int func);
void silan_io_func_config(uint32_t mod, uint32_t func);
void silan_io_driver_config(unsigned int mod,unsigned int func);

#endif

