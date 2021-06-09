#ifndef REG_ADC_TYPE_H_
#define REG_ADC_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t SFCR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t RESERVED0;
    volatile uint32_t JOFR1;
    volatile uint32_t JOFR2;
    volatile uint32_t JOFR3;
    volatile uint32_t JOFR4;
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t RSQR1;
    volatile uint32_t RSQR2;
    volatile uint32_t RESERVED1;
    volatile uint32_t RESERVED2;
    volatile uint32_t JSQR;
    volatile uint32_t SQLR;
    volatile uint32_t JDR1;
    volatile uint32_t JDR2;
    volatile uint32_t JDR3;
    volatile uint32_t JDR4;
    volatile uint32_t RESERVED3;
    volatile uint32_t RESERVED4;
    volatile uint32_t RESERVED5;
    volatile uint32_t RDR;
    volatile uint32_t CCR;
}reg_adc_t;

enum ADC_REG_SR_FIELD
{
    ADC_AWD_MASK = 0x1,
    ADC_AWD_POS = 0,
    ADC_REOC_MASK = 0x2,
    ADC_REOC_POS = 1,
    ADC_JEOC_MASK = 0x4,
    ADC_JEOC_POS = 2,
    ADC_OVR_MASK = 0x8,
    ADC_OVR_POS = 3,
    ADC_REOS_MASK = 0x10,
    ADC_REOS_POS = 4,
    ADC_JEOS_MASK = 0x20,
    ADC_JEOS_POS = 5,
    ADC_RSTRTC_MASK = 0x100,
    ADC_RSTRTC_POS = 8,
    ADC_JSTRTC_MASK = 0x200,
    ADC_JSTRTC_POS = 9,
};

enum ADC_REG_SFCR_FIELD
{
    ADC_AWDC_MASK = 0x1,
    ADC_AWDC_POS = 0,
    ADC_REOCC_MASK = 0x2,
    ADC_REOCC_POS = 1,
    ADC_JEOCC_MASK = 0x4,
    ADC_JEOCC_POS = 2,
    ADC_OVRC_MASK = 0x8,
    ADC_OVRC_POS = 3,
    ADC_REOSC_MASK = 0x10,
    ADC_REOSC_POS = 4,
    ADC_JEOSC_MASK = 0x20,
    ADC_JEOSC_POS = 5,
    ADC_RSTRTCC_MASK = 0x100,
    ADC_RSTRTCC_POS = 8,
    ADC_JSTRTCC_MASK = 0x200,
    ADC_JSTRTCC_POS = 9,
};

enum ADC_REG_CR1_FIELD
{
    ADC_AWDCH_MASK = 0x1f,
    ADC_AWDCH_POS = 0,
    ADC_REOCIE_MASK = 0x20,
    ADC_REOCIE_POS = 5,
    ADC_AWDIE_MASK = 0x40,
    ADC_AWDIE_POS = 6,
    ADC_JEOCIE_MASK = 0x80,
    ADC_JEOCIE_POS = 7,
    ADC_SCAN_MASK = 0x100,
    ADC_SCAN_POS = 8,
    ADC_AWDSGL_MASK = 0x200,
    ADC_AWDSGL_POS = 9,
    ADC_JAUTO_MASK = 0x400,
    ADC_JAUTO_POS = 10,
    ADC_RDISCEN_MASK = 0x800,
    ADC_RDISCEN_POS = 11,
    ADC_JDISCEN_MASK = 0x1000,
    ADC_JDISCEN_POS = 12,
    ADC_REOSIE_MASK = 0x2000,
    ADC_REOSIE_POS = 13,
    ADC_JEOSIE_MASK = 0x4000,
    ADC_JEOSIE_POS = 14,
    ADC_DISCNUM_MASK = 0xf0000,
    ADC_DISCNUM_POS = 16,
    ADC_JAWDEN_MASK = 0x400000,
    ADC_JAWDEN_POS = 22,
    ADC_RAWDEN_MASK = 0x800000,
    ADC_RAWDEN_POS = 23,
    ADC_RES_MASK = 0x3000000,
    ADC_RES_POS = 24,
    ADC_OVRIE_MASK = 0x4000000,
    ADC_OVRIE_POS = 26,
};

enum ADC_REG_CR2_FIELD
{
    ADC_ADEN_MASK = 0x1,
    ADC_ADEN_POS = 0,
    ADC_CONT_MASK = 0x2,
    ADC_CONT_POS = 1,
    ADC_DIFF_MASK = 0x4,
    ADC_DIFF_POS = 2,
    ADC_TEST_MASK = 0x8,
    ADC_TEST_POS = 3,
    ADC_DMAEN_MASK = 0x100,
    ADC_DMAEN_POS = 8,
    ADC_ALIGN_MASK = 0x800,
    ADC_ALIGN_POS = 11,
    ADC_JTRIG_MASK = 0x400000,
    ADC_JTRIG_POS = 22,
    ADC_EINBUF_MASK = 0x1000000,
    ADC_EINBUF_POS = 24,
    ADC_BINRES_MASK = 0x2000000,
    ADC_BINRES_POS = 25,
    ADC_BINBUF_MASK = 0x4000000,
    ADC_BINBUF_POS = 26,
    ADC_BATADJ_MASK = 0x30000000,
    ADC_BATADJ_POS = 28,
    ADC_RTRIG_MASK = 0x40000000,
    ADC_RTRIG_POS = 30,
    ADC_RDY_PH_MASK = (int)0x80000000,
    ADC_RDY_PH_POS = 31,
};

enum ADC_REG_SMPR1_FIELD
{
    ADC_SMP0_MASK = 0x3,
    ADC_SMP0_POS = 0,
    ADC_SMP1_MASK = 0xc,
    ADC_SMP1_POS = 2,
    ADC_SMP2_MASK = 0x30,
    ADC_SMP2_POS = 4,
    ADC_SMP3_MASK = 0xc0,
    ADC_SMP3_POS = 6,
    ADC_SMP4_MASK = 0x300,
    ADC_SMP4_POS = 8,
    ADC_SMP5_MASK = 0xc00,
    ADC_SMP5_POS = 10,
    ADC_SMP6_MASK = 0x3000,
    ADC_SMP6_POS = 12,
    ADC_SMP7_MASK = 0xc000,
    ADC_SMP7_POS = 14,
    ADC_SMP8_MASK = 0x30000,
    ADC_SMP8_POS = 16,
    ADC_SMP9_MASK = 0xc0000,
    ADC_SMP9_POS = 18,
    ADC_SMP10_MASK = 0x300000,
    ADC_SMP10_POS = 20,
    ADC_SMP11_MASK = 0xc00000,
    ADC_SMP11_POS = 22,
};

enum ADC_REG_JOFR1_FIELD
{
    ADC_JOFF1_MASK = 0xfff,
    ADC_JOFF1_POS = 0,
};

enum ADC_REG_JOFR2_FIELD
{
    ADC_JOFF2_MASK = 0xfff,
    ADC_JOFF2_POS = 0,
};

enum ADC_REG_JOFR3_FIELD
{
    ADC_JOFF3_MASK = 0xfff,
    ADC_JOFF3_POS = 0,
};

enum ADC_REG_JOFR4_FIELD
{
    ADC_JOFF4_MASK = 0xfff,
    ADC_JOFF4_POS = 0,
};

enum ADC_REG_HTR_FIELD
{
    ADC_HT_MASK = 0xfff,
    ADC_HT_POS = 0,
};

enum ADC_REG_LTR_FIELD
{
    ADC_LT_MASK = 0xfff,
    ADC_LT_POS = 0,
};

enum ADC_REG_RSQR1_FIELD
{
    ADC_RSQ1_MASK = 0xf,
    ADC_RSQ1_POS = 0,
    ADC_RSQ2_MASK = 0xf0,
    ADC_RSQ2_POS = 4,
    ADC_RSQ3_MASK = 0xf00,
    ADC_RSQ3_POS = 8,
    ADC_RSQ4_MASK = 0xf000,
    ADC_RSQ4_POS = 12,
    ADC_RSQ5_MASK = 0xf0000,
    ADC_RSQ5_POS = 16,
    ADC_RSQ6_MASK = 0xf00000,
    ADC_RSQ6_POS = 20,
    ADC_RSQ7_MASK = 0xf000000,
    ADC_RSQ7_POS = 24,
    ADC_RSQ8_MASK = (int)0xf0000000,
    ADC_RSQ8_POS = 28,
};

enum ADC_REG_RSQR2_FIELD
{
    ADC_RSQ9_MASK = 0xf,
    ADC_RSQ9_POS = 0,
    ADC_RSQ10_MASK = 0xf0,
    ADC_RSQ10_POS = 4,
    ADC_RSQ11_MASK = 0xf00,
    ADC_RSQ11_POS = 8,
    ADC_RSQ12_MASK = 0xf000,
    ADC_RSQ12_POS = 12,
};

enum ADC_REG_JSQR_FIELD
{
    ADC_JSQ1_MASK = 0xf,
    ADC_JSQ1_POS = 0,
    ADC_JSQ2_MASK = 0xf00,
    ADC_JSQ2_POS = 8,
    ADC_JSQ3_MASK = 0xf0000,
    ADC_JSQ3_POS = 16,
    ADC_JSQ4_MASK = 0xf000000,
    ADC_JSQ4_POS = 24,
};

enum ADC_REG_SQLR_FIELD
{
    ADC_RSQL_MASK = 0xf,
    ADC_RSQL_POS = 0,
    ADC_JSQL_MASK = 0x300,
    ADC_JSQL_POS = 8,
};

enum ADC_REG_JDR1_FIELD
{
    ADC_JDATA1_MASK = 0xffff,
    ADC_JDATA1_POS = 0,
};

enum ADC_REG_JDR2_FIELD
{
    ADC_JDATA2_MASK = 0xffff,
    ADC_JDATA2_POS = 0,
};

enum ADC_REG_JDR3_FIELD
{
    ADC_JDATA3_MASK = 0xffff,
    ADC_JDATA3_POS = 0,
};

enum ADC_REG_JDR4_FIELD
{
    ADC_JDATA4_MASK = 0xffff,
    ADC_JDATA4_POS = 0,
};

enum ADC_REG_RDR_FIELD
{
    ADC_RDATA_MASK = 0xffff,
    ADC_RDATA_POS = 0,
};

enum ADC_REG_CCR_FIELD
{
    ADC_CKDIV_MASK = 0x7,
    ADC_CKDIV_POS = 0,
    ADC_TRIM_EN_MASK = 0x8,
    ADC_TRIM_EN_POS = 3,
    ADC_VRPS_MASK = 0x70,
    ADC_VRPS_POS = 4,
    ADC_VREFEN_MASK = 0x80,
    ADC_VREFEN_POS = 7,
    ADC_VCMEN_MASK = 0x100,
    ADC_VCMEN_POS = 8,
    ADC_OFFCAL_MASK = 0x200,
    ADC_OFFCAL_POS = 9,
    ADC_LPCTL_MASK = 0x400,
    ADC_LPCTL_POS = 10,
    ADC_GAINCAL_MASK = 0x800,
    ADC_GAINCAL_POS = 11,
    ADC_MSBCAL_MASK = 0x3000,
    ADC_MSBCAL_POS = 12,
    ADC_BP_MASK = 0x4000,
    ADC_BP_POS = 14,
    ADC_VRBUFEN_MASK = 0x8000,
    ADC_VRBUFEN_POS = 15,
    ADC_GCALV_MASK = 0xff0000,
    ADC_GCALV_POS = 16,
    ADC_OCALV_MASK = (int)0xff000000,
    ADC_OCALV_POS = 24,
};

#endif
