#include "io_config.h"
#include "reg_v33_rg.h"
//#include "ARMCM3.h"
//#include "sleep.h"
#include "tinycrypt/sha256.h"
#include <string.h>
#define ROM_SIZE 0x20000
#define ROM_BASE (0x780000)
void cpu_sleep_asm(void);
void cpu_recover_asm(void);
void boot_ram_start(uint32_t exec_addr)
{
    io_cfg_output(PA01);
    io_set_pin(PA01);
    io_clr_pin(PA01);
    struct tc_sha256_state_struct sha256;
    uint8_t digest[TC_SHA256_DIGEST_SIZE];
    tc_sha256_init(&sha256);
    uint32_t size = ROM_SIZE;
    uint8_t *data = (uint8_t *)ROM_BASE;
    tc_sha256_update(&sha256,data, size);
    tc_sha256_final(digest, &sha256);
    uint32_t *dst = (uint32_t *)0x7e07a8;
    uint32_t *src =(uint32_t *)digest;
    while(src<(uint32_t *)&digest[TC_SHA256_DIGEST_SIZE])
    {
        *dst++ = *src++;
    }
    io_cfg_output(PA01);
    io_set_pin(PA01);
    io_clr_pin(PA01);
    *(uint32_t *)0x7e0000 = 0xf0000000;
    while(1);
/*
    __enable_irq();
    
    V33_RG->SFT_CTRL0F = (0x100+0xd2)/2;
//    cpu_sleep_recover_init();
    V33_RG->WKUP_CTRL0 = 1<<RTC2_IRQn;
    __NVIC_EnableIRQ(RTC2_IRQn);
    V33_RG->TIM_CTRL0 = 30;
    V33_RG->TIM_CTRL4 = V33_RG_TIM_EN_MASK | V33_RG_TIM_INTR_EN_MASK | 1<<V33_RG_TIM_CLK_SEL_POS;
    SCB->SCR |= (1<<2);
    cpu_sleep_asm();
    io_cfg_output(PA01);
    io_set_pin(PA01);
    io_clr_pin(PA01);
    while(1);
*/
}
