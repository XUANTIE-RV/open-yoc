/*
 * silan_codec.c
 * author :zhugang
 * date :2016-10-26
 ***************************
 * modify:2017-9-22
 * fix adc inport
 * zhugang
 ***************************
 * modify:2017-12-5
 * fix adc analog gain and analog mclk config
 * fix codec load para
 * zhugang
 ***************************
 * modify:2017-12-5
 * fix adc aux/linein/fmin single-ended hardware bug
 * zhugang
 ***************************
 * modify:2018-3-14
 * fix dac power up pop
 * zhugang
 ***************************
 */

#define MODULE_NAME        "CODEC"
//#define __DEBUG__

#include <string.h>
#include "silan_codec.h"
#include "ap1508.h"
#include "silan_codec_regs.h"
#include "silan_syscfg_regs.h"
#include "silan_syscfg.h"
#include "silan_printf.h"
#include "silan_timer.h"

#define SRAM_TEXT __attribute__((section(".sram.text")))
#define SRAM_DATA __attribute__((section(".sram.data")))
#define SRAM_BSS __attribute__((section(".sram.bss")))

CODEC_CORE_REGS *codec_regs[2] = {(CODEC_CORE_REGS *)SILAN_CODEC1_BASE, \
                                  (CODEC_CORE_REGS *)SILAN_CODEC2_BASE};

static uint8_t have_opened[2] = {0};

static void open_codec_clk()
{
    silan_system_misc_clk_config(SILAN_CODEC1,0);
    silan_system_misc_clk_config(SILAN_CODEC2,0);
    silan_system_misc_clk_config(SILAN_RTC,0);
    __REG32(SILAN_SYSCFG_REG9) &= ~((1<<18)|(1<<19));
    __REG32(SILAN_SYSCFG_REG9) |= ((1<<18)|(1<<19));    //rst
}
void silan_codec_pwd_deal()
{
    open_codec_clk();
    CODEC1_AD_R0 = 0x00;
    CODEC1_DA_R0 = 0x00;
    CODEC2_AD_R0 = 0x00;
    CODEC2_DA_R0 = 0x00;
    __REG32(SILAN_SYSCFG_REG9) &= ~((1<<18)|(1<<19));  //IN RESET
}

SRAM_TEXT static void word_memcpy(uint32_t *dst, uint32_t *src, uint32_t cnt)
{
    int i;

    for (i = 0; i < cnt; i++) {
        dst[i] = src[i];
    }
}

void silan_codec1_pwd_deal_save(uint32_t *regs)
{
    word_memcpy(regs, (uint32_t *)SILAN_CODEC1_BASE, 102);

    silan_system_misc_clk_config(SILAN_CODEC1,0);
    __REG32(SILAN_SYSCFG_REG9) &= ~(1<<18);
    __REG32(SILAN_SYSCFG_REG9) |= (1<<18);
    CODEC1_AD_R0 = 0x00;
    CODEC1_DA_R0 = 0x00;
    __REG32(SILAN_SYSCFG_REG9) &= ~(1<<18);
}

void silan_codec1_pwd_deal_restore(uint32_t *regs)
{
    silan_system_misc_clk_config(SILAN_CODEC1,0);

    __REG32(SILAN_SYSCFG_REG9) |= (1<<18);
    word_memcpy((uint32_t *)SILAN_CODEC1_BASE, regs, 102);
}

SRAM_TEXT void silan_codec2_pwd_deal_save(uint32_t *regs)
{
    word_memcpy(regs, (uint32_t *)SILAN_CODEC2_BASE, 102);

    __REG32(SILAN_SYSCFG_REG2) &= ~(1 << 24);
    __REG32(SILAN_SYSCFG_REG9) &= ~(1<<19);
    __REG32(SILAN_SYSCFG_REG9) |= (1<<19);
    CODEC2_AD_R0 = 0x00;
    CODEC2_DA_R0 = 0x00;
    __REG32(SILAN_SYSCFG_REG9) &= ~(1<<19);
}

SRAM_TEXT void silan_codec2_pwd_deal_restore(uint32_t *regs)
{
    __REG32(SILAN_SYSCFG_REG2) |= (1 << 24);
    __REG32(SILAN_SYSCFG_REG9) |= (1<<19);
    word_memcpy((void *)SILAN_CODEC2_BASE, regs, 102);
}

static void config_codec_mclk(silan_codec_t codec, silan_codec_mclk_t div)
{
    int val;
    val = __REG32(SILAN_SYSCFG_REG7);
    if(codec == CODEC1){
        switch(div)
        {
            case CODEC_MCLK_APLL_1:
                val &= ~(0x3);
                val |= CODEC_MCLK_APLL_1;
                break;
            case CODEC_MCLK_APLL_2:
                val &= ~(0x3);
                val |= CODEC_MCLK_APLL_2;
                break;
            case CODEC_MCLK_APLL_3:
                val &= ~(0x3);
                val |= CODEC_MCLK_APLL_3;
                break;
            case CODEC_MCLK_APLL_4:
                val &= ~(0x3);
                val |= CODEC_MCLK_APLL_4;
                break;
        }
    }else if(codec == CODEC2){
        switch(div)
        {
            case CODEC_MCLK_APLL_1:
                val &= ~(0xc);
                val |= CODEC_MCLK_APLL_1<<2;
                break;
            case CODEC_MCLK_APLL_2:
                val &= ~(0xc);;
                val |= CODEC_MCLK_APLL_2<<2;
                break;
            case CODEC_MCLK_APLL_3:
                val &= ~(0xc);;
                val |= CODEC_MCLK_APLL_3<<2;
                break;
            case CODEC_MCLK_APLL_4:
                val &= ~(0xc);;
                val |= CODEC_MCLK_APLL_4<<2;
                break;
        }
    }
    val  |= 0x3<<6; //MCLK ENABLE
    __REG32(SILAN_SYSCFG_REG7) = val;

}

static void config_codec_bclk_dac(silan_codec_t codec, int div)
{
    int val;
    if(codec == CODEC1)
    {
        val = CODEC1_RC;
        val &= ~(0x3f<<1);
        val |= (div<<1);
        CODEC1_RC = val;
    }
    else if(codec == CODEC2)
    {
        val = CODEC2_RC;
        val &= ~(0x3f<<1);
        val |= (div<<1);
        CODEC2_RC = val;
    }
}

static void config_codec_adc_analog_mclk(silan_codec_t codec, int div)
{
    int val;
    if(codec == CODEC1)
    {
        val = CODEC1_R43;
        val &= ~AD_MCLKDIV(0x3f);
        val |= AD_MCLKDIV(div);
        CODEC1_R43 = val;
    }
    else if(codec == CODEC2)
    {
        val = CODEC2_R43;
        val &= ~AD_MCLKDIV(0x3f);
        val |= AD_MCLKDIV(div);
        CODEC2_R43 = val;
    }
}
static void config_codec_over_sample_osr(silan_codec_t codec,int val)
{

    //int OSR_SEL[0x10]={0,1804,1536,1408,1024,768,536,512,384,312,272,256,250,200,192,128};
    //val = OSR_SEL num
    if(codec == CODEC1){
        CODEC1_R6 &= 0x0f;
        CODEC1_R6 |= (val&0xf)<<4;
    }else if(codec == CODEC2){
        CODEC2_R6 &= 0x0f;
        CODEC2_R6 |= (val&0xf)<<4;
    }

    //    CODEC1_R5 = 0x00;
    //    CODEC2_R5 = 0x00;
    //    CODEC1_R6 = 0x00;
    //    CODEC2_R6 = 0x00;
}
/*
   this code don't fix    //zhugang osr
   */
void config_codec_iis_dac(silan_codec_t codec,int rate)
{
    //bclk = 64fs;
    switch (rate) {
        case 8000 :
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,24);
                config_codec_over_sample_osr(CODEC1,2);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,24);
                config_codec_over_sample_osr(CODEC2,2);
            }
            break;

        case 16000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,12);
                config_codec_over_sample_osr(CODEC1,5);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,12);
                config_codec_over_sample_osr(CODEC2,5);
            }
            break;

        case 32000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,6);
                config_codec_over_sample_osr(CODEC1,8);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,6);
                config_codec_over_sample_osr(CODEC2,8);
            }
            break;

        case 64000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_3);
                config_codec_bclk_dac(CODEC1,4);
                config_codec_over_sample_osr(CODEC1,0xb);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_3);
                config_codec_bclk_dac(CODEC2,4);
                config_codec_over_sample_osr(CODEC2,0xb);
            }
            break;

        case 128000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_2);
                config_codec_bclk_dac(CODEC1,3);
                config_codec_over_sample_osr(CODEC1,0xe);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_2);
                config_codec_bclk_dac(CODEC2,3);
                config_codec_over_sample_osr(CODEC2,0xe);
            }
            break;

        case 12000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,16);
                config_codec_over_sample_osr(CODEC1,4);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,16);
                config_codec_over_sample_osr(CODEC2,4);
            }
            break;

        case 24000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,8);
                config_codec_over_sample_osr(CODEC1,7);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,8);
                config_codec_over_sample_osr(CODEC2,7);
            }
            break;

        case 48000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,4);
                config_codec_over_sample_osr(CODEC1,0xb);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,4);
                config_codec_over_sample_osr(CODEC2,0xb);
            }
            break;

        case 96000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_2);
                config_codec_bclk_dac(CODEC1,4);
                config_codec_over_sample_osr(CODEC1,0xb);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_2);
                config_codec_bclk_dac(CODEC2,4);
                config_codec_over_sample_osr(CODEC2,0xb);
            }
            break;

        case 192000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_1);
                config_codec_bclk_dac(CODEC1,4);
                config_codec_over_sample_osr(CODEC1,0xb);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_1);
                config_codec_bclk_dac(CODEC2,4);
                config_codec_over_sample_osr(CODEC2,0xb);
            }
            break;

        case 11025:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,16);
                config_codec_over_sample_osr(CODEC1,4);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,16);
                config_codec_over_sample_osr(CODEC2,4);
            }
            break;

        case 22050:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,8);
                config_codec_over_sample_osr(CODEC1,7);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,8);
                config_codec_over_sample_osr(CODEC2,7);
            }
            break;

        case 44100:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC1,4);
                config_codec_over_sample_osr(CODEC1,0xb);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_bclk_dac(CODEC2,4);
                config_codec_over_sample_osr(CODEC2,0xb);
            }
            break;

        case 88200:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_2);
                config_codec_bclk_dac(CODEC1,4);
                config_codec_over_sample_osr(CODEC1,7);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_2);
                config_codec_bclk_dac(CODEC2,4);
                config_codec_over_sample_osr(CODEC2,7);
            }
            break;

        case 176400:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_1);
                config_codec_bclk_dac(CODEC1,4);
                config_codec_over_sample_osr(CODEC1,0xb);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_1);
                config_codec_bclk_dac(CODEC2,4);
                config_codec_over_sample_osr(CODEC2,0xb);
            }
            break;
    }

}

static void config_codec_bclk_adc(silan_codec_t codec, int div)
{
    int val;
    if(codec == CODEC1)
    {
        val = CODEC1_R47;
        val &= ~(0x3f<<1);
        val |= (div<<1);
        CODEC1_R47 = val;
    }
    else if(codec == CODEC2)
    {
        val = CODEC2_R47;
        val &= ~(0x3f<<1);
        val |= (div<<1);
        CODEC2_R47 = val;
    }
}

/*
   this code don't fix    //zhugang  osr
   */
void config_codec_iis_adc(silan_codec_t codec,int rate)
{
    //bclk = 64fs;

    switch (rate) {
        case 8000 :
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x18);
                config_codec_bclk_adc(CODEC1,24);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x18);
                config_codec_bclk_adc(CODEC2,24);
            }
            break;

        case 16000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0xc);
                config_codec_bclk_adc(CODEC1,12);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0xc);
                config_codec_bclk_adc(CODEC2,12);
            }
            break;

        case 32000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x6);
                config_codec_bclk_adc(CODEC1,6);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x6);
                config_codec_bclk_adc(CODEC2,6);
            }
            break;

        case 64000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_3);
                config_codec_adc_analog_mclk(CODEC1,0x8);
                config_codec_bclk_adc(CODEC1,4);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_3);
                config_codec_adc_analog_mclk(CODEC2,0x8);
                config_codec_bclk_adc(CODEC2,4);
            }
            break;

        case 12000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x10);
                config_codec_bclk_adc(CODEC1,16);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x10);
                config_codec_bclk_adc(CODEC2,16);
            }
            break;

        case 24000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x8);
                config_codec_bclk_adc(CODEC1,8);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x8);
                config_codec_bclk_adc(CODEC2,8);
            }
            break;

        case 48000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x4);
                config_codec_bclk_adc(CODEC1,4);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x4);
                config_codec_bclk_adc(CODEC2,4);
            }
            break;

        case 96000:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_2);
                config_codec_adc_analog_mclk(CODEC1,0x8);
                config_codec_bclk_adc(CODEC1,4);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_2);
                config_codec_adc_analog_mclk(CODEC2,0x8);
                config_codec_bclk_adc(CODEC2,4);
            }
            break;

        case 11025:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x10);
                config_codec_bclk_adc(CODEC1,16);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x10);
                config_codec_bclk_adc(CODEC2,16);
            }
            break;

        case 22050:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x8);
                config_codec_bclk_adc(CODEC1,8);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x8);
                config_codec_bclk_adc(CODEC2,8);
            }
            break;

        case 44100:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC1,0x4);
                config_codec_bclk_adc(CODEC1,4);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_4);
                config_codec_adc_analog_mclk(CODEC2,0x4);
                config_codec_bclk_adc(CODEC2,4);
            }
            break;

        case 88200:
            if(codec == CODEC1){
                config_codec_mclk(CODEC1,CODEC_MCLK_APLL_1);
                config_codec_adc_analog_mclk(CODEC1,0x10);
                //full mask  need to add code here     ?z//zhugang
                config_codec_bclk_adc(CODEC1,2);
            }else if(codec == CODEC2){
                config_codec_mclk(CODEC2,CODEC_MCLK_APLL_1);
                config_codec_adc_analog_mclk(CODEC2,0x10);
                //full mask  need to add code here     ?z//zhugang
                config_codec_bclk_adc(CODEC2,2);
            }
    }

}
static void silan_codec_adc_linein_init(silan_codec_t codec,silan_codec_inport_t inport)
{
    if(codec == CODEC1){
        CODEC1_AD_R0 &= ~(AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC1_AD_R1 =     0x10;  // LINE IN
        if(inport != DIFF)
        {
            CODEC1_AD_R1 &= ~(SEL_DIFF2);
//            CODEC1_AD_R1 |= (SEL_VCOM|SEL_NVCOM);
            CODEC1_AD_R1 |= (SEL_VCOM);    //No, VCOM 20180307 zhugang
            CODEC1_AD_R6 = AUX_PGA_L(0x18); //6 db = 0.75*8     SINGLE-ENDED need to *2,hardware bug
            CODEC1_AD_R7 = AUX_PGA_R(0x18); //6 db
        }
        else
        {
            CODEC1_AD_R1 |= (SEL_DIFF2);
            CODEC1_AD_R1 &= ~(SEL_VCOM|SEL_NVCOM);
            CODEC1_AD_R6 = AUX_PGA_L(0x10); //0 db
            CODEC1_AD_R7 = AUX_PGA_R(0x10); //0 db
        }

    }else if(codec == CODEC2){
        CODEC2_AD_R0 &= ~(AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC2_AD_R1 =     0x10;  // LINE IN
        if(inport != DIFF)
        {
            CODEC2_AD_R1 &= ~(SEL_DIFF2);
//            CODEC2_AD_R1 |= (SEL_VCOM|SEL_NVCOM);
            CODEC2_AD_R1 |= (SEL_VCOM);
            CODEC2_AD_R6 = AUX_PGA_L(0x18); //6 db = 0.75*8  SINGLE-ENDED need to *2,hardware bug
            CODEC2_AD_R7 = AUX_PGA_R(0x18); //6 db
        }
        else
        {
            CODEC2_AD_R1 |= (SEL_DIFF2);
            CODEC2_AD_R1 &= ~(SEL_VCOM|SEL_NVCOM);
            CODEC2_AD_R6 = AUX_PGA_L(0x10); //0 db
            CODEC2_AD_R7 = AUX_PGA_R(0x10); //0 db
        }

    }
}
static void silan_codec_adc_auxin_init(silan_codec_t codec,silan_codec_inport_t inport)
{
    if(codec == CODEC1){
        CODEC1_AD_R0 &= ~(AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC1_AD_R1 =     0x10|SEL_IN(1);  // AUX IN
        if(inport != DIFF)
        {
            CODEC1_AD_R1 &= ~(SEL_DIFF2);
//            CODEC1_AD_R1 |= (SEL_VCOM|SEL_NVCOM);
            CODEC1_AD_R1 |= (SEL_VCOM);
            CODEC1_AD_R6 = AUX_PGA_L(0x18); //6 db = 0.75*8  SINGLE-ENDED need to *2,hardware bug
            CODEC1_AD_R7 = AUX_PGA_R(0x18); //6 db
        }
        else
        {
            CODEC1_AD_R1 |= (SEL_DIFF2);
            CODEC1_AD_R1 &= ~(SEL_VCOM|SEL_NVCOM);
            CODEC1_AD_R6 = AUX_PGA_L(0x10); //0 db
            CODEC1_AD_R7 = AUX_PGA_R(0x10); //0 db
        }

    }else if(codec == CODEC2){
        CODEC2_AD_R0 &= ~(AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC2_AD_R1 =     0x10|SEL_IN(1);  // AUX IN
        if(inport != DIFF)
        {
            CODEC2_AD_R1 &= ~(SEL_DIFF2);
//            CODEC2_AD_R1 |= (SEL_VCOM|SEL_NVCOM);
            CODEC2_AD_R1 |= (SEL_VCOM);
            CODEC2_AD_R6 = AUX_PGA_L(0x18); //6 db  0.75*8
            CODEC2_AD_R7 = AUX_PGA_R(0x18); //6 db
        }
        else
        {
            CODEC2_AD_R1 |= (SEL_DIFF2);
            CODEC2_AD_R1 &= ~(SEL_VCOM|SEL_NVCOM);
            CODEC2_AD_R6 = AUX_PGA_L(0x10); //0 db
            CODEC2_AD_R7 = AUX_PGA_R(0x10); //0 db
        }
    }
}
static void silan_codec_adc_fmin_init(silan_codec_t codec,silan_codec_inport_t inport)
{
    if(codec == CODEC1){
        CODEC1_AD_R0 &= ~(AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC1_AD_R1 =     0x10|SEL_IN(2);  // FM IN
        if(inport != DIFF)
        {
            CODEC1_AD_R1 &= ~(SEL_DIFF2);
//            CODEC1_AD_R1 |= (SEL_VCOM|SEL_NVCOM);
            CODEC1_AD_R1 |= (SEL_VCOM);
            CODEC1_AD_R6 = AUX_PGA_L(0x18); //6 db
            CODEC1_AD_R7 = AUX_PGA_R(0x18); //6 db
        }
        else
        {
            CODEC1_AD_R1 |= (SEL_DIFF2);
            CODEC1_AD_R1 &= ~(SEL_VCOM|SEL_NVCOM);
            CODEC1_AD_R6 = AUX_PGA_L(0x10); //0 db
            CODEC1_AD_R7 = AUX_PGA_R(0x10); //0 db
        }

    }else if(codec == CODEC2){
        CODEC2_AD_R0 &= ~(AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC2_AD_R1 =     0x10|SEL_IN(2);  // FMIN
        if(inport != DIFF)
        {
            CODEC2_AD_R1 &= ~(SEL_DIFF2);
//            CODEC2_AD_R1 |= (SEL_VCOM|SEL_NVCOM);
            CODEC2_AD_R1 |= (SEL_VCOM);
            CODEC2_AD_R6 = AUX_PGA_L(0x18); //+6 db
            CODEC2_AD_R7 = AUX_PGA_R(0x18); //+6 db
        }
        else
        {
            CODEC2_AD_R1 |= (SEL_DIFF2);
            CODEC2_AD_R1 &= ~(SEL_VCOM|SEL_NVCOM);
            CODEC2_AD_R6 = AUX_PGA_L(0x10); //0 db
            CODEC2_AD_R7 = AUX_PGA_R(0x10); //0 db
        }

    }
}

static void silan_codec_adc_micin_init(silan_codec_t codec,silan_codec_inport_t inport)
{
    uint8_t tmp;
    if(codec == CODEC1){
        CODEC1_AD_R0 |= (AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC1_AD_R1 =     0x10|SEL_IN(3);  // MIC IN
        if(inport != DIFF){
            tmp = CODEC1_AD_R1;
            tmp &= ~SEL_DIFF1(3);
            tmp |= (SEL_DIFF1(1)|SEL_NVCOM);  //Support P IN
            CODEC1_AD_R1 = tmp;
        }
        else
        {
            tmp = CODEC1_AD_R1;
            tmp &= ~(SEL_DIFF1(3)|SEL_NVCOM);
            CODEC1_AD_R1 = tmp;
        }
        CODEC1_AD_R6 = MIC_BOOST_L(0x0)|MIC_PGA_L(0x8); //0 db
        CODEC1_AD_R7 = MIC_BOOST_R(0x0)|MIC_PGA_R(0x8); //0 db
    }else if(codec == CODEC2){
        CODEC2_AD_R0 |= (AD_POWUP_MICBIAS    \
                |AD_POWUP_MIC) ;
        CODEC2_AD_R1 =     0x10|SEL_IN(3);  // MIC IN
        if(inport != DIFF){
            tmp = CODEC2_AD_R1;
            tmp &= ~SEL_DIFF1(3);
            tmp |= (SEL_DIFF1(1)|SEL_NVCOM);  //Support P IN
            CODEC2_AD_R1 = tmp;
        }
        else
        {
            tmp = CODEC2_AD_R1;
            tmp &= ~(SEL_DIFF1(3)|SEL_NVCOM);
            CODEC2_AD_R1 = tmp;
        }
        CODEC2_AD_R6 = MIC_BOOST_L(0x0)|MIC_PGA_L(0x8); //0 db
        CODEC2_AD_R7 = MIC_BOOST_R(0x0)|MIC_PGA_R(0x8); //0 db
    }
}

void silan_codec1_adc_analog_init(silan_codec_adc_ch_t channel,silan_codec_inport_t inport_x)
{
    switch(channel)
    {
        case MIC_IN:
            silan_codec_adc_micin_init(CODEC1,inport_x);
            break;
        case LINE_IN:
            silan_codec_adc_linein_init(CODEC1,inport_x);
            break;
        case AUX_IN:
            silan_codec_adc_auxin_init(CODEC1,inport_x);
            break;
        case FM_IN:
            silan_codec_adc_fmin_init(CODEC1,inport_x);
            break;
    }
}

void silan_codec2_adc_analog_init(silan_codec_adc_ch_t channel,silan_codec_inport_t inport_x)
{
    switch(channel)
    {
        case MIC_IN:
            silan_codec_adc_micin_init(CODEC2,inport_x);
            break;
        case LINE_IN:
            silan_codec_adc_linein_init(CODEC2,inport_x);
            break;
        case AUX_IN:
            silan_codec_adc_auxin_init(CODEC2,inport_x);
            break;
        case FM_IN:
            silan_codec_adc_fmin_init(CODEC2,inport_x);
            break;
    }
}

int set_adc1_left_channel_digital_gain(int gain)
{
    if(gain >0x17)
        return -1;
    CODEC1_R5E = AD_PGA_L(gain);
    return 0;
}

int set_adc1_right_channel_digital_gain(int gain)
{
    if(gain >0x17)
        return -1;
    CODEC1_R5F = AD_PGA_R(gain);
    return 0;
}

int set_adc2_left_channel_digital_gain(int gain)
{
    if(gain >0x17)
        return -1;
    CODEC2_R5E = AD_PGA_L(gain);
    return 0;
}

int set_adc2_right_channel_digital_gain(int gain)
{
    if(gain >0x17)
        return -1;
    CODEC2_R5F = AD_PGA_R(gain);
    return 0;
}

void silan_codec1_adc_digital_init(silan_codec_role_t mode,silan_codec_iis_w_t bits,uint32_t fs)
{
    SL_DBGLOG("adc1 master=%d\n", mode);
    switch(bits)
    {
        case IIS_16BIT:
            //#ifdef _IIS_16BIT_
            CODEC1_R41 &= ~AD_IWL(3);
            break;
        case IIS_18BIT:
            //#elif  _IIS_18BIT_
            CODEC1_R41 &= ~AD_IWL(3);
            CODEC1_R41 |= AD_IWL(1);
            break;
        case IIS_20BIT:
            //#elif  _IIS_20BIT_
            CODEC1_R41 &= ~AD_IWL(3);
            CODEC1_R41 |= AD_IWL(2);
            break;
        case IIS_24BIT:
            //#elif  _IIS_24BIT_
            CODEC1_R41 &= ~AD_IWL(3);
            CODEC1_R41 |= AD_IWL(3);
            break;
        default:
            CODEC1_R41 &= ~AD_IWL(3);
            break;
    }

    //CODEC1_R42 = 0x00;

    if(mode&0x1)
        CODEC1_R47 |= AD_DAI_MASTER;
    else
        CODEC1_R47 &= ~(AD_DAI_MASTER);

    CODEC1_R48 = AD_LRCDIV_L(fs%0x100);
    CODEC1_R49 = AD_LRCDIV_H((fs/0x100));

}

void silan_codec2_adc_digital_init(silan_codec_role_t mode,silan_codec_iis_w_t bits,uint32_t fs)
{
    SL_DBGLOG("adc2 master=%d\n", mode);
    switch(bits)
    {
        case IIS_16BIT:
            //#ifdef _IIS_16BIT_
            CODEC2_R41 &= ~AD_IWL(3);
            break;
        case IIS_18BIT:
            //#elif  _IIS_18BIT_
            CODEC2_R41 &= ~AD_IWL(3);
            CODEC2_R41 |= AD_IWL(1);
            break;
        case IIS_20BIT:
            //#elif  _IIS_20BIT_
            CODEC2_R41 &= ~AD_IWL(3);
            CODEC2_R41 |= AD_IWL(2);
            break;
        case IIS_24BIT:
            //#elif  _IIS_24BIT_
            CODEC2_R41 &= ~AD_IWL(3);
            CODEC2_R41 |= AD_IWL(3);
            break;
        default:
            CODEC2_R41 &= ~AD_IWL(3);
            break;
    }

    //CODEC1_R42 = 0x00;

    if(mode&0x1)
        CODEC2_R47 |= AD_DAI_MASTER;
    else
        CODEC2_R47 &= ~(AD_DAI_MASTER);

    CODEC2_R48 = AD_LRCDIV_L(fs%0x100);
    CODEC2_R49 = AD_LRCDIV_H((fs/0x100));

}

void silan_codec1_dac_digital_init(silan_codec_role_t mode,silan_codec_iis_w_t bits,uint32_t fs)
{
    SL_DBGLOG("dac1 master=%d\n", mode);
    switch(bits)
    {
        case IIS_16BIT:
            //#ifdef _IIS_16BIT_
            CODEC1_R2 &= ~DA_IWL(3);
            break;
        case IIS_18BIT:
            //#elif  _IIS_18BIT_
            CODEC1_R2 &= ~DA_IWL(3);
            CODEC1_R2 |= DA_IWL(1);
            break;
        case IIS_20BIT:
            //#elif  _IIS_20BIT_
            CODEC1_R2 &= ~DA_IWL(3);
            CODEC1_R2 |= DA_IWL(2);
            break;
        case IIS_24BIT:
            //#elif  _IIS_24BIT_
            CODEC1_R2 &= ~DA_IWL(3);
            CODEC1_R2 |= DA_IWL(3);
            break;
        default:
            CODEC1_R2 &= ~DA_IWL(3);
            break;
    }

    if(mode&0x1)
        CODEC1_RC |= DA_DAI_MASTER;
    else
        CODEC1_RC &= ~(DA_DAI_MASTER);

    CODEC1_RD = DA_LRCDIV_L(fs%0x100);
    CODEC1_RE = DA_LRCDIV_H((fs/0x100));

}

void silan_codec2_dac_digital_init(silan_codec_role_t mode,silan_codec_iis_w_t bits,uint32_t fs)
{
    SL_DBGLOG("dac2 master=%d\n", mode);
    switch(bits)
    {
        case IIS_16BIT:
            //#ifdef _IIS_16BIT_
            CODEC2_R2 &= ~DA_IWL(3);
            break;
        case IIS_18BIT:
            //#elif  _IIS_18BIT_
            CODEC2_R2 &= ~DA_IWL(3);
            CODEC2_R2 |= DA_IWL(1);
            break;
        case IIS_20BIT:
            //#elif  _IIS_20BIT_
            CODEC2_R2 &= ~DA_IWL(3);
            CODEC2_R2 |= DA_IWL(2);
            break;
        case IIS_24BIT:
            //#elif  _IIS_24BIT_
            CODEC2_R2 &= ~DA_IWL(3);
            CODEC2_R2 |= DA_IWL(3);
            break;
        default:
            CODEC2_R2 &= ~DA_IWL(3);
            break;
    }

    if(mode&0x1)
        CODEC2_RC |= DA_DAI_MASTER;
    else
        CODEC2_RC &= ~(DA_DAI_MASTER);

    CODEC2_RD = DA_LRCDIV_L(fs%0x100);
    CODEC2_RE = DA_LRCDIV_H((fs/0x100));

}
int set_dac1_left_channel_analog_gain(int gain)
{
    if(gain > 0x7f)
        return -1;
    CODEC1_DA_R8 = DA_PGA_L(gain);
    return 0;
}

int set_dac1_right_channel_analog_gain(int gain)
{
    if(gain > 0x7f)
        return -1;
    CODEC1_DA_R9 = DA_PGA_R(gain);
    return 0;
}
int set_dac2_left_channel_analog_gain(int gain)
{
    if(gain > 0x7f)
        return -1;
    CODEC2_DA_R8 = DA_PGA_L(gain);
    return 0;
}

int set_dac2_right_channel_analog_gain(int gain)
{
    if(gain > 0x7f)
        return -1;
    CODEC2_DA_R9 = DA_PGA_R(gain);
    return 0;
}

int set_adc1_left_channel_mic_analog_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x1f)
        return -1;
    tmp = CODEC1_AD_R6;
    tmp    &= ~MIC_PGA_L(0x1f);
    tmp |= MIC_PGA_L(gain);
    CODEC1_AD_R6 = tmp;
    return 0;
}

int set_adc1_right_channel_mic_analog_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x1f)
        return -1;
    tmp = CODEC1_AD_R7;
    tmp    &= ~MIC_PGA_R(0x1f);
    tmp |= MIC_PGA_R(gain);
    CODEC1_AD_R7 = tmp;
    return 0;
}
int set_adc2_left_channel_mic_analog_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x1f)
        return -1;
    tmp = CODEC2_AD_R6;
    tmp    &= ~MIC_PGA_L(0x1f);
    tmp |= MIC_PGA_L(gain);
    CODEC2_AD_R6 = tmp;
    return 0;
}

int set_adc2_right_channel_mic_analog_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x1f)
        return -1;
    tmp = CODEC2_AD_R7;
    tmp    &= ~MIC_PGA_R(0x1f);
    tmp |= MIC_PGA_R(gain);
    CODEC2_AD_R7 = tmp;
    return 0;
}
int set_adc1_left_channel_other_analog_gain(int gain)
{
    if(gain > 0x3f)
        return -1;
    CODEC1_AD_R6 = AUX_PGA_L(gain);
    return 0;
}

int set_adc1_right_channel_other_analog_gain(int gain)
{
    if(gain > 0x3f)
        return -1;
    CODEC1_AD_R7 = AUX_PGA_R(gain);
    return 0;
}
int set_adc2_left_channel_other_analog_gain(int gain)
{
    if(gain > 0x3f)
        return -1;
    CODEC2_AD_R6 = AUX_PGA_L(gain);
    return 0;
}

int set_adc2_right_channel_other_analog_gain(int gain)
{
    if(gain > 0x3f)
        return -1;
    CODEC2_AD_R7 = AUX_PGA_R(gain);
    return 0;
}
int set_adc1_left_channel_micboost_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x3)
        return -1;
    tmp = CODEC1_AD_R6;
    tmp    &= ~MIC_BOOST_L(0x3);
    tmp |= MIC_BOOST_L(gain);
    CODEC1_AD_R6 = tmp;
    return 0;
}

int set_adc1_right_channel_micboost_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x3)
        return -1;
    tmp = CODEC1_AD_R7;
    tmp    &= ~MIC_BOOST_R(0x3);
    tmp |= MIC_BOOST_R(gain);
    CODEC1_AD_R7 = tmp;
    return 0;
}
int set_adc2_left_channel_micboost_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x3)
        return -1;
    tmp = CODEC2_AD_R6;
    tmp    &= ~MIC_BOOST_L(0x3);
    tmp |= MIC_BOOST_L(gain);
    CODEC2_AD_R6 = tmp;
    return 0;
}

int set_adc2_right_channel_micboost_gain(int gain)
{
    uint8_t tmp;
    if(gain > 0x3)
        return -1;
    tmp = CODEC2_AD_R7;
    tmp    &= ~MIC_BOOST_R(0x3);
    tmp |= MIC_BOOST_R(gain);
    CODEC2_AD_R7 = tmp;
    return 0;
}
#if 0
static void silan_codec1_config_para_load()  //default config
{
    // adc record table4-6
    CODEC1_R47 = 0x08;      // [6:1]ADC BCLKDIV [0]MASTER
    CODEC1_DA_RF = 0x40;    // [7]内部MCLK [6]内部时钟DAI [2:0]MCLK分频
    CODEC1_DA_R3 = 0x9f;    // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    CODEC1_DA_R4 = 0x00;    // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    CODEC1_R1 = 0x00;            // [1]增加一个时钟延时 [0]1=ACLK 0=MCLK1
    CODEC1_DA_R0 = 0x00;    // [7](DAC) 摸拟Pypass adc->dac
    CODEC1_RB  = 0x00;        // [1] LOOPBACK
    CODEC1_R40 = 0x04;
    CODEC1_R40 = 0x00;
    CODEC1_R41 = 0x0e;
    CODEC1_R42 = 0x2c;      // [5:2] ADC 高通 [1]offset [0]ADC HP enable
    CODEC1_R43 = 0x06;
    CODEC1_R44 = 0x90;
    CODEC1_R44 = 0x10;
    CODEC1_R45 = 0x00;
    CODEC1_R47 =0x0d;          //0x08; bit0 :0 slave  ;1 master
    CODEC1_R48 = 0x40;
    CODEC1_R49 = 0x00;
    //CODEC1_R57 =    0x00;  // [1]ADC_MUTE  [0]MIC_MUTE
    //CODEC1_AD_R0 = 0xff; // 0xFF(mic) 0xbd(aux/fm/line)
    CODEC1_AD_R0 = 0x9d; // [7]电源保护 [6]BIAS启动 [4]内部电源启动 [3]参考电流电压启动[2]ADC启动[1]MIC输入启动[0]内部电源时钟启动
    CODEC1_AD_R1 = 0x16; // 0x1C(mic) 0x14(aux)
    CODEC1_AD_R2 = 0x3c; // cienho
    CODEC1_AD_R3 = 0x00; // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    CODEC1_AD_R4 = 0x18; //0x78; // cienho
    CODEC1_AD_R6 = 0x10; // 0xFE(real mic)  [7:6]MIC_BOOST_L [5:1]MIC_PGA_L/[5:0]AUX_PGA_L  01000=0db 11111=+34.5db
    CODEC1_AD_R7 = 0x10; // 0xFE(real mic)    [7:6]MIC_BOOST_R [5:1]MIC_PGA_R/[5:0]AUX_PGA_R  01000=0db 11111=+34.5db
    CODEC1_R5E = 0x00;
    CODEC1_R5F = 0x00;
    CODEC1_R40= 0x03;
    CODEC1_AD_RF = 0x00;
    CODEC1_AD_R9 = 0x38;
    CODEC1_AD_R10 = 0x74;
    CODEC1_DA_RF = 0x40;

    // dac playback table3-6
    CODEC1_R1 = 0x04;
    CODEC1_R1 = 0x00;
    CODEC1_R2 = 0x0E;                // [5]DAC左右交换 [4]DAC左右反转 [3:2]数据长度00=16bit 01=18bit 10=20bit 11=24bit
    CODEC1_R3 = 0x04; //0x04;                //    [6] BCLK反转 [5:0] MCLK除数
    CODEC1_R4 = 0x00;
    CODEC1_R5 = 0x80;                // [7:0]采样率
    CODEC1_R6 = 0x81;                // [3：0]采样率高4位 [7:4]MCLK分频
    CODEC1_R9 = 0x86;                //  [0]DACMUTE
    CODEC1_RC =    0x08;              // [6:1] DAC BCLK DIV    [0] DAC MASTER MODE
    CODEC1_RD = 0x40;
    CODEC1_RE = 0x00;
    CODEC1_DA_R0 = 0x3f;        // [7](DAC) 摸拟Pypass adc->dac
    CODEC1_DA_R1 = 0x82;
    CODEC1_DA_R2 = 0xc7;
    CODEC1_DA_R3 = 0x9f;             // [0](ADC) 摸拟Pypass adc->dac
    CODEC1_DA_R4 = 0x00;            // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    CODEC1_DA_R5 = 0x44;//0x22;
    CODEC1_DA_R6 = 0x2c;
    CODEC1_DA_R8 = 0x5c;            // DAC_PGA_L[6:0]  000000=-72db    111111=23.25db
    CODEC1_DA_R9 = 0x5c;            // DAC_PGA_R[6:0]  000000=-72db    111111=23.25db
    CODEC1_DA_RF = 0x40;
    CODEC1_R4 = 0x00;
    CODEC1_R1 = 0x03;
    CODEC1_R36 = 0xed;
    //    CODEC1_RB =  0x02;            // [1] LOOPBACK
}

static void silan_codec2_config_para_load()  //default config
{
    // adc record table4-6
    CODEC2_R47 = 0x08;      // [6:1]ADC BCLKDIV [0]MASTER
    CODEC2_DA_RF = 0x40;    // [7]内部MCLK [6]内部时钟DAI [2:0]MCLK分频
    CODEC2_DA_R3 = 0x9f;    // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    CODEC2_DA_R4 = 0x00;    // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    CODEC2_R1 = 0x00;            // [1]增加一个时钟延时 [0]1=ACLK 0=MCLK1
    CODEC2_DA_R0 = 0x00;    // [7](DAC) 摸拟Pypass adc->dac
    CODEC2_RB  = 0x00;        // [1] LOOPBACK
    CODEC2_R40 = 0x04;
    CODEC2_R40 = 0x00;
    CODEC2_R41 = 0x0e;
    CODEC2_R42 = 0x2c;      // [5:2] ADC 高通 [1]offset [0]ADC HP enable
    CODEC2_R43 = 0x06;
    CODEC2_R44 = 0x90;
    CODEC2_R44 = 0x10;
    CODEC2_R45 = 0x00;
    CODEC2_R47 =0x0d;          //0x08; bit0 :0 slave  ;1 master
    CODEC2_R48 = 0x40;
    CODEC2_R49 = 0x00;
    //CODEC2_R57 =    0x00;  // [1]ADC_MUTE  [0]MIC_MUTE
    //CODEC2_AD_R0 = 0xff; // 0xFF(mic) 0xbd(aux/fm/line)
    CODEC2_AD_R0 = 0x9d; // [7]电源保护 [6]BIAS启动 [4]内部电源启动 [3]参考电流电压启动[2]ADC启动[1]MIC输入启动[0]内部电源时钟启动
    CODEC2_AD_R1 = 0x16; // 0x1C(mic) 0x14(aux)
    CODEC2_AD_R2 = 0x3c; // cienho
    CODEC2_AD_R3 = 0x00; // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    CODEC2_AD_R4 = 0x18; //0x78; // cienho
    CODEC2_AD_R6 = 0x10; // 0xFE(real mic)  [7:6]MIC_BOOST_L [5:1]MIC_PGA_L/[5:0]AUX_PGA_L  01000=0db 11111=+34.5db
    CODEC2_AD_R7 = 0x10; // 0xFE(real mic)    [7:6]MIC_BOOST_R [5:1]MIC_PGA_R/[5:0]AUX_PGA_R  01000=0db 11111=+34.5db
    CODEC2_R5E = 0x00;
    CODEC2_R5F = 0x00;
    CODEC2_R40= 0x03;
    CODEC2_AD_RF = 0x00;
    CODEC2_AD_R9 = 0x38;
    CODEC2_AD_R10 = 0x74;
    CODEC2_DA_RF = 0x40;

    // dac playback table3-6
    CODEC2_R1 = 0x04;
    CODEC2_R1 = 0x00;
    CODEC2_R2 = 0x0E;                // [5]DAC左右交换 [4]DAC左右反转 [3:2]数据长度00=16bit 01=18bit 10=20bit 11=24bit
    CODEC2_R3 = 0x04; //0x04;                //    [6] BCLK反转 [5:0] MCLK除数
    CODEC2_R4 = 0x00;
    CODEC2_R5 = 0x80;                // [7:0]采样率
    CODEC2_R6 = 0x81;                // [3：0]采样率高4位 [7:4]MCLK分频
    CODEC2_R9 = 0x86;                //  [0]DACMUTE
    CODEC2_RC =    0x08;              // [6:1] DAC BCLK DIV    [0] DAC MASTER MODE
    CODEC2_RD = 0x40;
    CODEC2_RE = 0x00;
    CODEC2_DA_R0 = 0x3f;        // [7](DAC) 摸拟Pypass adc->dac
    CODEC2_DA_R1 = 0x82;
    CODEC2_DA_R2 = 0xc7;
    CODEC2_DA_R3 = 0x9f;             // [0](ADC) 摸拟Pypass adc->dac
    CODEC2_DA_R4 = 0x00;            // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    CODEC2_DA_R5 = 0x44;//0x22;
    CODEC2_DA_R6 = 0x2c;
    CODEC2_DA_R8 = 0x5c;            // DAC_PGA_L[6:0]  000000=-72db    111111=23.25db
    CODEC2_DA_R9 = 0x5c;            // DAC_PGA_R[6:0]  000000=-72db    111111=23.25db
    CODEC2_DA_RF = 0x40;
    CODEC2_R4 = 0x00;
    CODEC2_R1 = 0x03;
    CODEC2_R36 = 0xed;
    //    CODEC2_RB =  0x02;            // [1] LOOPBACK
}
#endif
int silan_codec1_dac_mute()
{
    CODEC1_R9 |= DACMU;                //  [0]DACMUTE
    return 0;
}

int silan_codec2_dac_mute()
{
    CODEC2_R9 |= DACMU;                //  [0]DACMUTE
    return 0;
}
int silan_codec1_dac_unmute()
{
    CODEC1_R9 &= ~DACMU;                //  [0]DACMUTE
    return 0;
}

int silan_codec2_dac_unmute()
{
    CODEC2_R9 &= ~DACMU;                //  [0]DACMUTE
    return 0;
}

/*
   static void silan_codec1_config_para_load()  //default config
   {
// dac playback table3-6
CODEC1_R1 = 0x00;
CODEC1_R1 = 0x04;
CODEC1_R2 = 0x02;
CODEC1_R3 = 0x04;
CODEC1_R4 = 0x00;
CODEC1_R4 = 0x18;
CODEC1_R6 = 0xb1;
CODEC1_R9 = 0x86;
CODEC1_RB = 0x30;
CODEC1_RC = 0x09;
CODEC1_RD = 0x40;
CODEC1_RE = 0x00;
CODEC1_DA_R0 = 0x3f;
CODEC1_DA_R1 = 0x02;
CODEC1_DA_R2 = 0x5d;
CODEC1_DA_R3 = 0xdf;
CODEC1_DA_R4 = 0xb4;
CODEC1_DA_R5 = 0x66;
CODEC1_DA_R6 = 0xe2;
CODEC1_DA_R8 = 0x5c;
CODEC1_DA_R9 = 0x5c;
CODEC1_DA_RF = 0x40;
CODEC1_R4 = 0x00;
CODEC1_R1 = 0x03;

// adc record table4-6
CODEC1_R40 = 0x04;
CODEC1_R40 = 0x00;
CODEC1_R41 = 0x02;
CODEC1_R42 = 0x2c;
CODEC1_R43 = 0x04;
CODEC1_R44 = 0x90;
CODEC1_R44 = 0x10;
CODEC1_R45 = 0x00;
CODEC2_R47 = 0x09;//0x08; bit0 :0 slave  ;1 master
CODEC1_R48 = 0x40;
CODEC1_R49 = 0x00;
//CODEC1_AD_R0 = 0xff; // 0xFF(mic) 0xbd(aux/fm/line)
CODEC1_AD_R0 = 0xfd; // cienho
CODEC1_AD_R1 = 0x12; // 0x1C(mic) 0x14(aux)
CODEC1_AD_R2 = 0x18; // cienho
CODEC1_AD_R3 = 0x70;
CODEC1_AD_R4 = 0x18; // cienho
CODEC1_AD_R6 = 0x10; // 0xFE(real mic)
CODEC1_AD_R7 = 0x10; // 0xFE(real mic)
CODEC1_R5E = 0x00;
CODEC1_R5F = 0x00;
CODEC1_R40 = 0x03;
}
static void silan_codec2_config_para_load()  //default config
{
// dac playback table3-6
CODEC2_R1 = 0x00;
CODEC2_R1 = 0x04;
CODEC2_R2 = 0x02;
CODEC2_R3 = 0x04;
CODEC2_R4 = 0x00;
CODEC2_R4 = 0x18;
CODEC2_R6 = 0xb1;
CODEC2_R9 = 0x86;
CODEC2_RB = 0x30;
CODEC2_RC = 0x09;
CODEC2_RD = 0x40;
CODEC2_RE = 0x00;
CODEC2_DA_R0 = 0x3f;
CODEC2_DA_R1 = 0x02;
CODEC2_DA_R2 = 0x5d;
CODEC2_DA_R3 = 0xdf;
CODEC2_DA_R4 = 0xb4;
CODEC2_DA_R5 = 0x66;
CODEC2_DA_R6 = 0xe2;
CODEC2_DA_R8 = 0x5c;
CODEC2_DA_R9 = 0x5c;
CODEC2_DA_RF = 0x40;
CODEC2_R4 = 0x00;
CODEC2_R1 = 0x03;

// adc record table4-6
CODEC2_R40 = 0x04;
CODEC2_R40 = 0x00;
CODEC2_R41 = 0x02;
CODEC2_R42 = 0x2c;
CODEC2_R43 = 0x04;
CODEC2_R44 = 0x90;
CODEC2_R44 = 0x10;
CODEC2_R45 = 0x00;
CODEC2_R47 = 0x09; // 0x09(master)
CODEC2_R48 = 0x40;
CODEC2_R49 = 0x00;
//CODEC2_AD_R0 = 0xff; // 0xFF(mic) 0xbd(aux/fm/line)
CODEC2_AD_R0 = 0xfd; // cienho
CODEC2_AD_R1 = 0x12; // 0x1C(mic) 0x14(aux)
CODEC2_AD_R2 = 0x18; // cienho
CODEC2_AD_R3 = 0x70;
CODEC2_AD_R4 = 0x18; // cienho
CODEC2_AD_R6 = 0x10; // 0xFE(real mic)
CODEC2_AD_R7 = 0x10; // 0xFE(real mic)
CODEC2_R5E = 0x00;
CODEC2_R5F = 0x00;
CODEC2_R40 = 0x03;
}
*/
#if 0
static void codec_power_up_process(void)
{
    CODEC1_R9 = 0x87;
    CODEC2_R9 = 0x87;
    udelay(100000);
    CODEC1_DA_R3 = 0x9f;
    CODEC2_DA_R3 = 0x9f;
    udelay(5000);
    CODEC1_DA_R1 = 0x4a;
    CODEC2_DA_R1 = 0x4a;
    udelay(5000);
    CODEC1_DA_R6 = 0x2c;
    CODEC2_DA_R6 = 0x2c;
    udelay(10000);
    CODEC1_DA_R2 = 0x40;
    CODEC2_DA_R2 = 0x40;
    udelay(5000);
    CODEC1_DA_R4 = 0x02;
    CODEC2_DA_R4 = 0x02;
    udelay(5000);
    CODEC1_DA_R0 = 0x08;
    CODEC2_DA_R0 = 0x08;
    udelay(5000);
    CODEC1_DA_R0 = 0x28;
    CODEC2_DA_R0 = 0x28;
    udelay(20000);
    CODEC1_DA_R0 = 0x38;
    CODEC2_DA_R0 = 0x38;
    udelay(10000);
    CODEC1_DA_R4 = 0x16;
    CODEC2_DA_R4 = 0x16;
    udelay(10000);
    CODEC1_DA_R4 = 0x7e;
    CODEC2_DA_R4 = 0x7e;
    udelay(5000);
    CODEC1_DA_R1 = 0x42;
    CODEC2_DA_R1 = 0x42;
    udelay(10000);
    CODEC1_DA_R1 = 0x02;
    CODEC2_DA_R1 = 0x02;
    udelay(10000);
    CODEC1_DA_R2 = 0x41;
    CODEC2_DA_R2 = 0x41;
    udelay(10000);
    CODEC1_DA_R2 = 0x45;
    CODEC2_DA_R2 = 0x45;
    udelay(10000);
    CODEC1_DA_R2 = 0x47;
    CODEC2_DA_R2 = 0x47;
    udelay(10000);
    CODEC1_DA_R0 = 0x3c;
    CODEC2_DA_R0 = 0x3c;
    udelay(100000);
    CODEC1_DA_R2 = 0x40;
    CODEC2_DA_R2 = 0x40;
    udelay(250000);
    CODEC1_DA_R1 = 0x4A;
    CODEC2_DA_R1 = 0x4A;
    udelay(250000);
    CODEC1_DA_R4 = 0x16;
    CODEC2_DA_R4 = 0x16;
    udelay(750000);
    CODEC1_DA_R0 = 0x3f;
    CODEC2_DA_R0 = 0x3f;
    udelay(200000);
    CODEC1_DA_R1 = 0x46;
    CODEC2_DA_R1 = 0x46;
    udelay(10000);
    CODEC1_DA_R1 = 0x02;
    CODEC2_DA_R1 = 0x02;
    udelay(10000);
    CODEC1_DA_R4 = 0x00;
    CODEC2_DA_R4 = 0x00;
    udelay(10000);
    CODEC1_R47 = 0x08;
    CODEC2_R47 = 0x08;
    CODEC1_R2 = 0x0E;
    CODEC2_R2 = 0x0E;
    CODEC1_R3 = 0x04;
    CODEC2_R3 = 0x04;
    CODEC1_DA_R2 = 0xC7;
    CODEC2_DA_R2 = 0xC7;
    CODEC1_DA_R5 = 0x22;
    CODEC2_DA_R5 = 0x22;
    CODEC1_R5 = 0x00;
    CODEC2_R5 = 0x00;
    CODEC1_R6 = 0xB1;
    CODEC2_R6 = 0xB1;
    CODEC1_RD = 0x40;
    CODEC2_RD = 0x40;
    CODEC1_RE = 0x00;
    CODEC2_RE = 0x00;
    CODEC1_DA_R8 = 0x5C;
    CODEC2_DA_R8 = 0x5C;
    CODEC1_DA_R9 = 0x5C;
    CODEC2_DA_R9 = 0x5C;
    CODEC1_R36 = 0xED;
    CODEC2_R36 = 0xED;
    CODEC1_R9 = 0x86;
    CODEC2_R9 = 0x86;
    udelay(10000);
}
#endif

void silan_codec_dac_mute(silan_codec_t codec)
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    regs->R9 |= DACMU;
}

void silan_codec_dac_unmute(silan_codec_t codec)
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    regs->R9 &= ~DACMU;
}

int silan_codec_dac_set_gain(silan_codec_t codec, int l_gain, int r_gain)
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    if((l_gain > 0x7f) || (r_gain > 0x7F))
        return -1;
    regs->DA_R8 = DA_PGA_L(l_gain);
    regs->DA_R9 = DA_PGA_R(r_gain);
    return 0;
}

int silan_codec_dac_get_gain(silan_codec_t codec, int *l_gain, int *r_gain)
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    if(l_gain == NULL || r_gain == NULL) {
        return -1;
    }

    *l_gain = DA_PGA_L(regs->DA_R8);
    *r_gain = DA_PGA_L(regs->DA_R9);
    return 0;
}

int silan_codec_adc_set_gain(silan_codec_t codec, int l_gain, int r_gain)
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    if((l_gain > 0x17) || (r_gain > 0x17))
        return -1;

    regs->R5E = AD_PGA_L(l_gain);
    regs->R5F = AD_PGA_R(r_gain);
    return 0;
}

uint32_t silan_codec_check(silan_codec_t codec)
{
    return (have_opened[codec]);
}

static void silan_codec_power_up(silan_codec_t codec)
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    have_opened[codec] = 1;
    SL_LOG("CODEC_%d Power Up", codec+1);
    regs->R9 = 0x87;
    udelay(100000);
    regs->DA_R3 = 0x9f;
    udelay(5000);
    regs->DA_R1 = 0x4a;
    udelay(5000);
    regs->DA_R6 = 0x2c;
    udelay(10000);
    regs->DA_R2 = 0x40;
    udelay(5000);
    regs->DA_R4 = 0x02;
    udelay(5000);
    regs->DA_R0 = 0x08;
    udelay(5000);
    regs->DA_R0 = 0x28;
    udelay(20000);
    regs->DA_R0 = 0x38;
    udelay(10000);
    regs->DA_R4 = 0x16;
    udelay(10000);
    regs->DA_R4 = 0x7e;
    udelay(5000);
    regs->DA_R1 = 0x42;
    udelay(10000);
    regs->DA_R1 = 0x02;
    udelay(10000);
    regs->DA_R2 = 0x41;
    udelay(10000);
    regs->DA_R2 = 0x45;
    udelay(10000);
    regs->DA_R2 = 0x47;
    udelay(10000);
    regs->DA_R0 = 0x3c;
    udelay(100000);
    regs->DA_R2 = 0x40;
    udelay(250000);
    regs->DA_R1 = 0x4A;
    udelay(250000);
    regs->DA_R4 = 0x16;
    udelay(750000);
    regs->DA_R0 = 0x3f;
    udelay(200000);
    regs->DA_R1 = 0x46;
    udelay(10000);
    regs->DA_R1 = 0x02;
    udelay(10000);
    regs->DA_R4 = 0x00;
    udelay(10000);
    //regs->R47 = 0x08;  /* hge: fix dsp crash */
    regs->R2 = 0x0E;
    regs->R3 = 0x04;
    regs->DA_R2 = 0xC7;
    regs->DA_R5 = 0x22;
    regs->R5 = 0x00;
    regs->R6 = 0xB1;
    regs->RD = 0x40;
    regs->RE = 0x00;
    // regs->DA_R8 = 0x5C;
    // regs->DA_R9 = 0x5C;
    regs->R36 = 0xED;
    regs->R9 = 0x86;
    udelay(10000);
}

void silan_codec_power_down(silan_codec_t codec)
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    SL_LOG("CODEC_%d Power Down", codec+1);

    regs->DA_R4 = 0x02;
    udelay(10000);
    regs->DA_R1 = 0x4A;
    udelay(100000);
    regs->DA_R0 = 0x3C;
    udelay(30000);
    regs->DA_R2 = 0x40;
    udelay(100000);
    regs->DA_R6 = 0xA1;
    udelay(10000);
    regs->DA_R0 = 0x38;
    udelay(20000);
    regs->DA_R0 = 0x28;
    udelay(30000);
    regs->DA_R0 = 0x08;
    udelay(300000);
    regs->DA_R1 = 0x42;
    udelay(100000);
    regs->DA_R4 = 0x77;
    udelay(150000);
    regs->DA_R1 = 0x02;
    udelay(100000);
    regs->DA_R2 = 0x41;
    udelay(50000);
    regs->DA_R2 = 0x45;
    udelay(50000);
    regs->DA_R2 = 0x47;
    udelay(500000);
    regs->DA_R2 = 0x43;
    udelay(50000);
    regs->DA_R2 = 0x41;
    udelay(50000);
    regs->DA_R2 = 0x40;
    udelay(50000);
    regs->DA_R4 = 0x1D;
    udelay(100000);
    regs->DA_R1 = 0x4A;
    udelay(10000);
    regs->DA_R0 = 0x00;
    udelay(30000);
    regs->DA_R4 = 0x01;
    udelay(20000);
    regs->DA_R3 = 0x00;
    udelay(10000);
       have_opened[codec] = 0;
}

static void silan_codec_config_para_load(silan_codec_t codec)  //default config
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    // adc record table4-6
    regs->R47 = 0x08;      // [6:1]ADC BCLKDIV [0]MASTER
    regs->DA_RF = 0x40;    // [7]内部MCLK [6]内部时钟DAI [2:0]MCLK分频
    regs->DA_R3 = 0x9f;    // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    regs->DA_R4 = 0x00;    // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    regs->R1 = 0x00;            // [1]增加一个时钟延时 [0]1=ACLK 0=MCLK1
    regs->DA_R0 = 0x00;    // [7](DAC) 摸拟Pypass adc->dac
    regs->RB  = 0x00;        // [1] LOOPBACK
    regs->R40 = 0x04;
    regs->R40 = 0x00;
    regs->R41 = 0x0e;
    regs->R42 = 0x2c;      // [5:2] ADC 高通 [1]offset [0]ADC HP enable
    regs->R43 = 0x06;
    regs->R44 = 0x90;
    regs->R44 = 0x10;
    regs->R45 = 0x00;
    regs->R47 = 0x0d;          //0x08; bit0 :0 slave  ;1 master
    regs->R48 = 0x40;
    regs->R49 = 0x00;
    //CODEC1_R57 =    0x00;  // [1]ADC_MUTE  [0]MIC_MUTE
    //CODEC1_AD_R0 = 0xff; // 0xFF(mic) 0xbd(aux/fm/line)
    regs->AD_R0 = 0x9d; // [7]电源保护 [6]BIAS启动 [4]内部电源启动 [3]参考电流电压启动[2]ADC启动[1]MIC输入启动[0]内部电源时钟启动
    regs->AD_R1 = 0x16; // 0x1C(mic) 0x14(aux)
    regs->AD_R2 = 0x3c; // cienho
    regs->AD_R3 = 0x00; // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    regs->AD_R4 = 0x18; //0x78; // cienho
    regs->AD_R6 = 0x10; // 0xFE(real mic)  [7:6]MIC_BOOST_L [5:1]MIC_PGA_L/[5:0]AUX_PGA_L  01000=0db 11111=+34.5db
    regs->AD_R7 = 0x10; // 0xFE(real mic)    [7:6]MIC_BOOST_R [5:1]MIC_PGA_R/[5:0]AUX_PGA_R  01000=0db 11111=+34.5db
    regs->R5E = 0x00;
    regs->R5F = 0x00;
    regs->R40= 0x03;
    regs->AD_RF = 0x00;
    regs->AD_R9 = 0x38;
    regs->AD_R10 = 0x74;
    regs->DA_RF = 0x40;

    // dac playback table3-6
    regs->R1 = 0x04;
    regs->R1 = 0x00;
    regs->R2 = 0x0E;                // [5]DAC左右交换 [4]DAC左右反转 [3:2]数据长度00=16bit 01=18bit 10=20bit 11=24bit
    regs->R3 = 0x04; //0x04;                //    [6] BCLK反转 [5:0] MCLK除数
    regs->R4 = 0x00;
    regs->R5 = 0x80;                // [7:0]采样率
    regs->R6 = 0x81;                // [3：0]采样率高4位 [7:4]MCLK分频
    regs->R9 = 0x86;                //  [0]DACMUTE
    regs->RC = 0x08;              // [6:1] DAC BCLK DIV    [0] DAC MASTER MODE
    regs->RD = 0x40;
    regs->RE = 0x00;
    regs->DA_R0 = 0x3f;        // [7](DAC) 摸拟Pypass adc->dac
    regs->DA_R1 = 0x82;
    regs->DA_R2 = 0xc7;
    regs->DA_R3 = 0x9f;             // [0](ADC) 摸拟Pypass adc->dac
    regs->DA_R4 = 0x00;            // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    regs->DA_R5 = 0x44;//0x22;
    regs->DA_R6 = 0x2c;
    // regs->DA_R8 = 0x5f;            // DAC_PGA_L[6:0]  000000=-72db    111111=23.25db
    // regs->DA_R9 = 0x5f;            // DAC_PGA_R[6:0]  000000=-72db    111111=23.25db
    regs->DA_RF = 0x40;
    regs->R4 = 0x00;
    regs->R1 = 0x03;
    regs->R36 = 0xed;
    //    CODEC1_RB =  0x02;            // [1] LOOPBACK
}

static void silan_codec_adc_config_para_load(silan_codec_t codec)  //default config
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    regs->DA_R3 = 0x9f;    // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    regs->DA_R4 = 0x00;    // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    regs->RB  = 0x00;        // [1] LOOPBACK

    // adc record table4-6
    regs->R47 = 0x08;      // [6:1]ADC BCLKDIV [0]MASTER
    regs->R40 = 0x04;
    regs->R40 = 0x00;
    regs->R41 = 0x0e;
    regs->R42 = 0x2c;      // [5:2] ADC 高通 [1]offset [0]ADC HP enable
    regs->R43 = 0x06;
    regs->R44 = 0x90;
    regs->R44 = 0x10;
    regs->R45 = 0x00;
    regs->R47 =0x0d;          //0x08; bit0 :0 slave  ;1 master
    regs->R48 = 0x40;
    regs->R49 = 0x00;
    regs->AD_R0 = 0x9d; // [7]电源保护 [6]BIAS启动 [4]内部电源启动 [3]参考电流电压启动[2]ADC启动[1]MIC输入启动[0]内部电源时钟启动
    regs->AD_R1 = 0x16; // 0x1C(mic) 0x14(aux)
    regs->AD_R2 = 0x3c; // cienho
    regs->AD_R3 = 0x00; // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    regs->AD_R4 = 0x18; //0x78; // cienho
    regs->AD_R6 = 0x10; // 0xFE(real mic)  [7:6]MIC_BOOST_L [5:1]MIC_PGA_L/[5:0]AUX_PGA_L  01000=0db 11111=+34.5db
    regs->AD_R7 = 0x10; // 0xFE(real mic)    [7:6]MIC_BOOST_R [5:1]MIC_PGA_R/[5:0]AUX_PGA_R  01000=0db 11111=+34.5db
    regs->R5E = 0x00;
    regs->R5F = 0x00;
    regs->R40= 0x03;
    regs->AD_RF = 0x00;
    regs->AD_R9 = 0x38;
    regs->AD_R10 = 0x74;
}

static void silan_codec_dac_config_para_load(silan_codec_t codec)  //default config
{
    CODEC_CORE_REGS *regs;
    regs = codec_regs[codec];

    regs->DA_RF = 0x40;    // [7]内部MCLK [6]内部时钟DAI [2:0]MCLK分频
    regs->DA_R3 = 0x9f;    // [5:4]MICBIAS_V 00=1.4V 01=1.65V 10=1.75V 11=2.1V
    regs->DA_R4 = 0x00;    // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    regs->R1 = 0x00;            // [1]增加一个时钟延时 [0]1=ACLK 0=MCLK1
    regs->DA_R0 = 0x00;    // [7](DAC) 摸拟Pypass adc->dac
    regs->RB  = 0x00;        // [1] LOOPBACK

    // dac playback table3-6
    regs->R1 = 0x04;
    regs->R1 = 0x00;
    regs->R2 = 0x0E;                // [5]DAC左右交换 [4]DAC左右反转 [3:2]数据长度00=16bit 01=18bit 10=20bit 11=24bit
    regs->R3 = 0x04; //0x04;                //    [6] BCLK反转 [5:0] MCLK除数
    regs->R4 = 0x00;
    regs->R5 = 0x80;                // [7:0]采样率
    regs->R6 = 0x81;                // [3：0]采样率高4位 [7:4]MCLK分频
    regs->R9 = 0x86;                //  [0]DACMUTE
    regs->RC =    0x08;              // [6:1] DAC BCLK DIV    [0] DAC MASTER MODE
    regs->RD = 0x40;
    regs->RE = 0x00;
    regs->DA_R0 = 0x3f;        // [7](DAC) 摸拟Pypass adc->dac
    regs->DA_R1 = 0x82;
    regs->DA_R2 = 0xc7;
    regs->DA_R3 = 0x9f;             // [0](ADC) 摸拟Pypass adc->dac
    regs->DA_R4 = 0x00;            // [6:2]=电流调整  [1]enable [0]DAC输出下拉或开漏
    regs->DA_R5 = 0x44;//0x22;
    regs->DA_R6 = 0x2c;
    // regs->DA_R8 = 0x5f;            // DAC_PGA_L[6:0]  000000=-72db    111111=23.25db
    // regs->DA_R9 = 0x5f;            // DAC_PGA_R[6:0]  000000=-72db    111111=23.25db
    regs->DA_RF = 0x40;
    regs->R4 = 0x00;
    regs->R1 = 0x03;
    regs->R36 = 0xed;
}

void silan_codec_dac_init(silan_codec_t codec, CODEC_CFG *pcfg)
{
    CODEC_CORE_REGS *regs;
    uint8_t val;

    regs = codec_regs[codec];

    val  = regs->R2;
    val &= ~DA_IWL(3);
    val |= DA_IWL(pcfg->dac_wl);
    regs->R2 = val;

    if(pcfg->dac_master == CLK_MASTER)
        regs->R2 |= DA_DAI_MASTER;
    else
        regs->R2 &= ~(DA_DAI_MASTER);

    regs->RD = DA_LRCDIV_L(64%0x100);
    regs->RE = DA_LRCDIV_H((64/0x100));
}

void silan_codec_adc_init(silan_codec_t codec, CODEC_CFG *pcfg)
{
    CODEC_CORE_REGS *regs;
    uint8_t val;

    regs = codec_regs[codec];

    regs->AD_R1 = 0x10 | SEL_IN(pcfg->adc_sel);
    if(pcfg->adc_sel == CODEC_ADSEL_MIC) {
        regs->AD_R0 |= (AD_POWUP_MICBIAS | AD_POWUP_MIC);

        if(pcfg->adc_diff != 0){
            val = regs->AD_R1;
            val &= ~SEL_DIFF1(3);
            val |= (SEL_DIFF1(1)|SEL_NVCOM);  //Support P IN
            regs->AD_R1 = val;
        }
        else
        {
            val = regs->AD_R1;
            val &= ~(SEL_DIFF1(3)|SEL_NVCOM);
            regs->AD_R1 = val;
        }
        regs->AD_R6 = MIC_BOOST_L(pcfg->mic_boost)|MIC_PGA_L(0x8); //0 db
        regs->AD_R7 = MIC_BOOST_R(pcfg->mic_boost)|MIC_PGA_R(0x8); //0 db
    }
    else {
        regs->AD_R0 &= ~(AD_POWUP_MICBIAS | AD_POWUP_MIC);

        if(pcfg->adc_diff != CODEC_DIFF) {
            regs->AD_R1 &= ~(SEL_DIFF2);
            regs->AD_R1 |= (SEL_VCOM);
            regs->AD_R6 = AUX_PGA_L(0x18); //6 db = 0.75*8  SINGLE-ENDED need to *2,hardware bug
            regs->AD_R7 = AUX_PGA_R(0x18); //6 db
            regs->AD_R4 = 0x89;// 20190926 fix aux 采集THD低的问题
        }
        else {
            regs->AD_R1 |= (SEL_DIFF2);
            regs->AD_R1 &= ~(SEL_VCOM|SEL_NVCOM);
            regs->AD_R6 = AUX_PGA_L(0x10); //0 db
            regs->AD_R7 = AUX_PGA_R(0x10); //0 db
            regs->AD_R4 = 0x78; // 20190926 fix aux 采集THD低的问题
        }
    }

    val  = regs->R41;
    val &= ~AD_IWL(3);
    val |= AD_IWL(pcfg->adc_wl);
    regs->R41 = val;

    if(pcfg->adc_master == CLK_MASTER)
        regs->R47 |= AD_DAI_MASTER;
    else
        regs->R47 &= ~(AD_DAI_MASTER);

    regs->R48 = AD_LRCDIV_L(64%0x100);
    regs->R49 = AD_LRCDIV_H((64/0x100));
}

void silan_codec_adc_open(silan_codec_t codec)
{
    silan_codec_adc_config_para_load(codec);
}

void silan_codec_dac_open(silan_codec_t codec)
{
    silan_codec_power_up(codec);
    silan_codec_dac_config_para_load(codec);
}
void silan_codec_open(silan_codec_t codec)
{
    silan_codec_power_up(codec);
    silan_codec_config_para_load(codec);
}
void silan_codec_alc_on(silan_codec_t codec)
{
    volatile int delay = 1000;

    if(codec == CODEC1)
    {
        CODEC1_R54 = 0x38;
        CODEC1_R55 = 0x74;
        CODEC1_R53 = 0xa0;
        CODEC1_R54 = 0x38;
        CODEC1_R56 = 0x38;
        CODEC1_R57 = 0x4c;//0x4c;
        CODEC1_R58 = 0xc4;
        CODEC1_R59 = 0x0f;
        CODEC1_R5A = 0xc4;
        CODEC1_R5B = 0x0f;
        //CODEC1_R5E = 0x17;
        //CODEC1_R5F = 0x17;
        CODEC1_R55 = 0x3d;
        while(delay--);
        CODEC1_R54 = 0xa0;
    }
    else
    {
        CODEC2_R54 = 0x38;
        CODEC2_R55 = 0x74;
        CODEC2_R53 = 0xa0;
        CODEC2_R54 = 0x38;
        CODEC2_R56 = 0x38;
        CODEC2_R57 = 0x4c;//0x4c;
        CODEC2_R58 = 0xc4;
        CODEC2_R59 = 0x0f;
        CODEC2_R5A = 0xc4;
        CODEC2_R5B = 0x0f;
        //CODEC2_R5E = 0x17;
        //CODEC2_R5F = 0x17;
        CODEC2_R55 = 0x3d;
        while(delay--);
        CODEC2_R54 = 0xa0;

    }
}
void silan_codec_alc_off(silan_codec_t codec)
{
    if(codec == CODEC1)
        CODEC1_R54 = 0x38;
    else
        CODEC2_R54 = 0x38;
}

void silan_codec_alc_para_config(silan_codec_t codec,u8 alc_t_up,u8 alc_t_dn, \
                                                                 u8 alc_maxgain,u8 alc_mingain,u8 level)
{
    u8 tmp;

    sl_printf("alc config:%x %x %x %x %x %x\n", \
                   codec, \
                   alc_t_up, \
                   alc_t_dn, \
                   alc_maxgain, \
                   alc_mingain, \
                   level);
    if(codec == CODEC1)
    {
        CODEC1_R56 = (ALC_T_UP(alc_t_up) | ALC_T_UN(alc_t_dn));

        tmp = CODEC1_R54;
        tmp &= ~0x3f;
        tmp |= ALC_G_MAX(alc_maxgain);
        tmp |= ALC_G_MIN(alc_mingain);
        CODEC1_R54 = tmp;

        tmp = CODEC1_R55;
        tmp &= ~ALC_LVL(0xf);
        tmp |= ALC_LVL(level);
        CODEC1_R55 = tmp;
    }
    else
    {
        CODEC2_R56 = (ALC_T_UP(alc_t_up) | ALC_T_UN(alc_t_dn));

        tmp = CODEC2_R54;
        tmp &= ~0x3f;
        tmp |= ALC_G_MAX(alc_maxgain);
        tmp |= ALC_G_MIN(alc_mingain);
        CODEC2_R54 = tmp;

        tmp = CODEC2_R55;
        tmp &= ~ALC_LVL(0xf);
        tmp |= ALC_LVL(level);
        CODEC2_R55 = tmp;
    }
}

