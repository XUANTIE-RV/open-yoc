#ifndef _SILAN_CODEC_REGS_H_
#define _SILAN_CODEC_REGS_H_

#include "silan_types.h"
#if defined(__CC_ARM)
#include "silan_core_m0.h"
#endif

#define CODEC_BASE1           0x42061000
#define CODEC_BASE2           0x42062000

//#define SILAN_CODEC1_BASE     CODEC_BASE1
//#define SILAN_CODEC2_BASE     CODEC_BASE2

/*
 * DAC
*/
#define CODEC1_R1  __REG32(SILAN_CODEC1_BASE+0x02/2*4)
#define CODEC1_R2  __REG32(SILAN_CODEC1_BASE+0x04/2*4)
#define CODEC1_R3  __REG32(SILAN_CODEC1_BASE+0x06/2*4)
#define CODEC1_R4  __REG32(SILAN_CODEC1_BASE+0x08/2*4)
#define CODEC1_R5  __REG32(SILAN_CODEC1_BASE+0x0a/2*4)
#define CODEC1_R6  __REG32(SILAN_CODEC1_BASE+0x0c/2*4)
#define CODEC1_R9  __REG32(SILAN_CODEC1_BASE+0x12/2*4)
#define CODEC1_RB  __REG32(SILAN_CODEC1_BASE+0x16/2*4)
#define CODEC1_RC  __REG32(SILAN_CODEC1_BASE+0x18/2*4)
#define CODEC1_RD  __REG32(SILAN_CODEC1_BASE+0x1a/2*4)
#define CODEC1_RE  __REG32(SILAN_CODEC1_BASE+0x1c/2*4)
#define CODEC1_DA_R0  __REG32(SILAN_CODEC1_BASE+0x1e/2*4)
#define CODEC1_DA_R1  __REG32(SILAN_CODEC1_BASE+0x20/2*4)
#define CODEC1_DA_R2  __REG32(SILAN_CODEC1_BASE+0x22/2*4)
#define CODEC1_DA_R3  __REG32(SILAN_CODEC1_BASE+0x24/2*4)
#define CODEC1_DA_R4  __REG32(SILAN_CODEC1_BASE+0x26/2*4)
#define CODEC1_DA_R5  __REG32(SILAN_CODEC1_BASE+0x28/2*4)
#define CODEC1_DA_R6  __REG32(SILAN_CODEC1_BASE+0x2a/2*4)
#define CODEC1_DA_R8  __REG32(SILAN_CODEC1_BASE+0x2e/2*4)
#define CODEC1_DA_R9  __REG32(SILAN_CODEC1_BASE+0x30/2*4)
#define CODEC1_DA_RE  __REG32(SILAN_CODEC1_BASE+0x3a/2*4)
#define CODEC1_DA_RF  __REG32(SILAN_CODEC1_BASE+0x3c/2*4)
/*
 * ADC
*/
#define CODEC1_R36  __REG32(SILAN_CODEC1_BASE+0x6c/2*4)
#define CODEC1_R40  __REG32(SILAN_CODEC1_BASE+0x80/2*4)
#define CODEC1_R41  __REG32(SILAN_CODEC1_BASE+0x82/2*4)
#define CODEC1_R42  __REG32(SILAN_CODEC1_BASE+0x84/2*4)
#define CODEC1_R43  __REG32(SILAN_CODEC1_BASE+0x86/2*4)
#define CODEC1_R44  __REG32(SILAN_CODEC1_BASE+0x88/2*4)
#define CODEC1_R45  __REG32(SILAN_CODEC1_BASE+0x8a/2*4)
#define CODEC1_R47  __REG32(SILAN_CODEC1_BASE+0x8e/2*4)
#define CODEC1_R48  __REG32(SILAN_CODEC1_BASE+0x90/2*4)
#define CODEC1_R49  __REG32(SILAN_CODEC1_BASE+0x92/2*4)
#define CODEC1_R4B  __REG32(SILAN_CODEC1_BASE+0x96/2*4)
#define CODEC1_R53  __REG32(SILAN_CODEC1_BASE+0xa6/2*4)
#define CODEC1_R54  __REG32(SILAN_CODEC1_BASE+0xa8/2*4)
#define CODEC1_R55  __REG32(SILAN_CODEC1_BASE+0xaa/2*4)
#define CODEC1_R56  __REG32(SILAN_CODEC1_BASE+0xac/2*4)
#define CODEC1_R57  __REG32(SILAN_CODEC1_BASE+0xae/2*4)
#define CODEC1_R58  __REG32(SILAN_CODEC1_BASE+0xb0/2*4)
#define CODEC1_R59  __REG32(SILAN_CODEC1_BASE+0xb2/2*4)
#define CODEC1_R5A  __REG32(SILAN_CODEC1_BASE+0xb4/2*4)
#define CODEC1_R5B  __REG32(SILAN_CODEC1_BASE+0xb6/2*4)
#define CODEC1_R5E  __REG32(SILAN_CODEC1_BASE+0xbc/2*4)
#define CODEC1_R5F  __REG32(SILAN_CODEC1_BASE+0xbe/2*4)
#define CODEC1_R60  __REG32(SILAN_CODEC1_BASE+0xc0/2*4)
#define CODEC1_R65  __REG32(SILAN_CODEC1_BASE+0xca/2*4)
#define CODEC1_AD_R0  __REG32(SILAN_CODEC1_BASE+0x96/2*4)
#define CODEC1_AD_R1  __REG32(SILAN_CODEC1_BASE+0x98/2*4)
#define CODEC1_AD_R2  __REG32(SILAN_CODEC1_BASE+0x9a/2*4)
#define CODEC1_AD_R3  __REG32(SILAN_CODEC1_BASE+0x9c/2*4)
#define CODEC1_AD_R4  __REG32(SILAN_CODEC1_BASE+0x9e/2*4)
#define CODEC1_AD_R5  __REG32(SILAN_CODEC1_BASE+0xa0/2*4)
#define CODEC1_AD_R6  __REG32(SILAN_CODEC1_BASE+0xa2/2*4)
#define CODEC1_AD_R7  __REG32(SILAN_CODEC1_BASE+0xa4/2*4)
#define CODEC1_AD_R9  __REG32(SILAN_CODEC1_BASE+0xa8/2*4)
#define CODEC1_AD_R10  __REG32(SILAN_CODEC1_BASE+0xaa/2*4)
#define CODEC1_AD_RF  __REG32(SILAN_CODEC1_BASE+0xca/2*4)


/*
 * DAC
*/
#define CODEC2_R1  __REG32(SILAN_CODEC2_BASE+0x02/2*4)
#define CODEC2_R2  __REG32(SILAN_CODEC2_BASE+0x04/2*4)
#define CODEC2_R3  __REG32(SILAN_CODEC2_BASE+0x06/2*4)
#define CODEC2_R4  __REG32(SILAN_CODEC2_BASE+0x08/2*4)
#define CODEC2_R5  __REG32(SILAN_CODEC2_BASE+0x0a/2*4)
#define CODEC2_R6  __REG32(SILAN_CODEC2_BASE+0x0c/2*4)
#define CODEC2_R9  __REG32(SILAN_CODEC2_BASE+0x12/2*4)
#define CODEC2_RB  __REG32(SILAN_CODEC2_BASE+0x16/2*4)
#define CODEC2_RC  __REG32(SILAN_CODEC2_BASE+0x18/2*4)
#define CODEC2_RD  __REG32(SILAN_CODEC2_BASE+0x1a/2*4)
#define CODEC2_RE  __REG32(SILAN_CODEC2_BASE+0x1c/2*4)
#define CODEC2_DA_R0  __REG32(SILAN_CODEC2_BASE+0x1e/2*4)
#define CODEC2_DA_R1  __REG32(SILAN_CODEC2_BASE+0x20/2*4)
#define CODEC2_DA_R2  __REG32(SILAN_CODEC2_BASE+0x22/2*4)
#define CODEC2_DA_R3  __REG32(SILAN_CODEC2_BASE+0x24/2*4)
#define CODEC2_DA_R4  __REG32(SILAN_CODEC2_BASE+0x26/2*4)
#define CODEC2_DA_R5  __REG32(SILAN_CODEC2_BASE+0x28/2*4)
#define CODEC2_DA_R6  __REG32(SILAN_CODEC2_BASE+0x2a/2*4)
#define CODEC2_DA_R8  __REG32(SILAN_CODEC2_BASE+0x2e/2*4)
#define CODEC2_DA_R9  __REG32(SILAN_CODEC2_BASE+0x30/2*4)
#define CODEC2_DA_RE  __REG32(SILAN_CODEC2_BASE+0x3a/2*4)
#define CODEC2_DA_RF  __REG32(SILAN_CODEC2_BASE+0x3c/2*4)
/*
 * ADC
*/
#define CODEC2_R36  __REG32(SILAN_CODEC2_BASE+0x6c/2*4)
#define CODEC2_R40  __REG32(SILAN_CODEC2_BASE+0x80/2*4)
#define CODEC2_R41  __REG32(SILAN_CODEC2_BASE+0x82/2*4)
#define CODEC2_R42  __REG32(SILAN_CODEC2_BASE+0x84/2*4)
#define CODEC2_R43  __REG32(SILAN_CODEC2_BASE+0x86/2*4)
#define CODEC2_R44  __REG32(SILAN_CODEC2_BASE+0x88/2*4)
#define CODEC2_R45  __REG32(SILAN_CODEC2_BASE+0x8a/2*4)
#define CODEC2_R47  __REG32(SILAN_CODEC2_BASE+0x8e/2*4)
#define CODEC2_R48  __REG32(SILAN_CODEC2_BASE+0x90/2*4)
#define CODEC2_R49  __REG32(SILAN_CODEC2_BASE+0x92/2*4)
#define CODEC2_R4B  __REG32(SILAN_CODEC2_BASE+0x96/2*4)
#define CODEC2_R53  __REG32(SILAN_CODEC2_BASE+0xa6/2*4)
#define CODEC2_R54  __REG32(SILAN_CODEC2_BASE+0xa8/2*4)
#define CODEC2_R55  __REG32(SILAN_CODEC2_BASE+0xaa/2*4)
#define CODEC2_R56  __REG32(SILAN_CODEC2_BASE+0xac/2*4)
#define CODEC2_R57  __REG32(SILAN_CODEC2_BASE+0xae/2*4)
#define CODEC2_R58  __REG32(SILAN_CODEC2_BASE+0xb0/2*4)
#define CODEC2_R59  __REG32(SILAN_CODEC2_BASE+0xb2/2*4)
#define CODEC2_R5A  __REG32(SILAN_CODEC2_BASE+0xb4/2*4)
#define CODEC2_R5B  __REG32(SILAN_CODEC2_BASE+0xb6/2*4)
#define CODEC2_R5E  __REG32(SILAN_CODEC2_BASE+0xbc/2*4)
#define CODEC2_R5F  __REG32(SILAN_CODEC2_BASE+0xbe/2*4)
#define CODEC2_R60  __REG32(SILAN_CODEC2_BASE+0xc0/2*4)
#define CODEC2_R65  __REG32(SILAN_CODEC2_BASE+0xca/2*4)
#define CODEC2_AD_R0  __REG32(SILAN_CODEC2_BASE+0x96/2*4)
#define CODEC2_AD_R1  __REG32(SILAN_CODEC2_BASE+0x98/2*4)
#define CODEC2_AD_R2  __REG32(SILAN_CODEC2_BASE+0x9a/2*4)
#define CODEC2_AD_R3  __REG32(SILAN_CODEC2_BASE+0x9c/2*4)
#define CODEC2_AD_R4  __REG32(SILAN_CODEC2_BASE+0x9e/2*4)
#define CODEC2_AD_R5  __REG32(SILAN_CODEC2_BASE+0xa0/2*4)
#define CODEC2_AD_R6  __REG32(SILAN_CODEC2_BASE+0xa2/2*4)
#define CODEC2_AD_R7  __REG32(SILAN_CODEC2_BASE+0xa4/2*4)
#define CODEC2_AD_R9  __REG32(SILAN_CODEC2_BASE+0xa8/2*4)
#define CODEC2_AD_R10  __REG32(SILAN_CODEC2_BASE+0xaa/2*4)
#define CODEC2_AD_RF  __REG32(SILAN_CODEC2_BASE+0xca/2*4)
//R1
#define DA_SEL       (1)
#define DA_ACTIVE    (1<<1)
#define DA_RESET     (1<<2)
#define DA_MCLKINV   (1<<3)
//R2
#define DA_FORMAT(x) ((x&0x3))
#define DA_IWL(x)    ((x&0x3)<<2)
#define DA_LRP       (1<<4)
#define DA_LRSWAP    (1<<5)
//R3
#define DA_MCLKDIV(x)(x&0x3f)
#define DA_BCLKINV   (1<<6)
#define DA_CLKDIV2   (1<<7)
//R5
#define OSR_L(x)     ((x&0xff))
//R6
#define OSR_H(x)     ((x&0xf))
#define OSR_SEL(x)   ((x&0xf)<<4)
//R9
#define DACMU        (1)
//RB
#define BYP_SEL      (1<<1)
#define IN_SHIFT     ((x&0xf)<<3)
//RC
#define DA_BCLKDIV(x) ((x&0x3f)<<1)
#define DA_DAI_MASTER (1)
//RD
#define DA_LRCDIV_L(x)  (x&0xff)
#define DA_LRCDIV_H(x)  ((x&0xf))
//DA_R0
#define DA_S_ADBYP     (1<<7)
#define DA_POWUP_REGUS (1<<5)
#define DA_POWUP_DCK   (1<<4)
#define DA_POWUP_IREF  (1<<3)
#define DA_POWUP_DAC   (1<<2)
#define DA_POWUP_LINEOUT  (1)
//DA_R1
#define DA_ISB3_H  (1<<7)
#define SC1        (1<<1)
#define S_D_CLK    (1)
//DA_R2
#define DA_ISB3    ((x&0x3)<<6)
#define DA_ISB2    ((x&0x7)<<3)
#define DA_ISB1    ((x&0x7))

//DA_R3
#define SEL_RA(x)  ((x&0x7)<<5)
#define POW_25V    (1<<4)
#define S_VCOML    (1<<3)
#define S_DIO      (1<<2)
#define DA_POWUP_BG  (1<<1)
#define S_VREF1    (1)
//DA_R4
#define I_ANTIPOP  ((x&0x1f)<<2)
#define S_ANTIPOP2  (1<<1)
#define PDN0        (1)
//DA_R5
#define DA_SGAIN_L(x) ((x&0x3)<<5)
#define DA_SGAIN_R(x) ((x&0x3)<<1)
//DA_R8
#define DA_PGA_L(x) ((x&0x7f))
//DA_R9
#define DA_PGA_R(x) ((x&0x7f))
//DA_RF
#define DA_SEL_DAICLK   (1<<6)
#define DA_SEL_MCLK1K   (1<<7)
//R40
#define AD_LRSWAP    (1<<5)
#define AD_LRP       (1<<4)
#define AD_MCLKINV   (1<<3)
#define AD_RESET     (1<<2)
#define AD_ACTICE    (1<<1)
#define AD_SEL       (1)

//R41
#define AD_IWL(x)  ((x&0x3)<<2)
//R42
#define AD_CLKDIV2  (1<<7)
#define AD_BLCKINV  (1<<6)
#define HPCUTOFF(x) ((x&0xf)<<2)
#define HPOR        (1<<1)
#define ADCHPD      (1)

//R43
#define SINC_X16      (1<<7)
#define AD_MCLKDIV(x) ((x&0x3f))
//R44
#define SINC_RESET    (1<<7)
#define SINC_RATE     ((x&0x3f))
//R45
#define SINC_SHIFT     ((x&0xf)<<4)
//R47
#define AD_BCLKDIV(x) ((x*0x3f)<<1)
#define AD_DAI_MASTER  (1)
//R48
#define AD_LRCDIV_L(x)  (x&0xff)
//R49
#define AD_LRCDIV_H(x)  ((x&0xf)<<4)
//AD_R0
#define ANA_FLOATING       (1<<7)
#define AD_POWUP_MICBIAS   (1<<6)
#define AD_POWUP_NC        (1<<5)
#define AD_POWUP_REGUS     (1<<4)
#define AD_POWUP_IREF      (1<<3)
#define AD_POWUP_ADC       (1<<2)
#define AD_POWUP_MIC       (1<<1)
#define AD_POWUP_ACK       (1)
//AD_R1
#define SEL_NVCOM        (1<<7)
#define SEL_DIFF1(x)    ((x&0x3)<<5)
#define SEL_VCOM        (1<<5)
#define SEL_DIFF2       (1<<4)
#define SEL_IN(x)       ((x&0x3)<<2)
#define SC1             (1<<1)
#define S_D_CLK         (1)

//AD_R2
#define AD_SIB1(x)      ((x&0x3)<<5)
#define AD_SIB2(x)      ((x&0x3)<<2)
#define ISL1_IREF(x)    ((x&0x3))

//AD_R2
#define MIC_S_MDRV2(x)  (1<<6)
#define MIC_SEL_RA(x)   ((x&0x3)<<4)
#define MIC_ISL         (1<<3)
#define MIC_S_ADBYP     (1<<0)

/*
 *  AD_R6
 */
#define MIC_BOOST_L(x)  ((x&0x3)<<6)
#define MIC_PGA_L(x)    ((x&0x1f)<<1)
#define AUX_PGA_L(x)    ((x&0x3f))

/*
 *  AD_R7
 */
#define MIC_BOOST_R(x)   ((x&0x3)<<6)
#define MIC_PGA_R(x)    ((x&0x1f)<<1)
#define AUX_PGA_R(x)    ((x&0x3f))
// R5E
#define AD_PGA_L(x)     ((x&0x1f))
// R5F
#define AD_PGA_R(x)     ((x&0x1f))
// R57
#define AD_MU            (1<<1)
#define MIC_MUTE         (1)
// R54
#define ALC_SEL          (1<<7)
#define ALC_MODE         (1<<6)
#define ALC_G_MAX(x)     ((x&0x07)<<3)
#define ALC_G_MIN(x)     ((x&0x7))
// R55
#define ALC_LVL(x)       ((x&0x0f)<<4)
#define NG_TH(x)         ((x&0x7)<<1)
#define NG_EN

// R56
#define ALC_T_UP(x)       ((x&0x0f)<<4)
#define ALC_T_UN(x)       ((x&0xf))

// R55
#define ALC_HOLD(x)      ((x&0x0f)<<5)
#define ALC_ZC           (1<<3)
#define ALC_TIME_OUT     (1<<2)
#define ALC_MUTE         (1<<1)
#define MIC_MUTE         (1)

typedef struct CODEC_Core_regs
{
	__IO uint32_t		R0; // 0
	__IO uint32_t		R1; // 1
	__IO uint32_t		R2; // 2
	__IO uint32_t		R3; // 3
	__IO uint32_t		R4; // 4
	__IO uint32_t		R5; // 5
	__IO uint32_t		R6; // 6
	__IO uint32_t		R7; // 7
	__IO uint32_t		R8; // 8
	__IO uint32_t		R9; // 9
	__IO uint32_t		RA; // a
	__IO uint32_t		RB; // b
	__IO uint32_t		RC; // c
	__IO uint32_t		RD; // d
	__IO uint32_t		RE; // e
	__IO uint32_t		DA_R0; // f
	__IO uint32_t		DA_R1; // 10
	__IO uint32_t		DA_R2; // 11
	__IO uint32_t		DA_R3; // 12
	__IO uint32_t		DA_R4; // 13
	__IO uint32_t		DA_R5; // 14
	__IO uint32_t		DA_R6; // 15
	__IO uint32_t		DA_R7; // 16
	__IO uint32_t		DA_R8; // 17
	__IO uint32_t		DA_R9; // 18
	__IO uint32_t		DA_RA; // 19
	__IO uint32_t		DA_RB; // 1a
	__IO uint32_t		DA_RC; // 1b
	__IO uint32_t		DA_RD; // 1c
	__IO uint32_t		DA_RE; // 1d
	__IO uint32_t		DA_RF; // 1e
	__IO uint32_t       SP0[23];  // 1f~35
	__IO uint32_t		R36;   // 36
	__IO uint32_t		SP1[9];
	__IO uint32_t		R40;
	__IO uint32_t		R41;
	__IO uint32_t		R42;
	__IO uint32_t		R43;
	__IO uint32_t		R44;
	__IO uint32_t		R45;
	__IO uint32_t		R46;
	__IO uint32_t		R47;
	__IO uint32_t		R48;
	__IO uint32_t		R49;
	__IO uint32_t		R4A;
	__IO uint32_t		AD_R0;
	__IO uint32_t		AD_R1;
	__IO uint32_t		AD_R2;
	__IO uint32_t		AD_R3;
	__IO uint32_t		AD_R4;
	__IO uint32_t		AD_R5;
	__IO uint32_t		AD_R6;
	__IO uint32_t		AD_R7;
	__IO uint32_t		AD_R8;
	__IO uint32_t		AD_R9;
	__IO uint32_t		AD_R10;
	__IO uint32_t		R56;
	__IO uint32_t		R57;
	__IO uint32_t		R58;
	__IO uint32_t		R59;
	__IO uint32_t		R5A;
	__IO uint32_t		R5B;
	__IO uint32_t		R5C;
	__IO uint32_t		R5D;
	__IO uint32_t		R5E;
	__IO uint32_t		R5F;
	__IO uint32_t		R60;
	__IO uint32_t		SP3[4];
	__IO uint32_t		AD_RF; // R65
}
CODEC_CORE_REGS, *PCODEC_CORE_REGS;


#endif //_SILAN_CODEC_REGS_H_

