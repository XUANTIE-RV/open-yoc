/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include "silan_syscfg.h"
#include "silan_pic.h"
#include <yoc/partition_flash.h>

inline static void silan_dsp_unreset(void)
{
    __REG32(SILAN_SYSCFG_REG11) |= (0x1 << 2);
}

int sc5654_dsp_boot_sdram = 0;
void silan_dsp_reset(void)
{
    silan_pic_init();

    if (sc5654_dsp_boot_sdram) {
        printf("DSP Boot Mode: sdram\n");
        /* 从SDRAM 引导 */
        __REG32(SILAN_SYSCFG_SOC0) |=  (0x1<<1);
    } else {
        /* 从Flash 引导 */
        printf("DSP Boot Mode: flash\n");
        __REG32(SILAN_SYSCFG_SOC0) &= ~(0x1<<1);
    }

    __REG32(0x42020468) = 9; // bootcfg
	silan_dsp_cclk_onoff(CLK_ON);
	silan_dsp_reboot();
}

void silan_dsp_copy(uint32_t addr, uint32_t size, uint32_t load_addr)
{
    uint32_t offset = 4096;

    printf("silan_boot_dsp ...addr:0x%08x, load_addr:0x%08x, len:%d\n", addr, load_addr, size);
    if (partition_flash_read(NULL, addr, (uint8_t *)load_addr, offset)) {
        return;
    }

	printf("Check DSP First Inst: \n");
	if(__REG32(load_addr) == 0x0000AF02)
		printf("%08x, Success!\n", __REG32(load_addr));
	else {
        printf("%08x, Fail!\n", __REG32(load_addr));
        return;
    }

    if (size > offset) {
        if (!partition_flash_read(NULL, addr + offset, (uint8_t *)(load_addr + offset), size - offset)) {
            printf("copy dsp over ...\n");
            return;
        }
    }

    printf("copy dsp error\n");
}