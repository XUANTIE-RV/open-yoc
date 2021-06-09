#ifndef _LSADC_H_
#define _LSADC_H_
#include <stdint.h>
#include "lsadc.h"
#include "reg_lsadc_type.h"
#include "HAL_def.h"

/******************************************************************************/
/*                                                                            */
/*                      Analog to Digital Converter (ADC)                     */
/*                                                                            */
/******************************************************************************/

/*
 * 
 */
#define ADC_MULTIMODE_SUPPORT /*!< ADC feature available only on specific devices: multimode available on devices with several ADC instances */

/********************  Bit definition for ADC_SR register  ********************/
#define ADC_SR_AWD ADC_AWD_MASK       /*!< ADC analog watchdog 1 flag */
#define ADC_SR_EOS ADC_REOS_MASK      /*!< ADC group regular end of sequence conversions flag */
#define ADC_SR_JEOS ADC_JEOS_MASK     /*!< ADC group injected end of sequence conversions flag */
#define ADC_SR_JSTRTC ADC_JSTRTC_MASK /*!< ADC group injected conversion start flag */
#define ADC_SR_RSTRTC ADC_RSTRTC_MASK /*!< ADC group regular conversion start flag */

#define ADC_SFCR_AWDC ADC_AWDC_MASK      /*!< ADC analog watchdog 1 flag */
#define ADC_SFCR_EOSC ADC_REOSC_MASK     /*!< ADC group regular end of sequence conversions flag */
#define ADC_SFCR_JEOSC ADC_JEOSC_MASK    /*!< ADC group injected end of sequence conversions flag */
#define ADC_SFCR_JSTRTC ADC_JSTRTCC_MASK /*!< ADC group injected conversion start flag */
#define ADC_SFCR_RSTRTC ADC_RSTRTCC_MASK /*!< ADC group regular conversion start flag */

/* Legacy defines */
#define ADC_SR_EOC (ADC_SR_EOS)
#define ADC_SR_JEOC (ADC_SR_JEOS)

/*******************  Bit definition for ADC_CR1 register  ********************/
#define ADC_CR1_AWDCH ADC_AWDCH_MASK /*!< ADC analog watchdog 1 monitored channel selection */

#define ADC_CR1_REOCIE ADC_REOCIE_MASK
#define ADC_CR1_REOSIE ADC_REOSIE_MASK   /*!< ADC group regular end of sequence conversions interrupt */
#define ADC_CR1_AWDIE ADC_AWDIE_MASK     /*!< ADC analog watchdog 1 interrupt */
#define ADC_CR1_JEOSIE ADC_JEOSIE_MASK   /*!< ADC group injected end of sequence conversions interrupt */
#define ADC_CR1_SCAN ADC_SCAN_MASK       /*!< ADC scan mode */
#define ADC_CR1_AWDSGL ADC_AWDSGL_MASK   /*!< ADC analog watchdog 1 monitoring a single channel or all channels */
#define ADC_CR1_JAUTO ADC_JAUTO_MASK     /*!< ADC group injected automatic trigger mode */
#define ADC_CR1_DISCEN ADC_RDISCEN_MASK  /*!< ADC group regular sequencer discontinuous mode */
#define ADC_CR1_JDISCEN ADC_JDISCEN_MASK /*!< ADC group injected sequencer discontinuous mode */

#define ADC_CR1_DISCNUM ADC_DISCNUM_MASK /*!< ADC group regular sequencer discontinuous number of ranks */
//#define ADC_CR1_DISCNUM_0                   (0x1UL << ADC_CR1_DISCNUM_Pos)      /*!< 0x00002000 */
//#define ADC_CR1_DISCNUM_1                   (0x2UL << ADC_CR1_DISCNUM_Pos)      /*!< 0x00004000 */
//#define ADC_CR1_DISCNUM_2                   (0x4UL << ADC_CR1_DISCNUM_Pos)      /*!< 0x00008000 */

#define ADC_CR1_JAWDEN ADC_JAWDEN_MASK /*!< ADC analog watchdog 1 enable on scope ADC group injected */
#define ADC_CR1_AWDEN ADC_RAWDEN_MASK  /*!< ADC analog watchdog 1 enable on scope ADC group regular */

/* Legacy defines */
//#define  ADC_CR1_REOSIE                       (ADC_CR1_REOSIE)
//#define  ADC_CR1_JEOSIE                       (ADC_CR1_JEOSIE)

/*******************  Bit definition for ADC_CR2 register  ********************/
#define ADC_CR2_ADON ADC_ADEN_MASK             /*!< ADC enable */
#define ADC_CR2_CONT ADC_CONT_MASK             /*!< ADC group regular continuous conversion mode */
#define ADC_CR2_ALIGN ADC_ALIGN_MASK           /*!< ADC data alignement */
#define ADC_CR2_SWSTART ADC_RTRIG_MASK         /*!< ADC group regular conversion start */
#define ADC_CR2_RTRIG (0x1UL << ADC_RTRIG_POS) /*!< 0x00020000 */
/******************  Bit definition for ADC_SMPR1 register  *******************/
#define ADC_SMPR1_SMP10 ADC_SMP0_MASK             /*!< ADC channel 10 sampling time selection  */
#define ADC_SMPR1_SMP10_0 (0x1UL << ADC_SMP0_POS) /*!< 0x00000001 */
#define ADC_SMPR1_SMP10_1 (0x2UL << ADC_SMP0_POS) /*!< 0x00000002 */

#define ADC_SMPR1_SMP11 ADC_SMP1_MASK             /*!< ADC channel 11 sampling time selection  */
#define ADC_SMPR1_SMP11_0 (0x1UL << ADC_SMP1_POS) /*!< 0x00000001 */
#define ADC_SMPR1_SMP11_1 (0x1UL << ADC_SMP1_POS) /*!< 0x00000002 */

/******************  Bit definition for ADC_JOFR1 register  *******************/
#define ADC_JOFR1_JOFFSET1 ADC_JOFF1_MASK /*!< ADC group injected sequencer rank 1 offset value */

/******************  Bit definition for ADC_JOFR2 register  *******************/
#define ADC_JOFR2_JOFFSET2 ADC_JOFF2_MASK /*!< ADC group injected sequencer rank 2 offset value */

/******************  Bit definition for ADC_JOFR3 register  *******************/
#define ADC_JOFR3_JOFFSET3 ADC_JOFF3_MASK /*!< ADC group injected sequencer rank 3 offset value */

/******************  Bit definition for ADC_JOFR4 register  *******************/
#define ADC_JOFR4_JOFFSET4 ADC_JOFF4_MASK /*!< ADC group injected sequencer rank 4 offset value */

/*******************  Bit definition for ADC_HTR register  ********************/
#define ADC_HTR_HT ADC_HT_MASK /*!< ADC analog watchdog 1 threshold high */

/*******************  Bit definition for ADC_LTR register  ********************/
#define ADC_LTR_LT ADC_LT_MASK /*!< ADC analog watchdog 1 threshold low */

/*******************  Bit definition for ADC_SQR1 register  *******************/
#define ADC_RSQR1 ADC_RSQ1_MASK                   /*!< ADC group regular sequencer rank 13 */
#define ADC_RSQR1_RSQ1_0 (0x01UL << ADC_RSQ1_POS) /*!< 0x00000001 */
#define ADC_RSQR1_RSQ1_1 (0x02UL << ADC_RSQ1_POS) /*!< 0x00000002 */
#define ADC_RSQR1_RSQ1_2 (0x04UL << ADC_RSQ1_POS) /*!< 0x00000003 */
#define ADC_RSQR1_RSQ1_3 (0x08UL << ADC_RSQ1_POS) /*!< 0x00000004 */

#define ADC_RSQR2 ADC_RSQ2_MASK                   /*!< ADC group regular sequencer rank 13 */
#define ADC_RSQR2_RSQ2_0 (0x01UL << ADC_RSQ2_POS) /*!< 0x00000001 */
#define ADC_RSQR2_RSQ2_1 (0x02UL << ADC_RSQ2_POS) /*!< 0x00000002 */
#define ADC_RSQR2_RSQ2_2 (0x04UL << ADC_RSQ2_POS) /*!< 0x00000003 */
#define ADC_RSQR2_RSQ2_3 (0x08UL << ADC_RSQ2_POS) /*!< 0x00000004 */

/*******************  Bit definition for ADC_SQR2 register  *******************/
#define ADC_RSQR9 ADC_RSQ9_MASK                   /*!< ADC group regular sequencer rank 13 */
#define ADC_RSQR9_RSQ9_0 (0x01UL << ADC_RSQ9_POS) /*!< 0x00000001 */
#define ADC_RSQR9_RSQ9_1 (0x02UL << ADC_RSQ9_POS) /*!< 0x00000002 */
#define ADC_RSQR9_RSQ9_2 (0x04UL << ADC_RSQ9_POS) /*!< 0x00000003 */
#define ADC_RSQR9_RSQ9_3 (0x08UL << ADC_RSQ9_POS) /*!< 0x00000004 */
/*******************  Bit definition for ADC_JSQR register  *******************/
#define ADC_JSQR_JSQ1 ADC_JSQ1_MASK              /*!< ADC group injected sequencer rank 1 */
#define ADC_JSQR_JSQ1_0 (0x00UL << ADC_JSQ1_POS) /*!< 0x00000001 */
#define ADC_JSQR_JSQ1_1 (0x01UL << ADC_JSQ1_POS) /*!< 0x00000002 */
#define ADC_JSQR_JSQ1_2 (0x02UL << ADC_JSQ1_POS) /*!< 0x00000004 */
#define ADC_JSQR_JSQ1_3 (0x04UL << ADC_JSQ1_POS) /*!< 0x00000008 */

#define ADC_SQLR_RL ADC_RSQL_MASK              /*!< ADC group injected sequencer scan length */
#define ADC_SQLR_RL_1 (0x0UL << ADC_RSQL_POS)  /*!< 0x00100000 */
#define ADC_SQLR_RL_2 (0x1UL << ADC_RSQL_POS)  /*!< 0x00200000 */
#define ADC_SQLR_RL_3 (0x2UL << ADC_RSQL_POS)  /*!< 0x00100000 */
#define ADC_SQLR_RL_4 (0x3UL << ADC_RSQL_POS)  /*!< 0x00200000 */
#define ADC_SQLR_RL_5 (0x4UL << ADC_RSQL_POS)  /*!< 0x00100000 */
#define ADC_SQLR_RL_6 (0x5UL << ADC_RSQL_POS)  /*!< 0x00200000 */
#define ADC_SQLR_RL_7 (0x6UL << ADC_RSQL_POS)  /*!< 0x00100000 */
#define ADC_SQLR_RL_8 (0x7UL << ADC_RSQL_POS)  /*!< 0x00200000 */
#define ADC_SQLR_RL_9 (0x8UL << ADC_RSQL_POS)  /*!< 0x00100000 */
#define ADC_SQLR_RL_10 (0x9UL << ADC_RSQL_POS) /*!< 0x00200000 */
#define ADC_SQLR_RL_11 (0xaUL << ADC_RSQL_POS) /*!< 0x00100000 */
#define ADC_SQLR_RL_12 (0xbUL << ADC_RSQL_POS) /*!< 0x00200000 */

#define ADC_SQLR_JL ADC_JSQL_MASK             /*!< ADC group injected sequencer scan length */
#define ADC_SQLR_JL_1 (0x0UL << ADC_JSQL_POS) /*!< 0x00100000 */
#define ADC_SQLR_JL_2 (0x1UL << ADC_JSQL_POS) /*!< 0x00200000 */
#define ADC_SQLR_JL_3 (0x2UL << ADC_JSQL_POS) /*!< 0x00100000 */
#define ADC_SQLR_JL_4 (0x3UL << ADC_JSQL_POS) /*!< 0x00200000 */
/*******************  Bit definition for ADC_JDR1 register  *******************/
#define ADC_JDR1_JDATA ADC_JDATA1_MASK /*!< ADC group injected sequencer rank 1 conversion data */

/*******************  Bit definition for ADC_JDR2 register  *******************/
#define ADC_JDR2_JDATA ADC_JDATA2_MASK /*!< ADC group injected sequencer rank 2 conversion data */

/*******************  Bit definition for ADC_JDR3 register  *******************/
#define ADC_JDR3_JDATA ADC_JDATA3_MASK /*!< ADC group injected sequencer rank 3 conversion data */

/*******************  Bit definition for ADC_JDR4 register  *******************/
#define ADC_JDR4_JDATA ADC_JDATA4_MASK /*!< ADC group injected sequencer rank 4 conversion data */

/********************  Bit definition for ADC_DR register  ********************/
#define ADC_RDR_DATA ADC_RDATA_MASK /*!< ADC group regular conversion data */

/********************  Bit definition for ADC_DR register  ********************/
#define ADC_RDR_DATA ADC_RDATA_MASK /*!< ADC group regular conversion data */


/** 
  * @brief  Structure definition of ADC and regular group initialization 
  * @note   Parameters of this structure are shared within 2 scopes:
  *          - Scope entire ADC (affects regular and injected groups): DataAlign, ScanConvMode.
  *          - Scope regular group: ContinuousConvMode, NbrOfConversion, DiscontinuousConvMode, NbrOfDiscConversion, ExternalTrigConvEdge, ExternalTrigConv.
  * @note   The setting of these parameters with function HAL_ADC_Init() is conditioned to ADC state.
  *         ADC can be either disabled or enabled without conversion on going on regular group.
  */
typedef struct
{
    uint32_t DataAlign;                    /*!< Specifies ADC data alignment to right (MSB on register bit 11 and LSB on register bit 0) (default setting)
                                                  or to left (if regular group: MSB on register bit 15 and LSB on register bit 4, if injected group (MSB kept as signed value due to potential negative value after offset application): MSB on register bit 14 and LSB on register bit 3).
                                                  This parameter can be a value of @ref ADC_Data_align */
    uint32_t ScanConvMode;                 /*!< Configures the sequencer of regular and injected groups.
                                                  This parameter can be associated to parameter 'DiscontinuousConvMode' to have main sequence subdivided in successive parts.
                                                  If disabled: Conversion is performed in single mode (one channel converted, the one defined in rank 1).
                                                               Parameters 'NbrOfConversion' and 'InjectedNbrOfConversion' are discarded (equivalent to set to 1).
                                                  If enabled:  Conversions are performed in sequence mode (multiple ranks defined by 'NbrOfConversion'/'InjectedNbrOfConversion' and each channel rank).
                                                               Scan direction is upward: from rank1 to rank 'n'.
                                                  This parameter can be a value of @ref ADC_Scan_mode
                                                  Note: For regular group, this parameter should be enabled in conversion either by polling (HAL_ADC_Start with Discontinuous mode and NbrOfDiscConversion=1)
                                                        or by DMA (HAL_ADC_Start_DMA), but not by interruption (HAL_ADC_Start_IT): in scan mode, interruption is triggered only on the
                                                        the last conversion of the sequence. All previous conversions would be overwritten by the last one.
                                                        Injected group used with scan mode has not this constraint: each rank has its own result register, no data is overwritten. */
    FunctionalState ContinuousConvMode;    /*!< Specifies whether the conversion is performed in single mode (one conversion) or continuous mode for regular group,
                                                  after the selected trigger occurred (software start or external trigger).
                                                  This parameter can be set to ENABLE or DISABLE. */
    uint32_t NbrOfConversion;              /*!< Specifies the number of ranks that will be converted within the regular group sequencer.
                                                  To use regular group sequencer and convert several ranks, parameter 'ScanConvMode' must be enabled.
                                                  This parameter must be a number between Min_Data = 1 and Max_Data = 16. */
    FunctionalState DiscontinuousConvMode; /*!< Specifies whether the conversions sequence of regular group is performed in Complete-sequence/Discontinuous-sequence (main sequence subdivided in successive parts).
                                                  Discontinuous mode is used only if sequencer is enabled (parameter 'ScanConvMode'). If sequencer is disabled, this parameter is discarded.
                                                  Discontinuous mode can be enabled only if continuous mode is disabled. If continuous mode is enabled, this parameter setting is discarded.
                                                  This parameter can be set to ENABLE or DISABLE. */
    uint32_t NbrOfDiscConversion;          /*!< Specifies the number of discontinuous conversions in which the  main sequence of regular group (parameter NbrOfConversion) will be subdivided.
                                                  If parameter 'DiscontinuousConvMode' is disabled, this parameter is discarded.
                                                  This parameter must be a number between Min_Data = 1 and Max_Data = 8. */
    uint32_t ExternalTrigConv;             /*!< Selects the external event used to trigger the conversion start of regular group.
                                                  If set to ADC_SOFTWARE_START, external triggers are disabled.
                                                  If set to external trigger source, triggering is on event rising edge.
                                                  This parameter can be a value of @ref ADC_External_trigger_source_Regular */
} ADC_InitTypeDef;

/** 
  * @brief  Structure definition of ADC channel for regular group   
  * @note   The setting of these parameters with function HAL_ADC_ConfigChannel() is conditioned to ADC state.
  *         ADC can be either disabled or enabled without conversion on going on regular group.
  */
typedef struct
{
    uint32_t Channel;      /*!< Specifies the channel to configure into ADC regular group.
                                        This parameter can be a value of @ref ADC_channels
                                        Note: Depending on devices, some channels may not be available on package pins. Refer to device datasheet for channels availability.
                                        Note: On  devices with several ADC: Only ADC1 can access internal measurement channels (VrefInt/TempSensor) 
                                        Note: On 0xx8 and 0xxB devices: A low-amplitude voltage glitch may be generated (on ADC input 0) on the PA0 pin, when the ADC is converting with injection trigger.
                                              It is advised to distribute the analog channels so that Channel 0 is configured as an injected channel.
                                              Refer to errata sheet of these devices for more details. */
    uint32_t Rank;         /*!< Specifies the rank in the regular group sequencer 
                                        This parameter can be a value of @ref ADC_regular_rank
                                        Note: In case of need to disable a channel or change order of conversion sequencer, rank containing a previous channel setting can be overwritten by the new channel setting (or parameter number of conversions can be adjusted) */
    uint32_t SamplingTime; /*!< Sampling time value to be set for the selected channel.
                                        Unit: ADC clock cycles
                                        Conversion time is the addition of sampling time and processing time (12.5 ADC clock cycles at ADC resolution 12 bits).
                                        This parameter can be a value of @ref ADC_sampling_times
                                        Caution: This parameter updates the parameter property of the channel, that can be used into regular and/or injected groups.
                                                 If this same channel has been previously configured in the other group (regular/injected), it will be updated to last setting.
                                        Note: In case of usage of internal measurement channels (VrefInt/TempSensor),
                                              sampling time constraints must be respected (sampling time can be adjusted in function of ADC clock frequency and sampling time setting)
                                              Refer to device datasheet for timings values, parameters TS_vrefint, TS_temp (values rough order: 5us to 17.1us min). */
} ADC_ChannelConfTypeDef;

/**
  * @brief  ADC Configuration analog watchdog definition
  * @note   The setting of these parameters with function is conditioned to ADC state.
  *         ADC state can be either disabled or enabled without conversion on going on regular and injected groups.
  */
typedef struct
{
    uint32_t WatchdogMode;   /*!< Configures the ADC analog watchdog mode: single/all channels, regular/injected group.
                                   This parameter can be a value of @ref ADC_analog_watchdog_mode. */
    uint32_t Channel;        /*!< Selects which ADC channel to monitor by analog watchdog.
                                   This parameter has an effect only if watchdog mode is configured on single channel (parameter WatchdogMode)
                                   This parameter can be a value of @ref ADC_channels. */
    FunctionalState ITMode;  /*!< Specifies whether the analog watchdog is configured in interrupt or polling mode.
                                   This parameter can be set to ENABLE or DISABLE */
    uint32_t HighThreshold;  /*!< Configures the ADC analog watchdog High threshold value.
                                   This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */
    uint32_t LowThreshold;   /*!< Configures the ADC analog watchdog High threshold value.
                                   This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */
    uint32_t WatchdogNumber; /*!< Reserved for future use, can be set to 0 */
} ADC_AnalogWDGConfTypeDef;

/** 
  * @brief  HAL ADC state machine: ADC states definition (bitfields)
  */
/* States of ADC global scope */
#define HAL_ADC_STATE_RESET 0x00000000U         /*!< ADC not yet initialized or disabled */
#define HAL_ADC_STATE_READY 0x00000001U         /*!< ADC peripheral ready for use */
#define HAL_ADC_STATE_BUSY_INTERNAL 0x00000002U /*!< ADC is busy to internal process (initialization, calibration) */
#define HAL_ADC_STATE_TIMEOUT 0x00000004U       /*!< TimeOut occurrence */

/* States of ADC errors */
#define HAL_ADC_STATE_ERROR_INTERNAL 0x00000010U /*!< Internal error occurrence */
#define HAL_ADC_STATE_ERROR_CONFIG 0x00000020U   /*!< Configuration error occurrence */
#define HAL_ADC_STATE_ERROR_DMA 0x00000040U      /*!< DMA error occurrence */

/* States of ADC group regular */
#define HAL_ADC_STATE_REG_BUSY 0x00000100U  /*!< A conversion on group regular is ongoing or can occur (either by continuous mode, \
                                                external trigger, low power auto power-on, multimode ADC master control) */
#define HAL_ADC_STATE_REG_EOC 0x00000200U   /*!< Conversion data available on group regular */
#define HAL_ADC_STATE_REG_OVR 0x00000400U   /*!< Not available on  device: Overrun occurrence */
#define HAL_ADC_STATE_REG_EOSMP 0x00000800U /*!< Not available on  device: End Of Sampling flag raised  */

/* States of ADC group injected */
#define HAL_ADC_STATE_INJ_BUSY 0x00001000U  /*!< A conversion on group injected is ongoing or can occur (either by auto-injection mode, \
                                                external trigger, low power auto power-on, multimode ADC master control) */
#define HAL_ADC_STATE_INJ_EOC 0x00002000U   /*!< Conversion data available on group injected */
#define HAL_ADC_STATE_INJ_JQOVF 0x00004000U /*!< Not available on  device: Injected queue overflow occurrence */

/* States of ADC analog watchdogs */
#define HAL_ADC_STATE_AWD1 0x00010000U /*!< Out-of-window occurrence of analog watchdog 1 */
#define HAL_ADC_STATE_AWD2 0x00020000U /*!< Not available on  device: Out-of-window occurrence of analog watchdog 2 */
#define HAL_ADC_STATE_AWD3 0x00040000U /*!< Not available on  device: Out-of-window occurrence of analog watchdog 3 */

/* States of ADC multi-mode */
#define HAL_ADC_STATE_MULTIMODE_SLAVE 0x00100000U /*!< ADC in multimode slave state, controlled by another ADC master ( */

/**
  * @brief  ADC handle Structure definition  
  */
typedef struct __ADC_HandleTypeDef
{
    reg_adc_t *Instance; /*!< Register base address */

    ADC_InitTypeDef Init; /*!< ADC required parameters */

    //  DMA_HandleTypeDef             *DMA_Handle;            /*!< Pointer DMA Handler */

    HAL_LockTypeDef Lock; /*!< ADC locking object */

    volatile uint32_t State; /*!< ADC communication state (bitmap of ADC states) */

    volatile uint32_t ErrorCode; /*!< ADC Error code */

    void (*ConvCpltCallback)(struct __ADC_HandleTypeDef *hadc);                                                                 /*!< ADC conversion complete callback */
    void (*ConvHalfCpltCallback)(struct __ADC_HandleTypeDef *hadc);                                                             /*!< ADC conversion DMA half-transfer callback */
    void (*LevelOutOfWindowCallback)(struct __ADC_HandleTypeDef *hadc);                                                         /*!< ADC analog watchdog 1 callback */
    void (*ErrorCallback)(struct __ADC_HandleTypeDef *hadc);                                                                    /*!< ADC error callback */
    void (*InjectedConvCpltCallback)(struct __ADC_HandleTypeDef *hadc); /*!< ADC group injected conversion complete callback */ /*!< ADC end of sampling callback */
    void (*MspInitCallback)(struct __ADC_HandleTypeDef *hadc);                                                                  /*!< ADC Msp Init callback */
    void (*MspDeInitCallback)(struct __ADC_HandleTypeDef *hadc);                                                                /*!< ADC Msp DeInit callback */
} ADC_HandleTypeDef;

/**
  * @brief  HAL ADC Callback ID enumeration definition
  */
typedef enum
{
    HAL_ADC_CONVERSION_COMPLETE_CB_ID = 0x00U,     /*!< ADC conversion complete callback ID */
    HAL_ADC_CONVERSION_HALF_CB_ID = 0x01U,         /*!< ADC conversion DMA half-transfer callback ID */
    HAL_ADC_LEVEL_OUT_OF_WINDOW_1_CB_ID = 0x02U,   /*!< ADC analog watchdog 1 callback ID */
    HAL_ADC_ERROR_CB_ID = 0x03U,                   /*!< ADC error callback ID */
    HAL_ADC_INJ_CONVERSION_COMPLETE_CB_ID = 0x04U, /*!< ADC group injected conversion complete callback ID */
    HAL_ADC_MSPINIT_CB_ID = 0x09U,                 /*!< ADC Msp Init callback ID          */
    HAL_ADC_MSPDEINIT_CB_ID = 0x0AU                /*!< ADC Msp DeInit callback ID        */
} HAL_ADC_CallbackIDTypeDef;

/**
  * @brief  HAL ADC Callback pointer definition
  */
typedef void (*pADC_CallbackTypeDef)(ADC_HandleTypeDef *hadc); /*!< pointer to a ADC callback function */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup ADC_Exported_Constants ADC Exported Constants
  * @{
  */

/** @defgroup ADC_Error_Code ADC Error Code
  * @{
  */
#define HAL_ADC_ERROR_NONE 0x00U     /*!< No error                                              */
#define HAL_ADC_ERROR_INTERNAL 0x01U /*!< ADC IP internal error: if problem of clocking, enable/disable, erroneous state                       */
#define HAL_ADC_ERROR_OVR 0x02U      /*!< Overrun error                                         */
#define HAL_ADC_ERROR_DMA 0x04U      /*!< DMA transfer error                                    */

#define HAL_ADC_ERROR_INVALID_CALLBACK (0x10U) /*!< Invalid Callback error */
/**
  * @}
  */

/** @defgroup ADC_Data_align ADC data alignment
  * @{
  */
#define ADC_DATAALIGN_RIGHT 0x00000000U
#define ADC_DATAALIGN_LEFT ((uint32_t)ADC_ALIGN_MASK)
/**
  * @}
  */

/** @defgroup ADC_Scan_mode ADC scan mode
  * @{
  */
/* Note: Scan mode values are not among binary choices ENABLE/DISABLE for     */
/*       compatibility with other devices having a sequencer with       */
/*       additional options.                                                  */
#define ADC_SCAN_DISABLE 0x00000000U
#define ADC_SCAN_ENABLE ((uint32_t)ADC_SCAN_MASK)
/**
  * @}
  */

/** @defgroup ADC_External_trigger_edge_Regular ADC external trigger enable for regular group
  * @{
  */
//#define ADC_EXTERNALTRIGCONVEDGE_NONE           0x00000000U
//#define ADC_EXTERNALTRIGCONVEDGE_RISING         ((uint32_t)ADC_CR2_EXTTRIG)
/**
  * @}
  */
#define ADC_SOFTWARE_START ADC_SWSTART
/** @defgroup ADC_channels ADC channels
  * @{
  */
/* Note: Depending on devices, some channels may not be available on package  */
/*       pins. Refer to device datasheet for channels availability.           */
#define ADC_CHANNEL_0 0x00000000U
#define ADC_CHANNEL_1 ((uint32_t)(ADC_RSQR1_RSQ1_0))
#define ADC_CHANNEL_2 ((uint32_t)(ADC_RSQR1_RSQ1_1))
#define ADC_CHANNEL_3 ((uint32_t)(ADC_RSQR1_RSQ1_1 | ADC_RSQR1_RSQ1_0))
#define ADC_CHANNEL_4 ((uint32_t)(ADC_RSQR1_RSQ1_2))
#define ADC_CHANNEL_5 ((uint32_t)(ADC_RSQR1_RSQ1_2 | ADC_RSQR1_RSQ1_0))
#define ADC_CHANNEL_6 ((uint32_t)(ADC_RSQR1_RSQ1_2 | ADC_RSQR1_RSQ1_1))
#define ADC_CHANNEL_7 ((uint32_t)(ADC_RSQR1_RSQ1_2 | ADC_RSQR1_RSQ1_1 | ADC_RSQR1_RSQ1_0))
#define ADC_CHANNEL_8 ((uint32_t)(ADC_RSQR1_RSQ1_3))
#define ADC_CHANNEL_9 ((uint32_t)(ADC_RSQR1_RSQ1_3 | ADC_RSQR1_RSQ1_0))
#define ADC_CHANNEL_10 ((uint32_t)(ADC_RSQR1_RSQ1_3 | ADC_RSQR1_RSQ1_1))
#define ADC_CHANNEL_11 ((uint32_t)(ADC_RSQR1_RSQ1_3 | ADC_RSQR1_RSQ1_1 | ADC_RSQR1_RSQ1_0))
#define ADC_CHANNEL_12 ((uint32_t)(ADC_RSQR1_RSQ1_3 | ADC_RSQR1_RSQ1_2))
#define ADC_CHANNEL_13 ((uint32_t)(ADC_RSQR1_RSQ1_3 | ADC_RSQR1_RSQ1_2 | ADC_RSQR1_RSQ1_0))
#define ADC_CHANNEL_14 ((uint32_t)(ADC_RSQR1_RSQ1_3 | ADC_RSQR1_RSQ1_2 | ADC_RSQR1_RSQ1_1))
#define ADC_CHANNEL_15 ((uint32_t)(ADC_RSQR1_RSQ1_3 | ADC_RSQR1_RSQ1_2 | ADC_RSQR1_RSQ1_1 | ADC_RSQR1_RSQ1_0))

//#define ADC_CHANNEL_TEMPSENSOR          ADC_CHANNEL_16  /* ADC internal channel (no connection on device pin) */
//#define ADC_CHANNEL_VREFINT             ADC_CHANNEL_17  /* ADC internal channel (no connection on device pin) */
#define ADC_CHANNEL_TEMPSENSOR_VBAT       ADC_CHANNEL_10  /* ADC internal channel (no connection on device pin) */
/**
  * @}
  */

/** @defgroup ADC_sampling_times ADC sampling times
  * @{
  */
#define ADC_SAMPLETIME_1CYCLE 0x00000000U                                           /*!< Sampling time 1 ADC clock cycle */
#define ADC_SAMPLETIME_2CYCLES ((uint32_t)(ADC_SMPR1_SMP10_0))                      /*!< Sampling time 2 ADC clock cycles */
#define ADC_SAMPLETIME_4CYCLES ((uint32_t)(ADC_SMPR1_SMP10_1))                      /*!< Sampling time 4 ADC clock cycles */
#define ADC_SAMPLETIME_15CYCLES ((uint32_t)(ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0)) /*!< Sampling time 15 ADC clock cycles */
/**
  * @}
  */

/** @defgroup ADC_conversion_group ADC conversion group
  * @{
  */
#define ADC_REGULAR_GROUP ((uint32_t)(ADC_FLAG_EOC))
#define ADC_INJECTED_GROUP ((uint32_t)(ADC_FLAG_JEOC))
#define ADC_REGULAR_INJECTED_GROUP ((uint32_t)(ADC_FLAG_EOC | ADC_FLAG_JEOC))
/**
  * @}
  */

/** @defgroup ADC_Event_type ADC Event type
  * @{
  */
#define ADC_AWD_EVENT ((uint32_t)ADC_FLAG_AWD) /*!< ADC Analog watchdog event */

#define ADC_AWD1_EVENT ADC_AWD_EVENT /*!< ADC Analog watchdog 1 event: Alternate naming for compatibility with other devices having several analog watchdogs */
/**
  * @}
  */

/** @defgroup ADC_interrupts_definition ADC interrupts definition
  * @{
  */
#define ADC_IT_EOC ADC_CR1_REOCIE /*!< ADC End of Regular Conversion interrupt source */
//#define ADC_IT_JEOC          ADC_CR1_JEOSIE       /*!< ADC End of Injected Conversion interrupt source */
//#define ADC_IT_AWD           ADC_CR1_AWDIE        /*!< ADC Analog watchdog interrupt source */
/**
  * @}
  */

/** @defgroup ADC_flags_definition ADC flags definition
  * @{
  */
#define ADC_FLAG_STRT ADC_SR_STRT   /*!< ADC Regular group start flag */
#define ADC_FLAG_JSTRT ADC_SR_JSTRT /*!< ADC Injected group start flag */
#define ADC_FLAG_EOC ADC_SR_EOC     /*!< ADC End of Regular conversion flag */
#define ADC_FLAG_JEOC ADC_SR_JEOC   /*!< ADC End of Injected conversion flag */
#define ADC_FLAG_AWD ADC_SR_AWD     /*!< ADC Analog watchdog flag */
/**
  * @}
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/

/** @addtogroup ADC_Private_Constants ADC Private Constants
  * @{
  */
#define ADC_SWSTART ((uint32_t)(ADC_CR2_RTRIG))
/**
  * @}
  */
/**
  * @}
  */

/* Combination of all post-conversion flags bits: EOC/EOS, JEOC/JEOS, OVR, AWDx */
#define ADC_FLAG_POSTCONV_ALL (ADC_FLAG_EOC | ADC_FLAG_JEOC | ADC_FLAG_AWD)

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup ADC_Exported_Macros ADC Exported Macros
  * @{
  */
/* Macro for internal HAL driver usage, and possibly can be used into code of */
/* final user.                                                                */

/**
  * @brief Enable the ADC peripheral
  * @note ADC enable requires a delay for ADC stabilization time
  *       (refer to device datasheet, parameter tSTAB)
  * @note On , if ADC is already enabled this macro trigs a conversion 
  *       SW start on regular group.
  * @param __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_ENABLE(__HANDLE__) \
    (SET_BIT((__HANDLE__)->Instance->CR2, (ADC_CR2_ADON)))

/**
  * @brief Disable the ADC peripheral
  * @param __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_DISABLE(__HANDLE__) \
    (CLEAR_BIT((__HANDLE__)->Instance->CR2, (ADC_CR2_ADON)))

/** @brief Enable the ADC end of conversion interrupt.
  * @param __HANDLE__: ADC handle
  * @param __INTERRUPT__: ADC Interrupt
  *          This parameter can be any combination of the following values:
  *            @arg ADC_IT_EOC: ADC End of Regular Conversion interrupt source
  *            @arg ADC_IT_JEOC: ADC End of Injected Conversion interrupt source
  *            @arg ADC_IT_AWD: ADC Analog watchdog interrupt source
  * @retval None
  */
#define __HAL_ADC_ENABLE_IT(__HANDLE__, __INTERRUPT__) \
    (SET_BIT((__HANDLE__)->Instance->CR1, (__INTERRUPT__)))

/** @brief Disable the ADC end of conversion interrupt.
  * @param __HANDLE__: ADC handle
  * @param __INTERRUPT__: ADC Interrupt
  *          This parameter can be any combination of the following values:
  *            @arg ADC_IT_EOC: ADC End of Regular Conversion interrupt source
  *            @arg ADC_IT_JEOC: ADC End of Injected Conversion interrupt source
  *            @arg ADC_IT_AWD: ADC Analog watchdog interrupt source
  * @retval None
  */
#define __HAL_ADC_DISABLE_IT(__HANDLE__, __INTERRUPT__) \
    (CLEAR_BIT((__HANDLE__)->Instance->CR1, (__INTERRUPT__)))

/** @brief  Checks if the specified ADC interrupt source is enabled or disabled.
  * @param __HANDLE__: ADC handle
  * @param __INTERRUPT__: ADC interrupt source to check
  *          This parameter can be any combination of the following values:
  *            @arg ADC_IT_EOC: ADC End of Regular Conversion interrupt source
  *            @arg ADC_IT_JEOC: ADC End of Injected Conversion interrupt source
  *            @arg ADC_IT_AWD: ADC Analog watchdog interrupt source
  * @retval None
  */
#define __HAL_ADC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) \
    (((__HANDLE__)->Instance->CR1 & (__INTERRUPT__)) == (__INTERRUPT__))

/** @brief Get the selected ADC's flag status.
  * @param __HANDLE__: ADC handle
  * @param __FLAG__: ADC flag
  *          This parameter can be any combination of the following values:
  *            @arg ADC_FLAG_STRT: ADC Regular group start flag
  *            @arg ADC_FLAG_JSTRT: ADC Injected group start flag
  *            @arg ADC_FLAG_EOC: ADC End of Regular conversion flag
  *            @arg ADC_FLAG_JEOC: ADC End of Injected conversion flag
  *            @arg ADC_FLAG_AWD: ADC Analog watchdog flag
  * @retval None
  */

/** @brief Get the selected ADC's flag status.
  * @param __HANDLE__: ADC handle
  * @param __FLAG__: ADC flag
  *          This parameter can be any combination of the following values:
  *            @arg ADC_FLAG_STRT: ADC Regular group start flag
  *            @arg ADC_FLAG_JSTRT: ADC Injected group start flag
  *            @arg ADC_FLAG_EOC: ADC End of Regular conversion flag
  *            @arg ADC_FLAG_JEOC: ADC End of Injected conversion flag
  *            @arg ADC_FLAG_AWD: ADC Analog watchdog flag
  * @retval None
  */
#define __HAL_ADC_GET_FLAG(__HANDLE__, __FLAG__) \
    ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief Clear the ADC's pending flags
  * @param __HANDLE__: ADC handle
  * @param __FLAG__: ADC flag
  *          This parameter can be any combination of the following values:
  *            @arg ADC_FLAG_STRT: ADC Regular group start flag
  *            @arg ADC_FLAG_JSTRT: ADC Injected group start flag
  *            @arg ADC_FLAG_EOC: ADC End of Regular conversion flag
  *            @arg ADC_FLAG_JEOC: ADC End of Injected conversion flag
  *            @arg ADC_FLAG_AWD: ADC Analog watchdog flag
  * @retval None
  */
#define __HAL_ADC_CLEAR_FLAG(__HANDLE__, __FLAG__) \
    (WRITE_REG((__HANDLE__)->Instance->SFCR, ~(__FLAG__)))

/** @brief  Reset ADC handle state
  * @param  __HANDLE__: ADC handle
  * @retval None
  */

#define __HAL_ADC_RESET_HANDLE_STATE(__HANDLE__)   \
    do                                             \
    {                                              \
        (__HANDLE__)->State = HAL_ADC_STATE_RESET; \
        (__HANDLE__)->MspInitCallback = NULL;      \
        (__HANDLE__)->MspDeInitCallback = NULL;    \
    } while (0)

/**
  * @}
  */

/* Private macro ------------------------------------------------------------*/

/** @defgroup ADC_Private_Macros ADC Private Macros
  * @{
  */
/* Macro reserved for internal HAL driver usage, not intended to be used in   */
/* code of final user.                                                        */

/**
  * @brief Verification of ADC state: enabled or disabled
  * @param __HANDLE__: ADC handle
  * @retval SET (ADC enabled) or RESET (ADC disabled)
  */
#define ADC_IS_ENABLE(__HANDLE__) \
    (((((__HANDLE__)->Instance->CR2 & ADC_CR2_ADON) == ADC_CR2_ADON)) ? SET : RESET)

/**
  * @brief Test if conversion trigger of regular group is software start
  *        or external trigger.
  * @param __HANDLE__: ADC handle
  * @retval SET (software start) or RESET (external trigger)
  */
#define ADC_IS_SOFTWARE_START_REGULAR(__HANDLE__) \
    (READ_BIT((__HANDLE__)->Instance->CR2, ADC_RTRIG_MASK) == ADC_SOFTWARE_START)

/**
  * @brief Test if conversion trigger of injected group is software start
  *        or external trigger.
  * @param __HANDLE__: ADC handle
  * @retval SET (software start) or RESET (external trigger)
  */
#define ADC_IS_SOFTWARE_START_INJECTED(__HANDLE__) \
    (READ_BIT((__HANDLE__)->Instance->CR2, ADC_CR2_JEXTSEL) == ADC_INJECTED_SOFTWARE_START)

/**
  * @brief Clear ADC error code (set it to error code: "no error")
  * @param __HANDLE__: ADC handle
  * @retval None
  */
#define ADC_CLEAR_ERRORCODE(__HANDLE__) \
    ((__HANDLE__)->ErrorCode = HAL_ADC_ERROR_NONE)

/**
  * @brief Set the ADC's sample time for channel numbers between 0 and 11.
  * @param _SAMPLETIME_: Sample time parameter.
  * @param _CHANNELNB_: Channel number.  
  * @retval None
  */
#define ADC_SMPR1(_SAMPLETIME_, _CHANNELNB_) \
    ((_SAMPLETIME_) << (ADC_SMP1_POS * (_CHANNELNB_)))

/**
  * @brief Set the reg sequence length.
  * @param _RSQR_RL_: Sequence length.
  * @retval None
  */
#define ADC_RSQR_RL_SHIFT(_RSQR_RL_) \
    (((_RSQR_RL_)-1) << ADC_RSQL_POS)

/**
  * @brief Set the injected sequence length.
  * @param _JSQR_JL_: Sequence length.
  * @retval None
  */
#define ADC_JSQR_JL_SHIFT(_JSQR_JL_) \
    (((_JSQR_JL_)-1) << ADC_JSQL_POS)

/**
  * @brief Enable ADC continuous conversion mode.
  * @param _CONTINUOUS_MODE_: Continuous mode.
  * @retval None
  */
#define ADC_CR2_CONTINUOUS(_CONTINUOUS_MODE_) \
    ((_CONTINUOUS_MODE_) << ADC_CONT_POS)

/**
  * @brief Configures the number of discontinuous conversions for the regular group channels.
  * @param _NBR_DISCONTINUOUS_CONV_: Number of discontinuous conversions.
  * @retval None
  */
#define ADC_CR1_DISCONTINUOUS_NUM(_NBR_DISCONTINUOUS_CONV_) \
    (((_NBR_DISCONTINUOUS_CONV_)-1) << ADC_DISCNUM_POS)

/**
  * @brief Enable ADC scan mode to convert multiple ranks with sequencer.
  * @param _SCAN_MODE_: Scan conversion mode.
  * @retval None
  */
/* Note: Scan mode is compared to ENABLE for legacy purpose, this parameter   */
/*       is equivalent to ADC_SCAN_ENABLE.                                    */
#define ADC_CR1_SCAN_SET(_SCAN_MODE_) \
    ((((_SCAN_MODE_) == ADC_SCAN_ENABLE) || ((_SCAN_MODE_) == ENABLE)) ? (ADC_SCAN_ENABLE) : (ADC_SCAN_DISABLE))

#define IS_ADC_DATA_ALIGN(ALIGN) (((ALIGN) == ADC_DATAALIGN_RIGHT) || \
                                  ((ALIGN) == ADC_DATAALIGN_LEFT))

#define IS_ADC_SCAN_MODE(SCAN_MODE) (((SCAN_MODE) == ADC_SCAN_DISABLE) || \
                                     ((SCAN_MODE) == ADC_SCAN_ENABLE))

#define IS_ADC_CONVERSION_GROUP(CONVERSION) (((CONVERSION) == ADC_REGULAR_GROUP) ||  \
                                             ((CONVERSION) == ADC_INJECTED_GROUP) || \
                                             ((CONVERSION) == ADC_REGULAR_INJECTED_GROUP))

#define IS_ADC_EVENT_TYPE(EVENT) ((EVENT) == ADC_AWD_EVENT)

/**
  * @}
  */

/** @defgroup ADC_regular_nb_conv_verification ADC regular nb conv verification
  * @{
  */
#define IS_ADC_REGULAR_NB_CONV(LENGTH) (((LENGTH) >= 1U) && ((LENGTH) <= 16U))
/**
  * @}
  */

/** @defgroup ADC_regular_discontinuous_mode_number_verification ADC regular discontinuous mode number verification
  * @{
  */
#define IS_ADC_REGULAR_DISCONT_NUMBER(NUMBER) (((NUMBER) >= 1U) && ((NUMBER) <= 8U))
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup ADC_Exported_Functions
  * @{
  */

/** @addtogroup ADC_Exported_Functions_Group1
  * @{
  */

/* Initialization and de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc);

/* Callbacks Register/UnRegister functions  ***********************************/
HAL_StatusTypeDef HAL_ADC_RegisterCallback(ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID, pADC_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_ADC_UnRegisterCallback(ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID);

/**
  * @}
  */

/* IO operation functions  *****************************************************/

/** @addtogroup ADC_Exported_Functions_Group2
  * @{
  */

/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout);
HAL_StatusTypeDef HAL_ADC_PollForEvent(ADC_HandleTypeDef *hadc, uint32_t EventType, uint32_t Timeout);

/* Non-blocking mode: Interruption */
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc);

/* ADC retrieve conversion value intended to be used with polling or interruption */
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);

/* ADC IRQHandler and Callbacks used in non-blocking modes (Interruption and DMA) */
void HAL_ADC_IRQHandler(ADC_HandleTypeDef *hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
/**
  * @}
  */

/* Peripheral Control functions ***********************************************/
/** @addtogroup ADC_Exported_Functions_Group3
  * @{
  */
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig);
/**
  * @}
  */

/* Peripheral State functions *************************************************/
/** @addtogroup ADC_Exported_Functions_Group4
  * @{
  */
uint32_t HAL_ADC_GetState(ADC_HandleTypeDef *hadc);
uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc);
/**
  * @}
  */

/**
  * @}
  */

/* Internal HAL driver functions **********************************************/
/** @addtogroup ADC_Private_Functions
  * @{
  */
HAL_StatusTypeDef ADC_Enable(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef ADC_ConversionStop_Disable(ADC_HandleTypeDef *hadc);
void ADC_StabilizationTime(uint32_t DelayUs);

void lsadc_init(void);
#endif //(_LSADC_H_)
