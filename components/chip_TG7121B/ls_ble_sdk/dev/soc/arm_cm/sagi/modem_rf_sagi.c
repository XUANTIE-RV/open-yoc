#include <stdint.h>
#include "reg_mdm2.h"
#include "reg_rf.h"
#include "field_manipulate.h"

#define SetWord32(a,d)      (* ( volatile uint32_t*)(a)=(d) )
#define GetWord32(a)        (* ( volatile uint32_t*)(a) )

uint32_t rf_rpl_reg_rd(uint16_t addr) {
    uint32_t rdat = 0;
    SetWord32(0x5001B004,0);
    SetWord32(0x5001B000,0x0B00|0x0000|addr);
    while(GetWord32(0x5001B014)!=1);
    while(GetWord32(0x5001B014)!=0);
    rdat = GetWord32(0x5001B010);
    return rdat;
}

void rf_rpl_reg_wr(uint16_t addr, uint32_t value) {
    SetWord32(0x5001B004,value);
    SetWord32(0x5001B000,0x0300|0xB000|addr);
    while(GetWord32(0x5001B014)!=1);
    while(GetWord32(0x5001B014)!=0);
    for(int i=0; i<10; i++) { __asm ("nop"); }
}

void rf_init_setting(void) {    
  rf_rpl_reg_wr(0x14, 0x000673ac);
  rf_rpl_reg_wr(0x02, 0x0f002460);
  rf_rpl_reg_wr(0x14, 0x000673fd);
  rf_rpl_reg_wr(0x00, 0xff7f013f);
  rf_rpl_reg_wr(0x01, 0x605a3317);
  rf_rpl_reg_wr(0x02, 0x07012a58);
  rf_rpl_reg_wr(0x03, 0x22e966d4);
  rf_rpl_reg_wr(0x04, 0xc0044005);
  rf_rpl_reg_wr(0x05, 0x80000000);
  rf_rpl_reg_wr(0x06, 0x50b00080);
  rf_rpl_reg_wr(0x07, 0x1f000980);
 // rf_rpl_reg_wr(0x08, 0x4c65824c);
	rf_rpl_reg_wr(0x08, 0x4c00824c);
  rf_rpl_reg_wr(0x09, 0x50752515);
  rf_rpl_reg_wr(0x0a, 0xaa32011c);
  rf_rpl_reg_wr(0x0b, 0xaa325f00);
  rf_rpl_reg_wr(0x0c, 0x00007444);
  rf_rpl_reg_wr(0x03, 0x24e966d2);
  rf_rpl_reg_wr(0x04, 0xc0244120);
  rf_rpl_reg_wr(0x00, 0xc77f0000);
	
	rf_rpl_reg_rd(0x00);
  rf_rpl_reg_rd(0x01);
  rf_rpl_reg_rd(0x02);
  rf_rpl_reg_rd(0x03);
  rf_rpl_reg_rd(0x04);
  rf_rpl_reg_rd(0x05);
  rf_rpl_reg_rd(0x06);
  rf_rpl_reg_rd(0x07);
  rf_rpl_reg_rd(0x08);
  rf_rpl_reg_rd(0x09);
  rf_rpl_reg_rd(0x0a);
  rf_rpl_reg_rd(0x0b);
  rf_rpl_reg_rd(0x0c);

}

static void rf_reg_init_fpga()
{
    rf_init_setting();
}

static void rf_reg_init()
{
    RF->REG00 = FIELD_BUILD(RF_EN_DAC_DIG_PWR, 1)
               | FIELD_BUILD(RF_EN_AGC_PWR, 1)
               | FIELD_BUILD(RF_EN_PLL_SDM, 1)
               | FIELD_BUILD(RF_EN_ADC_DIG, 1)
               | FIELD_BUILD(RF_EN_LDO_PAHP, 1)
               | FIELD_BUILD(RF_EN_LDO_PLL, 1)
               | FIELD_BUILD(RF_EN_LDO_VCO, 1)
               | FIELD_BUILD(RF_EN_LDO_PA, 1)
               | FIELD_BUILD(RF_EN_LDO_IF, 1)
               | FIELD_BUILD(RF_EN_LDO_TX, 1)
               | FIELD_BUILD(RF_EN_LDO_RX, 1)
               | FIELD_BUILD(RF_EN_LDO_PAHP_BYPS, 0)
               | FIELD_BUILD(RF_EN_PAHP, 0)
               | FIELD_BUILD(RF_EN_DAC_ZB, 0)
               | FIELD_BUILD(RF_EN_DAC_BLE, 0)
               | FIELD_BUILD(RF_EN_PA_STG2, 0)
               | FIELD_BUILD(RF_EN_PA_STG1, 0)
               | FIELD_BUILD(RF_EN_PA, 0)
               | FIELD_BUILD(RF_EN_PLL, 1)
               | FIELD_BUILD(RF_EN_AGC, 0)
               | FIELD_BUILD(RF_EN_ADC, 1)
               | FIELD_BUILD(RF_EN_LMT_RSSI, 1)
               | FIELD_BUILD(RF_EN_BPF, 1)
               | FIELD_BUILD(RF_EN_MIXL, 1)
               | FIELD_BUILD(RF_EN_MIXH, 1)
               | FIELD_BUILD(RF_EN_LNA, 1);
    RF->REG04 = FIELD_BUILD(RF_LNA_VB_ADJ,7)
               | FIELD_BUILD(RF_LNA_TANK_TUNE,1)
               | FIELD_BUILD(RF_LNA_R_ADJ,0)
               | FIELD_BUILD(RF_MIXL_BIAS_CTL,3)
               | FIELD_BUILD(RF_MIXL_BIAS_SEL,0)
               | FIELD_BUILD(RF_MIXH_BIAS_CTL,3)
               | FIELD_BUILD(RF_MIXH_BIAS_SEL,0)
               | FIELD_BUILD(RF_BPF_CAL_CODE_EXT,0x1d) // 1a  -1d
               | FIELD_BUILD(RF_BPF_CAL_CODE_EXT_EN,1)
               | FIELD_BUILD(RF_BPF_CAL_EN,0)
               | FIELD_BUILD(RF_EN_LNA_BYPS,0)
               | FIELD_BUILD(RF_LNA_GAIN,0)
               | FIELD_BUILD(RF_MIXL_GAIN_CTL,0)
               | FIELD_BUILD(RF_MIXH_GAIN_CTL,0)
               | FIELD_BUILD(RF_BPF_GAIN_ADJ,3)
               | FIELD_BUILD(RF_MIX_ENB_CAP,0);
    RF->REG08 = FIELD_BUILD(RF_LDO_RX_TRIM,1)
               | FIELD_BUILD(RF_LDO_TX_TRIM,1)
               | FIELD_BUILD(RF_CF_BW12M_ADJ,0)
               | FIELD_BUILD(RF_TX_RATE,0)
               | FIELD_BUILD(RF_CF_BW08M_ADJ,0)
               | FIELD_BUILD(RF_TX_DATA_TST_EN,0)
               | FIELD_BUILD(RF_PA_VCAS_RES_ADJ,1)
               | FIELD_BUILD(RF_PA_GAIN,0xf)
               | FIELD_BUILD(RF_PA_TANK_Q_ADJ,0)
               | FIELD_BUILD(RF_EN_PA_IBX2,0);
    RF->REG0C = FIELD_BUILD(RF_PA_TANK_TUNE,1)
               | FIELD_BUILD(RF_EN_RSSI_Q,1)
               | FIELD_BUILD(RF_EN_RSSI_I,1)
               | FIELD_BUILD(RF_PA_VB1_ADJ,0)
               | FIELD_BUILD(RF_PA_VB2_ADJ,0)
               | FIELD_BUILD(RF_PA_PTAT_ADJ,1)
               | FIELD_BUILD(RF_EN_PA_IPTAT,1)
               | FIELD_BUILD(RF_PA_BG_ADJ,4)
               | FIELD_BUILD(RF_EN_PA_IBG,1)
               | FIELD_BUILD(RF_PLL_BAND_CAL_SEL,0)
               | FIELD_BUILD(RF_PLL_AFC_FRAC_EN,1)
               | FIELD_BUILD(RF_PLL_AFC_DC_EN,1)
               | FIELD_BUILD(RF_PLL_VCTRL_EXT_EN,0)
               | FIELD_BUILD(RF_PLL_DIV_ADJ,2)
               | FIELD_BUILD(RF_PLL_SEL_RTX_BW,0);
    RF->REG10 = FIELD_BUILD(RF_PLL_DI_S,5)
               | FIELD_BUILD(RF_PLL_RTX_SEL,0)
               | FIELD_BUILD(RF_PLL_OPEN_EN,0)
               | FIELD_BUILD(RF_PLL_CAL_EN,0)
               | FIELD_BUILD(RF_PLL_FREQ_ADJ_EXT,0)
               | FIELD_BUILD(RF_PLL_FREQ_EXT_EN,0)
               | FIELD_BUILD(RF_PLL_FAST_LOCK_EN,1)
               | FIELD_BUILD(RF_PLL_REF_SEL,0)
               | FIELD_BUILD(RF_PLL_VREF_ADJ,7)
               | FIELD_BUILD(RF_PLL_FBDIV_PD_BYPS,0)
               | FIELD_BUILD(RF_PLL_BW_ADJ,2)
               | FIELD_BUILD(RF_PLL_LOCK_BYPS,0)
               | FIELD_BUILD(RF_PLL_CP_OS_ADJ,0)
               | FIELD_BUILD(RF_PLL_CP_OS_EN,0)
               | FIELD_BUILD(RF_PLL_VCO_ADJ,2);//2020 5.22CAI 2-6
    RF->REG14 = FIELD_BUILD(RF_PLL_FRAC,0)
               | FIELD_BUILD(RF_DAC_CAL_DATA_EXT,0)
               | FIELD_BUILD(RF_DAC_CAL_EN_EXT,0)
               | FIELD_BUILD(RF_DAC_EXT_EN,0)
               | FIELD_BUILD(RF_DAC_BLE_DELAY_ADJ,0x10);
    RF->REG18 = FIELD_BUILD(RF_DAC_REFL_ADJ,3)
               | FIELD_BUILD(RF_ADC_MUX_SEL,0)
               | FIELD_BUILD(RF_ADC_VREF_ADJ,0)
               | FIELD_BUILD(RF_ADC_TEST_SEL,0)
               | FIELD_BUILD(RF_EN_ADC_CNT_MODE,0)
               | FIELD_BUILD(RF_ADC_START,0);
    RF->REG1C = FIELD_BUILD(RF_EN_LDO_PLL_BYPS,0)
               | FIELD_BUILD(RF_EN_LDO_RX_BYPS,0)
               | FIELD_BUILD(RF_EN_LDO_TX_BYPS,0)
               | FIELD_BUILD(RF_EN_LDO_IF_BYPS,0)
               | FIELD_BUILD(RF_EN_LDO_PA_BYPS,0)
               | FIELD_BUILD(RF_EN_LDO_VCO_BYPS,0)
               | FIELD_BUILD(RF_ADC_REFBUF_LP,0)
               | FIELD_BUILD(RF_DAC_REFH_ADJ,1)
               | FIELD_BUILD(RF_PLL_DI_P,0x1f)
               | FIELD_BUILD(RF_PLL_FBDIV_PD,0)
               | FIELD_BUILD(RF_PLL_SDM_TEST_EN,0)
               | FIELD_BUILD(RF_PLL_FRAC_INT_MODE,0);
    RF->REG20 = FIELD_BUILD(RF_BPF_IADJ,4)
               | FIELD_BUILD(RF_BPF_BW_ADJ,1)
               | FIELD_BUILD(RF_BPF_MODE_SEL,0)
               | FIELD_BUILD(RF_BPF_CENT_ADJ,2)
               | FIELD_BUILD(RF_AT0_SEL,0xe)
               | FIELD_BUILD(RF_AT1_SEL,0xe);
    RF->REG24 = FIELD_BUILD(RF_AGC_S00L,8)
               | FIELD_BUILD(RF_AGC_S11_LNA_BYPS_ADJ,1)
               | FIELD_BUILD(RF_AGC_S10_LNA_BYPS_ADJ,1)
               | FIELD_BUILD(RF_AGC_S00H,0x2a)
               | FIELD_BUILD(RF_AGC_S01_MIX_ADJ,0)
               | FIELD_BUILD(RF_AGC_S01H,0x30)
               | FIELD_BUILD(RF_AGC_S10_MIX_ADJ,0x1) 
               | FIELD_BUILD(RF_AGC_S01L,0x1a)
               | FIELD_BUILD(RF_AGC_POWER_DET_EN,1)
               | FIELD_BUILD(RF_AGC_TEST_EN,0);
    RF->REG28 = FIELD_BUILD(RF_AGC_S10L,0x1d)
               | FIELD_BUILD(RF_AGC_S11_LNA_EN_ADJ,0)
               | FIELD_BUILD(RF_AGC_S10_LNA_EN_ADJ,0)
               | FIELD_BUILD(RF_AGC_S10_BPF_ADJ,1)
               | FIELD_BUILD(RF_AGC_T_ADJ,0)
               | FIELD_BUILD(RF_AGC_VH_ADD_ADJ,0)
               | FIELD_BUILD(RF_AGC_S01_BPF_ADJ,0)
               | FIELD_BUILD(RF_AGC_S11_BPF_ADJ,1)
               | FIELD_BUILD(RF_AGC_S00_BPF_ADJ,1)
               | FIELD_BUILD(RF_AGC_S11_MIX_ADJ,1)
               | FIELD_BUILD(RF_AGC_S00_MIX_ADJ,0)
               | FIELD_BUILD(RF_AGC_S11_LNA_ADJ,3)
               | FIELD_BUILD(RF_AGC_S10_LNA_ADJ,3)
               | FIELD_BUILD(RF_AGC_S01_LNA_ADJ,3)
               | FIELD_BUILD(RF_AGC_S00_LNA_ADJ,0);
    RF->REG2C = FIELD_BUILD(RF_PLL_GAIN_CAL_SEL,5)
               | FIELD_BUILD(RF_PLL_FBDIV_RST_SEL,0)
               | FIELD_BUILD(RF_PLL_FBDIV_RST_EXT,0)
               | FIELD_BUILD(RF_PLL_PS_CNT_RST_SEL,0)
               | FIELD_BUILD(RF_AGC_TEST_S,0)
               | FIELD_BUILD(RF_PA_MN_TUNE,1)
               | FIELD_BUILD(RF_PLL_GAIN_CAL_TH,0x1e)
               | FIELD_BUILD(RF_PLL_VTXD_EXT,9)
               | FIELD_BUILD(RF_PLL_VTXD_EXT_EN,0)
               | FIELD_BUILD(RF_PLL_GAIN_CAL_EN,0)
               | FIELD_BUILD(RF_PLL_GAIN_CAL_DC,1);
    RF->REG30 = FIELD_BUILD(RF_RSV,0x44)
               | FIELD_BUILD(RF_LDO_PA_TRIM,6)
               | FIELD_BUILD(RF_EN_LMT_OUTI_EXT,1)
               | FIELD_BUILD(RF_EN_LMT_OUTQ_EXT,1)
               | FIELD_BUILD(RF_PAHP_SEL,0)
               | FIELD_BUILD(RF_LDO_PAHP_TRIM,0)
               | FIELD_BUILD(RF_EN_AT,0)
               | FIELD_BUILD(RF_PAHP_ADJ,0xf);
    RF->REG50 = FIELD_BUILD(RF_ANA_TEST_EN,0)
               | FIELD_BUILD(RF_PLL_AFC_BP,0)
               | FIELD_BUILD(RF_PLL_GAIN_BP,0)
               | FIELD_BUILD(RF_PPF_RC_BP,1)
               | FIELD_BUILD(RF_LDO_TEST_EN,0)
               | FIELD_BUILD(RF_RD_CLK_EN,1)
               | FIELD_BUILD(RF_PLL_TEST_EN,0)
               | FIELD_BUILD(RF_CH_SEL,1)
               | FIELD_BUILD(RF_PA_VB_SEL,0)  //  0-rf_ctl      1-cs_ctl
               | FIELD_BUILD(RF_PA_VB_TARGET,0x4)
               | FIELD_BUILD(RF_LDO_START_CNT,6)
               | FIELD_BUILD(RF_PA_STEP_SET,4);
    // RF->REG58 = FIELD_BUILD(RF_EN_DAC_CNT,10)
    //            | FIELD_BUILD(RF_PLL_CAL_EN_CNT,2)
    //            | FIELD_BUILD(RF_PLL_GAIN_CAL_EN_CNT,0)
    //            | FIELD_BUILD(RF_EN_PA_CNT,0x33);
    // RF->REG5C = FIELD_BUILD(RF_EN_PA_STG1_CNT,0x35)
    //            | FIELD_BUILD(RF_EN_PA_STG2_CNT,0x37)
    //            | FIELD_BUILD(RF_PLL_LOCK_CNT,0x39)
    //            | FIELD_BUILD(RF_EN_RX_CNT,2);

    RF->REG64 = FIELD_BUILD(RF_RSSI_OFFSET, 0X80)
               |FIELD_BUILD(RF_ADC_MDM_EN, 1);      
    RF->REG70 = FIELD_BUILD(RF_RX2MBW_FORCE_EN,0)
               | FIELD_BUILD(RF_INT_VTXD_CHN_THR1,0x19)
               | FIELD_BUILD(RF_INT_VTXD_CHN_THR0,0xc)
               | FIELD_BUILD(RF_INT_VTXD_EXT2,9)
               | FIELD_BUILD(RF_INT_VTXD_EXT1,9);    
}

static void modem_reg_init()
{
    MDM2->REG08 = FIELD_BUILD(MDM2_MIN_MAG_CONF,0x474)
               | FIELD_BUILD(MDM2_DRIFT_COR_SET,1)
               | FIELD_BUILD(MDM2_IF_SHIFT,0x400);
    MDM2->REG20 = FIELD_BUILD(MDM2_LR_IF_SHIFT,0x400)
               | FIELD_BUILD(MDM2_LR_RX_INVERT,1)
               | FIELD_BUILD(MDM2_LR_IQ_INVERT,0)
               | FIELD_BUILD(MDM2_LR_ACC_INVERT,0);
}


void modem_rf_init()
{
    modem_reg_init();
    rf_reg_init_fpga();
}