#ifndef __RCHBAND_CODEC_LL_H__
#define __RCHBAND_CODEC_LL_H__
#include <drv/common.h>
#include <stdbool.h>

#define CK_IP_CTRL_CODEC_Pos                      (10U)
#define CK_IP_CTRL_CODEC_Msk                      (0x1U << CK_IP_CTRL_CODEC_Pos)
#define CK_IP_CTRL_CODEC                          CK_IP_CTRL_CODEC_Msk

typedef enum {
    MIC_IN_SINGLE_ENDED,
    MIC_IN_DIFFERENTIAL,
    LINE_IN_SINGLE_ENDED,
    LINE_IN_DIFFERENTIAL,
} CODEC_INPUT_PATH_E;

typedef enum {
    HP_OUT_SINGLE_ENDED,
    HP_OUT_DIFFERENTIAL,
} CODEC_OUTPUT_PATH_E;

typedef enum {
    PGA_GAIN_N12DB = 0x00,
    PGA_GAIN_0DB = 0x10,
    PGA_GAIN_35DB = 0x3f,
} CODEC_PGA_GAIN_E;


typedef enum {
    BIAS_1V57 = 0,
    BIAS_1V81,
    BIAS_1V96,
    BIAS_2V35,
} CODEC_MIC_BIAS_E;

typedef struct {
    CODEC_INPUT_PATH_E  e_input_path;
    CODEC_OUTPUT_PATH_E e_output_path;

    /*
     *   放大增益,范围 0 ~ 0x3f，步进0.75DB
     *   MIC : 8DB + input_gain*0x75
     *         0（8DB）, 1(8.75DB) ... 0x10(20DB) ... 0x3f(55.25DB)
     *   Line In:  input_gain*0x75
     *        0（-12DB）, 1(-11.25DB) ... 0x10(0DB) ... 0x3f(35.25DB)
     */
    uint8_t  input_gain;

    /*
     *    最在输出音量，0x00 ~ 0x3F,步进0.75DB
     *    0（-35.25DB）... 0x2f(0DB) ... 0x3f(12DB)
     */
    uint8_t  output_gain;

} codec_config_t;


// CODEC 上电初始化
void rchband_codec_init(codec_config_t  config);

/*
 *  延时初始化CODEC寄存器，解决单端上电，电容充电导致的“砰”声
 *  输入参数：
 * 		config : CODEC相关配置
 * 		i_timer : 芯片上电时间，单位10mS
 */
//bool rchband_codec_delay_init(codec_config_t  config,uint32_t i_timer);

/*
 *  功能描述： 控制Codec进入低功耗模式
 *  输入参数：
 */
uint8_t rchband_codec_power_down_codec();

/*
 *  读取CODEC所有寄存器到CodecRegValue数组
 */
void rchband_codec_read_all_reg(void);

/*
 *  功能描述： 设置耳机输出是否静音
 *  输入参数： f_mute
 *            true: 静音
 *            false: 非静音
 */
void  rchband_codec_set_dac_mute(bool f_mute);

/*
 *  功能描述： 控制耳机输出驱动电源
 *  输入参数： f_power_on
 *            true: 打开耳机输出电源
 *            false: 关闭耳机输出电源
 */
void  rchband_codec_set_dac_power_on(bool f_power_on);

/*
 * 功能描述： 使能耳机输出驱动，关闭静音
 *           主要用在Codec初始后，打开耳机功能
 */
void rchband_codec_power_on_dac(void);


/*
 * 功能描述： 设置Codec输入路径
 * 输入参数： path
 *     MIC_IN_SINGLE_ENDED   :  麦克风单端输入
 *     MIC_IN_DIFFERENTIAL   :  麦克风差分输入
 *     LINE_IN_SINGLE_ENDED  :  线性单端输入
 *     LINE_IN_DIFFERENTIAL  :  线性差分输入
 */
void rchband_codec_set_input_path(CODEC_INPUT_PATH_E path);


/*
 *  功能描述： 设置麦克风输入放大增益
 *  输入参数： vol_db
 *            放大增益,范围 0 ~ 0x3f，步进0.75DB
 *            20DB + vol_db*0x75
 *            0（8DB）, 1(8.75DB) ... 0x10(20DB) ... 0x3f(55.25DB)
 */
bool rchband_codec_set_mic_volume(uint8_t vol_db);

/*
 *  功能描述： 设置线性输入放大增益
 *  输入参数： vol_db
 *            放大增益,范围 0 ~ 0x3f，步进0.75DB
 *            20DB + vol_db*0x75
 *            0（-12DB）, 1(-11.25DB) ... 0x10(0DB) ... 0x3f(35.25DB)
 */
bool rchband_codec_set_line_in_volume(uint8_t vol_db);

/*
 *  功能描述： 设置耳机输出音量
 *  输入参数： volume
 *            耳机输出音量，范围：0 ~ 0x3f,步进0.75DB
 *            0（-35.25DB）... 0x2f(0DB) ... 0x3f(12DB)
 */
uint8_t rchband_codec_set_dac_volume(uint8_t volume);

/*
 *  功能描述： 设置耳机输出音量
 *  输入参数：
 *       volume
 *            耳机输出音量百分比，范围：0 ~ 100
 *       max_db
 *            最在输出音量，0x00 ~ 0x3F,步进0.75DB
 *             0（-35.25DB）... 0x2f(0DB) ... 0x3f(12DB)
 *            CODEC Gain最大可设到12DB,但是很多耳机的最大音量不需要输出到12DB
 *
 */
uint8_t rchband_codec_set_dac_volume_per(uint8_t per, uint8_t max_db);


/*
 *  功能描述： 设置MIC BIAS输出电压
 *  输入参数： bias
 *            BIAS_1V57 = 1.57V
 *            BIAS_1V81 = 1.81V
 *            BIAS_1V96 = 1.96V
 *            BIAS_2V35 = 2.35V
 */
bool rchband_codec_set_mic_bias(CODEC_MIC_BIAS_E bias);

/*
 *	功能描述： 控制麦克风baise电压是否输出
 *  输入参数： f_power_up
 *            false: 关mic baise
 *            true: 开mic baise
 */
void rchband_codec_set_mic_bias_power(bool f_power_up);


/*
 *  功能描述： 设置麦克风音量
 *  输入参数：
 *       volume
 *            麦克风音量百分比，范围：0 ~ 100
 *       max_db
 *            最在输出音量，0x00 ~ 0x3F,步进0.75DB
 *            20DB + vol_db*0x75
 *            0（8DB）, 1(8.75DB) ... 0x10(20DB) ... 0x3f(55.25DB)
 *
 */
uint8_t rchband_codec_set_mic_volume_per(uint8_t per, uint8_t max_db);

/*
 *  复位初始化状态机到重新初始化Line In
 */
uint32_t rchband_codec_get_retry_init_mic_tick(void);

/*
 *  配置寄存器，解决Codec掉电时，单端耳机产生的噪音
 */
void rchband_codec_power_down_setting(void);

/* set codec sample width*/
uint32_t rchband_codec_dac_sample_width(uint32_t sample_width);

/* set codec sample rate*/
uint32_t rchband_codec_dac_sample_rate(uint32_t sample_rate);

/* set codec sample width*/
uint32_t rchband_codec_adc_sample_width(uint32_t sample_width);

/* set output ch is difference*/
uint32_t rchband_codec_output_ch_config(uint32_t is_difference);

/* set input ch is difference*/
void rchband_codec_input_ch_config(uint32_t is_difference);

/* resume from low power*/
void rchband_codec_resume_form_low_power(void);
/*
 *   [47][0c] : 0x09  FBCLK = FMCLK1/4，I2S master
 *   [0D][0E} : DA LRCK=BCLK_O/64
 *   [48][49} : AD LRCK=BCLK_O/64
 *   [05][06] : OSR=256
 *   [02][41] : I2S,22bit
 *   [03][43] : DCLK=MCLK1/4   ACLK_O=MCLK1/4
 *   [01] : Reset to DAC,Enable DCLK ,Enable DAI Interface
 *   [40] : Reset to ADC,Enable DCLK ,Enable DAI Interface
 *   [4B] : Power Up,ΣΔADC , Microphone Input
 *   [0F] : Power Up,ΣΔDAC Modulator,Headphone Driver
 *   [17][18] : DAC Channel PGA Gain, -35.25dB ~ +12dB
 *   [51][52] : AD Channel PGA Gain, -12dB ~ +35.25dB
 *   [4C] : Select ADC Input Channel,Differential or Single-ended
 *
 */
#define    DAC_SINGLE_END         0x80U
#define    DAC_DIFFER_END         0x00U

static const unsigned char  CodecRegPartOne[][2] = {
    {0x01U, 0x04U},	 // Soft Reset to DAC Analog
    {0xFEU, 0x05U},		// delay 10ms
    {0x01U, 0x00U},	 // DAI Interface Disable,DCLK_O Disable
    {0xFEU, 0x05U},		// delay 10ms
    {0x01U, 0x03U},		// DA DAI Interface,DCLK_O: Enable
    //--------------------------------------------------------------------
    {0x09U, 0x87U},   // enable dac mute
    {0xFEU, 0x64U},		// delay 100ms
    {0x12U, 0x9FU},		// VREF,Bandgap Power On
    {0xFEU, 0x05U},		// delay 5ms
    //---------------------------------------------------------
    {0x10U, 0x06U},		// --(0x4A) Add Delay in Analog Clock Signals
    {0xFEU, 0x05U},		// delay 5ms
    {0x15U, 0x0AU},		// Basic Setup for DAC Internal Regulator
    {0xFEU, 0x0AU},		// delay 10ms
    {0x11U, 0xA7U},		// --(0x40) Adjust Internal Analog Block
    {0xFEU, 0x05U},		// delay 5ms
    {0x13U, 0x7CU},   // 0xFC(Single-ended or Virtual Ground) 0x7C(differ),Adjustment for Anti-pop Function
    // No signal, so differential can be output.
    {0xFEU, 0x05U},		// delay 5ms
    {0x0FU, 0x48U},		// --(0x08) PowerUp DAC Current Reference
    {0xFEU, 0x50U},		// delay 80ms
    {0x0FU, 0x7CU},   // PowerUp DAC Current Reference,DAC Regulator
    // delay 3 S
    //--------------------------------------------------------------------
    // End
    {0xFFU, 0xFFU}
};

static const unsigned char  CodecRegPartTwo[][2] = {
    {0x02U, 0x02U},		// DA I2S,16bit
    {0x03U, 0x04U},		// DCLK_O = MCLK1/MCLKDIV=MCLK1/4
    {0x04U, 0x00U},	    // Digital Gain
    {0x14U, 0x22U},		// Channel Output Mixer Gain, default  -3.3DB
    {0x05U, 0x00U},		// OSR[7:0] ,FMCLK1 = OSR x FS,
    {0x06U, 0xB1U},		// OSR_SEL[3:0],OSR[11:8],OSR= 256
    {0x0CU, 0x09U},		// FDA_BCLK_O = FMCLK1/DA_BCLKDIV = FMCLK1/4,bit[0] : 0(Slave)
    {0x0DU, 0x40U},		// FDA_LRC_O = FDA_BCLK_O/DA_LRCDIV=FDA_BCLK_O/64
    {0x0EU, 0x00U},		// DA_LRCDIV[11:8]
    {0x0FU, 0x3DU},       // PowerUp DAC Current Reference,DAC Regulator
    {0x17U, 0x2fU},		// DAC Left Channel PGA Gain. 0x2F(0DB)
    {0x18U, 0x2fU},		// DAC Right Channel PGA Gain. 0x2F(0DB)
    {0x36U, 0xEDU},		// DAC High Pass Filter Offset Recording, 3.73Hz
    {0x09U, 0x86U},       // disable dac mute
    // -------------------------------------------
    // End
    {0xFFU, 0xFFU}
};


static const unsigned char  CodecRegPartThree[][2] = {
    //===================================================================
    // Line In
    {0x44U, 0x10U},		// MCLK1 = AD_MCLKDIV x SINC_RATE x 4 x Fs
    {0x45U, 0x00U},		// Attenuation of SINC Output
    {0x47U, 0x09U},		// FBCLK_O = FMCLK1/BCLKDIV = FMCLK1/4 , bit[0]: 0(Slave)  1(master)
    {0x48U, 0x40U},		// FAD_LRC_O = FAD_BCLK_O/AD_LRCDIV = FAD_BCLK_O/64
    {0x49U, 0x00U},		// AD_LRCDIV[11:8]
    //----------------------------------------------------------------------
    // MIC differential
    {0x4BU, 0x9FU},		// Power Up,ΣΔADC , Microphone Input
    {0x4CU, 0x1EU},  	// Line-In Differential
    {0x51U, 0x00U},		// Left Channel Microphone/Line-In Gain , 0xA0(20DB + 12DB) 0x10(0DB)
    {0x52U, 0x00U},		// Right Channel Microphone/Line-In Gain, 0x96(20DB + 4.5DB) 0x19(6.75DB)
    //----------------------------------------------------------------------
    {0x4DU, 0x3CU},		// Adjust Internal Analog Block I Current
    {0x4EU, 0x10U},		// MIC_BIAS. 0x00(1.57V) 0x10(1.81V) 0x20(1.96V) 0x30(2.35V)
    {0x4FU, 0x18U},		// Basic Setup for ADC Internal Regulator
    {0x54U, 0x38U},		// Disable ALC
    {0x55U, 0x74U},		// Set ALC Target Peak Level
    // {0x55,0xDF},
    {0x40U, 0x03U},		// AD DAI Interface,ACLK_O: Enable
    {0x65U, 0x00U},		// ADC Internal Master Clock Divider of MCLK from SoC , set 000 only
    // -------------------------------------------
    // End
    {0xFFU, 0xFFU}
};



#endif

