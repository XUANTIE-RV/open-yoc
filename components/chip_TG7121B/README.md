## 1.概述
​
TG7121B芯片驱动组件

## 2.组件安装
​

```bash
yoc init
yoc install chip_TG7121B
```
​
sdk_genie_mesh是SDK的入口，在安装sdk_genie_mesh时chip_TG7121B组件也会自动安装。
​

## 3.配置
​
配置文件：package.yaml

| 宏定义名称              | 功能说明  | 功能说明                             | 备注  |
| --------------------- | -------- | ------------------------------------| ---- |
| CONFIG_SYSTICK_HZ     | 1000     | 系统时钟定义                          |      |
| CONFIG_CSI_V1         | 1        | 指定使用的CSI接口，默认使用CSI1.0接口    |      |
| CONFIG_CSI            | "csi1"   | 指定使用的CSI接口，默认使用CSI1.0接口    |      |

## 4.接口列表与说明
​

TG7121B的大多数外设都已经对接HAL API，可以参考如下文档链接直接使用HAL API开发。
​

部分功能HAL API尚不支持，可以参考以下章节。
​

## 5.ADC接口与说明
### 5.1 初始化
#### 5.1.1 IO初始化
调用IO 的初始化接口，可以配置具体的IO的ADC模拟功能。
```c
void adc12b_in0_io_init(void);
void adc12b_in1_io_init(void);
void adc12b_in2_io_init(void);
void adc12b_in3_io_init(void);
void adc12b_in4_io_init(void);
void adc12b_in5_io_init(void);
void adc12b_in6_io_init(void);
void adc12b_in7_io_init(void);
void adc12b_in8_io_init(void);
```
每一个ADC的通道都有其对应的IO，不可随意映射。具体的对应关系如下：

| **GPIO** | **ANA_FUNC1** |
| --- | --- |
| GPIO_PB12 | ADC12B_AIN[0] |
| GPIO_PB13 | ADC12B_AIN[1] |
| GPIO_PC00 | ADC12B_AIN[2] |
| GPIO_PC01 | ADC12B_AIN[3] |
| GPIO_PA00 | ADC12B_AIN[4] |
| GPIO_PA01 | ADC12B_AIN[5] |
| GPIO_PA02 | ADC12B_AIN[6] |
| GPIO_PA03 | ADC12B_AIN[7] |
| GPIO_PA04 | ADC12B_AIN[8] |

另外ADC模块还有对应的内部通道，分布如下：：

| **内部输入信号** | **内部输入通道** |
| --- | --- |
| 芯片工作温度 | ADC12B_AIN[9] |
| 芯片工作电压 Vbat信号 | ADC12B_AIN[10] |
| 芯片内部ADC参考电压(标准1.4v) | ADC12B_AIN[11] |

#### 5.1.2 设置参数变量
设置ADC模块的参数变量，结构体原型如下：
```c
/**
  * @struct __ADC_HandleTypeDef
  * @brief  ADC handle type Structure definition
  */
typedef struct __ADC_HandleTypeDef
{
    reg_adc_t       *Instance; /*!< Register base address */
    ADC_InitTypeDef Init;
    void            *DMAC_Instance;
    union{
        struct AdcDMAEnv DMA;
        struct AdcInterruptEnv Interrupt;
    }Env;
    HAL_LockTypeDef Lock;
    volatile uint32_t State;
    volatile uint32_t ErrorCode;
} ADC_HandleTypeDef;
```
参数说明：

- ADC寄存器结构化处理(Instance)：目前TG7121B仅支持一个ADC，ADC 基地址 0x40012400
- ADC初始化处理(Init)：参考5.1.3。
- DMA寄存器结构化处理(DMAC_Instance)：参考5.1.4
- ADC转换的触发方式(Env)

ADC-DMA触发方式
设置DMA通道和Callback函数(规则组转换)
```c
/**
  * @struct AdcDMAEnv
  * @brief  ADC DMA Structure definition
  */
 struct AdcDMAEnv
{
    void                          (*Callback)();
    uint8_t                       DMA_Channel;
};
```
ADC中断单次触发方式
设置读取ADC数据的变量(单次转换)​
```c
/**
  * @struct AdcInterruptEnv
  * @brief  ADC Interrupt Structure definition
  */
struct AdcInterruptEnv
{
    uint8_t                       *pBuffPtr;      /*!< Pointer to ADC data Buffer */
    uint16_t                      XferCount;      /*!< UART ADC data Counter */
};
```
#### 5.1.3 ADC初始化
结构体原型如下：
```c
/**
  * @struct  ADC_InitTypeDef
  * @brief  Structure definition of ADC and regular group initialization
  * @note   Parameters of this structure are shared within 2 scopes:
  *          - Scope entire ADC (affects regular and injected groups): DataAlign, ScanConvMode.
  *          - Scope regular group: ContinuousConvMode, NbrOfConversion, DiscontinuousConvMode, NbrOfDiscConversion,ExternalTrigConv.
  * @note   The setting of these parameters with function HAL_ADC_Init() is conditioned to ADC state.
  *         ADC can be either disabled or enabled without conversion on going on regular group.
  */
typedef struct
{
    uint32_t DataAlign;
    uint32_t ScanConvMode;
    FunctionalState ContinuousConvMode;
    uint32_t NbrOfConversion;
    FunctionalState DiscontinuousConvMode;
    uint32_t NbrOfDiscConversion;
    uint32_t TrigType;
    uint32_t Vref;
    uint32_t AdcDriveType;
    uint32_t AdcCkDiv;
} ADC_InitTypeDef;
```
参数说明：

- 数据对齐(DataAlign)：
   - 默认情况，ADC 转换后的数据采用右对齐方式(bit11:0)。
   - 设置左对齐方式，ADC 转换后的数据：
   - 规则组转换数据寄存器(ADC_RDR)
   - 注入组转换数据寄存器((ADC_JDRx)： _Raw Converted Data + JnjectOffset_
- 扫描模式(ScanConvMode)：
   - 禁止

单通道单次转换
参数：NbrOfConversion无效
参数：NbrOfDiscConversion无效

   - 使能

会扫描所有规则通道。
与ContinuousConvMode的联动：
使能ContinuousConvMode，会连续采集所有通道，从rank1开始扫描，到最后一个rank。
禁止ContinuousConvMode，只会扫描一轮，从rank1开始扫描，到最后一个rank。

- 连续转换模式(ContinuousConvMode)：
   - 禁止

单通道单次转换

   - 使能

连续多通道转换，与NbrOfConversion相对应。

- 连续转换的次数(NbrOfConversion)：
   - 规则组序列转换
   - 转换次数范围：1～12
   - 参数：ScanConvMode 必须使能
- 间断转换模式(DiscontinuousConvMode)
   - 规则组子序列转换
   - 参数：ScanConvMode 必须禁止
   - 参数：ContinuousConvMode 必须禁止
   - 具体每次采集的个数与(NbrOfDiscConversion)相对应。
- 间断转换的次数(NbrOfDiscConversion)
   - 规则组转换子序列数
   - 转换次数范围：≤8
   - 参数：DiscontinuousConvMode 必须使能
- 触发转换的方式(TrigType)
| **触发转换的方式** | **TrigType** |
| --- | --- |
| PIS | ADC_PIS_TRIG |
| 软件规则组触发 | ADC_REGULAR_SOFTWARE_TRIGT |
| 软件注入组触发 | ADC_INJECTED_SOFTWARE_TRIGT |

- 选择参考电压(Vref)
| **选择参考电压** | **Vref** |
| --- | --- |
| 默认芯片内部1.4V为参考电压 | ADC_VREF_INSIDE |
| PA05输入参考电压 | ADC_VREF_EXPOWER |
| 芯片工作电压AVDD为参考电压 | ADC_VREF_VCC |

注意：当选择外部IO 为参考电压的时候，需要特殊配置PA05。

-  ADC通道的驱动方式(AdcDriveType)
| **ADC通道的驱动方式** | **AdcDriveType** |
| --- | --- |
| 输入信号经过输入buf运放驱动ADC | EINBUF_DRIVE_ADC |
| 输入信号1/3分压，并经过输入buf运放驱动ADC | INRES_ONETHIRD_EINBUF_DRIVE_ADC |
| 默认关闭输入buf运放，输入信号直接驱动ADC | BINBUF_DIRECT_DRIVE_ADC |

- ADC时钟分频系数(AdcCkDiv)

系统时钟按AdcCkDiv分频获得ADC运行时钟，默认ADC时钟为APBCLK的32分频，可以选择。
```c
#define ADC_CLOCK_DIV2          0x00000001U
#define ADC_CLOCK_DIV4          0x00000002U
#define ADC_CLOCK_DIV8          0x00000003U
#define ADC_CLOCK_DIV16         0x00000004U
#define ADC_CLOCK_DIV32         0x00000005U
#define ADC_CLOCK_DIV64         0x00000006U
#define ADC_CLOCK_DIV128        0x00000007U
```
#### 5.1.4 ADC采集通道初始化
##### 5.1.4.1 规则组转换参数配置
```c
/**
* @struct ADC_ChannelConfTypeDef
* @brief  Structure definition of ADC channel for regular group
* @note   The setting of these parameters with function HAL_ADC_ConfigChannel() is conditioned to ADC state.
*         ADC can be either disabled or enabled without conversion on going on regular group.
*/
typedef struct
{
  uint32_t    Channel;
  uint32_t    Rank;
  uint32_t    SamplingTime;
} ADC_ChannelConfTypeDef;
```
参数说明：

- 规则通道( Channel)

采样通道说明如下：
```c
#define ADC_CHANNEL_0               0x00000000U
#define ADC_CHANNEL_1               0x00000001U
#define ADC_CHANNEL_2               0x00000002U
#define ADC_CHANNEL_3               0x00000003U
#define ADC_CHANNEL_4               0x00000004U
#define ADC_CHANNEL_5               0x00000005U
#define ADC_CHANNEL_6               0x00000006U
#define ADC_CHANNEL_7               0x00000007U
#define ADC_CHANNEL_8               0x00000008U
#define ADC_CHANNEL_TEMPSENSOR      0x00000009U      /* ADC internal channel (no connection on device pin) */
#define ADC_CHANNEL_VBAT            0x0000000AU      /* ADC internal channel (no connection on device pin) */
#define ADC_CHANNEL_VREFINT         0x0000000BU      /* ADC internal channel (no connection on device pin) */
```

- 规则转换序列(Rank)

规则组序列说明如下：
```c
#define ADC_REGULAR_RANK_1          0x00000001U
#define ADC_REGULAR_RANK_2          0x00000002U
#define ADC_REGULAR_RANK_3          0x00000003U
#define ADC_REGULAR_RANK_4          0x00000004U
#define ADC_REGULAR_RANK_5          0x00000005U
#define ADC_REGULAR_RANK_6          0x00000006U
#define ADC_REGULAR_RANK_7          0x00000007U
#define ADC_REGULAR_RANK_8          0x00000008U
#define ADC_REGULAR_RANK_9          0x00000009U
#define ADC_REGULAR_RANK_10         0x0000000AU
#define ADC_REGULAR_RANK_11         0x0000000BU
#define ADC_REGULAR_RANK_12         0x0000000CU
```

- 规则转换采样周期(SamplingTime)
   - 采样周期说明：
```c
#define ADC_SAMPLETIME_1CYCLE        0x00000000U                 /*!< Sampling time 1 ADC clock cycle */
#define ADC_SAMPLETIME_2CYCLES       0x00000001U                 /*!< Sampling time 2 ADC clock cycles */
#define ADC_SAMPLETIME_4CYCLES       0x00000002U                 /*!< Sampling time 4 ADC clock cycles */
#define ADC_SAMPLETIME_15CYCLES      0x00000003U                 /*!< Sampling time 15 ADC clock cycles */
```

   - ADC 规则转换API函数
```c
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig);
```
##### 5.1.4.2 注入组转换参数配置
```c
/**
  * @struct ADC_InjectionConfTypeDef
  * @brief  ADC Configuration injected Channel structure definition
  * @note   Parameters of this structure are shared within 2 scopes:
  *          - Scope channel: InjectedChannel, InjectedRank, InjectedSamplingTime, InjectedOffset
  *          - Scope injected group (affects all channels of injected group): InjectedNbrOfConversion, InjectedDiscontinuousConvMode,
  *            AutoInjectedConv, ExternalTrigInjecConv.
  */
typedef struct
{
  uint32_t InjectedChannel;
  uint32_t InjectedRank;
  uint32_t InjectedSamplingTime;
  uint32_t InjectedOffset;
  uint32_t InjectedNbrOfConversion;
  FunctionalState InjectedDiscontinuousConvMode;
  FunctionalState AutoInjectedConv;
```
参数说明：

- 注入通道(InjectedChannel)与规则通道一致，请参考规则通道。
- 注入转换序列(Rank)

注入组序列说明：
```c
#define ADC_INJECTED_RANK_1                0x00000001U
#define ADC_INJECTED_RANK_2                0x00000002U
#define ADC_INJECTED_RANK_3                0x00000003U
#define ADC_INJECTED_RANK_4                0x00000004U
```

- 注入转换采样周期(SamplingTime)：与规则转换采样周期一致，请参考规则转换采样周期
- 注入转换数据偏移量(InjectedOffset)
   - 该偏移量为有符号数，其中bit11表示符号位
   - 注入组转换数据寄存器((ADC_JDRx)： _Raw Converted Data + JnjectOffset_
- 注入转换的次数(InjectedNbrOfConversion)
   - 注入转换序列子序列数
   - 范围：1～4
   - 参数：ScanConvMode 必须使能。
- 注入序列间断转换模式(InjectedDiscontinuousConvMode)
   - 参数：ScanConvMode 必须禁止
   - 参数：ContinuousConvMode 必须禁止
- 自动注入转换模式（AutoInjectedConv）
   - 参数：DiscontinuousConvMode 必须禁止
   - 参数：InjectedDiscontinuousConvMode必须禁止
- ADC注入转换API函数
```c
HAL_StatusTypeDef  HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef* hadc,ADC_InjectionConfTypeDef* sConfigInjected);
```
#### 5.1.5 初始化ADC模块
```c
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc);
```
### 5.2 反初始化
#### 5.2.1 反初始化ADC模块
通过反初始化函数，根据场景需求可以关闭ADC模块，可以降低系统的功耗。
```c
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
```
#### 5.2.2 反初始化IO
根据场景需求通过反初始化函数，可以关闭ADC模块，对应的模拟IO反初始为普通GPIO。
```c
void adc12b_in0_io_deinit(void);
void adc12b_in1_io_deinit(void);
void adc12b_in2_io_deinit(void);
void adc12b_in3_io_deinit(void);
void adc12b_in4_io_deinit(void);
void adc12b_in5_io_deinit(void);
void adc12b_in6_io_deinit(void);
void adc12b_in7_io_deinit(void);
void adc12b_in8_io_deinit(void);
```
由于ADC外部输入电压的不确定性，不好配置内部IO的状态，所以在使用ADC功能，在进入休眠之后，IO内部电平状态与外部输入电压易产生压差，导致出现部分漏电。
### 5.3 ADC模块采集数据
ADC模块采集数据一共提供了三种接口，规则通道采集、注入通道采集和DMA采集。在配置完初始化相关信息之后，需要调用相应的API接口，让ADC模块开始工作。
#### 5.3.1 规则通道
```c
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc);
```
#### 5.3.2 注入通道
```c
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef* hadc);
```
#### 5.3.3 DMA模式
```c
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint16_t* pData, uint32_t Length,void (*Callback)());
```
## 6.诊断错误码
​

无。
​

## 7.运行资源
​

## 8.依赖资源
​
- newlib
- aos
- aos_hal
- ble_host
- ble_mesh
- genie_cli
- csi
- hal_csi
- drivers
- genie_service
- kv
- drv_light
- rhino
- partition

​

## 9.组件参考
​
无。
