#ifndef _SILAN_CODEC_H_
#define _SILAN_CODEC_H_

#include "silan_types.h"

#define CODEC_DAC_GAIN_MAX         2325
#define CODEC_DAC_GAIN_MIN        -7200
#define CODEC_DAC_GAIN_STEP         75
#define CODEC_ADC_GAIN_MAX         2300
#define CODEC_ADC_GAIN_MIN         0
#define CODEC_ADC_GAIN_STEP         100

typedef enum CLK_mode
{
    CLK_SLAVE        = 0,
    CLK_MASTER,
}
CLK_MODE, *PCLK_MODE;

typedef enum CODEC_id {
    CODEC_ID_EXT,
    CODEC_ID_1,
    CODEC_ID_2,
    CODEC_ID_3,
}
CODEC_ID, *PCODEC_ID;

typedef enum {
    IIS_16BIT   = 0,
    IIS_18BIT   ,
    IIS_20BIT   ,
    IIS_24BIT   ,
}silan_codec_iis_w_t;

typedef enum {
    LINE_IN   = 0,      // VIIN
    AUX_IN   ,          // AUX
    FM_IN   ,           // FM
    MIC_IN   ,          // MIC
}silan_codec_adc_ch_t;

typedef enum {
    SLAVE   = 0,
    MASTER   ,
}silan_codec_role_t;

typedef enum {
    CODEC1       = 0,
    CODEC2 ,
}silan_codec_t;

typedef enum {
    DIFF       = 0,
    SINGLE ,
}silan_codec_inport_t;

typedef enum {
    CODEC_MCLK_APLL_1   = 0,
    CODEC_MCLK_APLL_2   ,
    CODEC_MCLK_APLL_3   ,
    CODEC_MCLK_APLL_4   ,
}silan_codec_mclk_t;

typedef enum CODEC_ds
{
    CODEC_DIFF        = 0,
    CODEC_SINGLE,
}
CODEC_DS, *PCODEC_DS;

typedef enum CODEC_wl
{
    CODEC_WL_16        = 0,
    CODEC_WL_18,
    CODEC_WL_20,
    CODEC_WL_24,
}
CODEC_WL, *PCODEC_WL;

typedef enum CODEC_adsel
{
    CODEC_ADSEL_LINE    = 0,
    CODEC_ADSEL_AUX,
    CODEC_ADSEL_FM,
    CODEC_ADSEL_MIC,
}
CODEC_ADSEL, *PCODEC_SEL;

typedef enum CODEC_micboost
{
    CODEC_MICBOOST_0    = 0,
    CODEC_MICBOOST_6,
    CODEC_MICBOOST_12,
    CODEC_MICBOOST_20,
}
CODEC_MICBOOST, *PCODEC_MICBOOST;

typedef struct CODEC_cfg
{
    CLK_MODE        adc_master;
    CLK_MODE        dac_master;
    CODEC_WL        adc_wl;
    CODEC_WL        dac_wl;
    CODEC_ADSEL        adc_sel;
    CODEC_DS        adc_diff;
    CODEC_MICBOOST    mic_boost;
}
CODEC_CFG, *PCODEC_CFG;
/*
 * gain :
 * default : 0x00   0db
 * step 1db
 * max 0x17 23db
 */
#define silan_codec_dac_config    config_codec_iis_dac
#define silan_codec_adc_config    config_codec_iis_adc

extern int set_adc1_left_channel_digital_gain(int gain);
extern int set_adc1_right_channel_digital_gain(int gain);
extern int set_adc2_left_channel_digital_gain(int gain);
extern int set_adc2_right_channel_digital_gain(int gain);
/*
 * gain :
 * default : 0x30   0db
 * step 0.75db
 * max 0x7f  23.25db
 */
extern int set_dac1_left_channel_analog_gain(int gain);
extern int set_dac1_right_channel_analog_gain(int gain);
extern int set_dac2_left_channel_analog_gain(int gain);
extern int set_dac2_right_channel_analog_gain(int gain);
/*
 * gain :
 * default : 0x08   0db
 * step 1.5db
 * max 0x1f  34.5db
 */
extern int set_adc1_left_channel_mic_analog_gain(int gain);
extern int set_adc1_right_channel_mic_analog_gain(int gain);
extern int set_adc2_left_channel_mic_analog_gain(int gain);
extern int set_adc2_right_channel_mic_analog_gain(int gain);
/*
 * gain :
 * default : 0x10   0db
 * step 0.75db
 * max 0x20  12db /min 0x00  -12db
 */
extern int set_adc1_left_channel_other_analog_gain(int gain);
extern int set_adc1_right_channel_other_analog_gain(int gain);
extern int set_adc2_left_channel_other_analog_gain(int gain);
extern int set_adc2_right_channel_other_analog_gain(int gain);
/*
 * gain :
 * default : 0x00   0db
 * step 6db
 * max 0x3  20db /min 0x00  0db
 */
int set_adc1_left_channel_micboost_gain(int gain);
int set_adc1_right_channel_micboost_gain(int gain);
int set_adc2_left_channel_micboost_gain(int gain);
int set_adc2_right_channel_micboost_gain(int gain);


/*
 * LINE_IN/ AUX_IN /MIC/FM......
 * inport_x :DIFF or SINGLE-ENDED
 */
void silan_codec1_adc_analog_init(silan_codec_adc_ch_t channel,\
        silan_codec_inport_t inport_x);
void silan_codec2_adc_analog_init(silan_codec_adc_ch_t channel,\
        silan_codec_inport_t inport_x);

/*
 *
 * mode : 1 master  ,0 sla
 * bits: 0:16bits  ;1:18bits ...
 * fs: bclk/ws
 */
void silan_codec1_adc_digital_init(silan_codec_role_t mode,\
        silan_codec_iis_w_t bits, unsigned int fs);
void silan_codec1_dac_digital_init(silan_codec_role_t mode,\
        silan_codec_iis_w_t bits, unsigned int fs);
void silan_codec2_adc_digital_init(silan_codec_role_t mode,\
        silan_codec_iis_w_t bits, unsigned int fs);
void silan_codec2_dac_digital_init(silan_codec_role_t mode,\
        silan_codec_iis_w_t bits, unsigned int fs);

/*
codec :CODEC1/CODEC2
rate:   adc  8k-96k /dac 8k-192k
*/
void config_codec_iis_dac(silan_codec_t codec,int rate);
void config_codec_iis_adc(silan_codec_t codec,int rate);
/*
codec init
default: fs= 64000, 24bit ,master
*/
int silan_codec_init(void);

int silan_codec1_dac_mute();
int silan_codec2_dac_mute();
int silan_codec1_dac_unmute();
int silan_codec2_dac_unmute();

uint32_t silan_codec_check(silan_codec_t codec);
void silan_codec_open(silan_codec_t codec);
void silan_codec_dac_init(silan_codec_t codec, CODEC_CFG *pcfg);
void silan_codec_adc_init(silan_codec_t codec, CODEC_CFG *pcfg);
void silan_codec_dac_mute(silan_codec_t codec);
void silan_codec_dac_unmute(silan_codec_t codec);
int silan_codec_dac_set_gain(silan_codec_t codec, int l_gain, int r_gain);
int silan_codec_dac_get_gain(silan_codec_t codec, int *l_gain, int *r_gain);
int silan_codec_adc_set_gain(silan_codec_t codec, int l_gain, int r_gain);

void silan_codec_adc_open(silan_codec_t codec);
void silan_codec_dac_open(silan_codec_t codec);

/*
 *
 * codec = CODEC1 or CODEC2
 */
void silan_codec_alc_on(silan_codec_t codec);
void silan_codec_alc_off(silan_codec_t codec);


/*
 * codec : CODEC1 or CODEC2
 * alc_t_up: min=0 ,max=0xa,default=3
 * alc_t_dn: min=0 ,max=0xa,default=8
 * alc_maxgain: min=0 ,max=0x7,default=4
 * alc_mingain: min=0 ,max=0x7,default=0
 * level: min=0 ,max=0xf,default=3(-19.5db);[-24db,-1.5db]
 */
void silan_codec_alc_para_config(silan_codec_t codec,u8 alc_t_up,u8 alc_t_dn, \
                                                                 u8 alc_maxgain,u8 alc_mingain,u8 level);

#endif //_SILAN_CODEC_H_
