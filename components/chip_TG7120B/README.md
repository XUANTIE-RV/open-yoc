## Change log

无

## 1. 概述

TG7120B芯片组件，主要提供TG7120B芯片驱动代码。包含
- 芯片外设基地址、中断号等信息定义
- 包含GPIO、ADC、SPI Flash、TIMER、DMA、UART、IIC、SPI、Flash等外设CSI驱动代码
- 包含CPU启动文件
- 包含芯片初始化功能
- 包含芯片异常处理
- 包含芯片复位功能


## 2. 主要接口说明

### 2.1. GPIO

- 硬件设计上支持10个GPIO (SOP16封装)
- 多数gpio都支持输入输出、上下拉电阻、中断、唤醒、retention、fullmux，具体参考用户手册
- P1不支持fullmux。
- P2默认值为SW TMS引脚。
- P3默认值为SW TCK引脚。
- P16 P17目前只能接晶振不能做GPIO用。

其他接口说明:

#### 2.1.1. rentention配置接口

函数名称：void phy_gpio_retention_enable(gpio_pin_e pin, uint8_t en)
函数功能：打开或关闭gpio的retention功能。rentention指当系统休眠时，gpio作为输出其输出值可以保持的功能。

#### 2.1.2. fullmux配置接口

函数名称：void phy_gpio_fmux_set(gpio_pin_e pin, gpio_fmux_e type)
函数功能：配置gpio的fullmux功能。fullmux指可以将gpio复用为uart、spi、pwm、i2c等特殊用途引脚。

#### 2.1.3. 上下拉配置接口

函数名称：void phy_gpio_pull_set(gpio_pin_e pin, gpio_pupd_e type)
函数功能：配置gpio上下拉电阻。

#### 2.1.4. 唤醒配置接口

函数名称：void phy_gpio_wakeup_set(gpio_pin_e pin, gpio_polarity_e type)
函数功能：配置gpio的唤醒方式。

### 2.2. ADC

- 硬件支持bypass模式和attenuation模式，bypass模式量程为[0,0.8V],attenuation模式量程约为[0,3.2V]。
- 支持电源电压检测功能，即测量当前芯片供电电压。

其他接口说明:

#### 2.2.1. adc配置接口，无battery测量功能

函数名称：int32_t drv_adc_config(adc_handle_t handle, adc_conf_t *config)
函数功能：初始化adc，config中channel_array存放当前使能的adc通道。

#### 2.2.2. adc配置接口，有battery测量功能

函数名称：int32_t drv_adc_battery_config(adc_handle_t handle, adc_conf_t *config, uint32_t battery_channel_index)
函数功能：初始化adc，config中channel_array存放当前使能的adc通道。battery_channel_index为当前测量电源电压通道，该通道需要存放于config的channel_array中。

#### 2.2.3. 获取adc测量到的电压值

函数名称：int32_t drv_adc_read(adc_handle_t handle, uint32_t *data, uint32_t num)
函数功能：获取adc测量到的电压值，num为当前使能的通道数，data为测量到的电压值。

### 2.3. PWM

- 支持6路pwm。
- 时钟源16Mhz，每路pwm支持的分频有1、2、4、8、16、32、64、128。
- 硬件支持UP MODE和UP AND DOWN MODE。
- UP MODE频率计算：16/N/(TOP_VAL+1),N为分频(N=1,2,4,8,16,32,64,128)
- UP AND DOWN MODE频率计算：8/N/(TOP_VAL+1),N为分频(N=1,2,4,8,16,32,64,128)
- UP MODE，FALLIN方式，占空比为(CMP_VAL+1)/(TOP_VAL+1)，支持占空比0%~100%。 需要gpio输出高低来辅助实现。
- UP AND DOWN MODE，FALLING方式，占空比为CMP_VAL/TOP_VAL，不支持0%和100%。需要gpio输出高低来辅助实现。
- 当系统休眠时，pwm信息会丢失，唤醒后需要重新配置。
-  所有可fmux的io都可以复用为pwm。

其他接口说明:

#### 2.3.1. pwm模块初始化

函数名称：void phy_pwm_module_init(void)
函数功能：清零pwm寄存器，控制结构体初始化

#### 2.3.2. pwm模块去初始化

函数名称：void phy_pwm_module_deinit(void)
函数功能：关闭pwm输出，控制结构体恢复初始值

#### 2.3.3. 开启某一通道pwm

函数名称：void phy_pwm_ch_start(uint8_t ch)
函数功能：开启某一通道pwm

#### 2.3.4. 关闭某一通道pwm

函数名称：void phy_pwm_ch_stop(uint8_t ch)
函数功能：关闭某一通道pwm

#### 2.3.5. 获取当前pwm使能状态

函数名称：bool phy_pwm_ch_enable(PWMN_e pwmN)
函数功能：获取当前pwm使能状态

### 2.4. SPI

- 支持2路spi，可配置master或slave。
- 时钟等于hclk，可分频，不建议分频。
- 当系统休眠时，spi信息会丢失，唤醒后需要重新配置。
- 所有可fmux的io都可以复用为spi。
- 当使用spi发送数据时，可以选择自动或手动控制cs的高低，可以选择是否使用中断。所谓手动是通过将IO设置为GPIO并将其输出高低。spi_Cfg_t中的force_cs为true时选择手动模式，spi_Cfg_t中的int_mode为true时使用中断方式。

### 2.5. I2C

-  支持2路i2c。
- 时钟等于hclk，可分频，不建议分频。
-  当系统休眠时，i2c信息会丢失，唤醒后需要重新配置。
-  所有可fmux的io都可以复用为i2c。
-  i2c使用时需要接上拉电阻，比如2.2K。

### 2.6. UART

- 支持2路uart。
- 时钟等于hclk，可分频，不建议分频。
- 当系统休眠时，uart信息会丢失，唤醒后需要重新配置。
-  所有可fmux的io都可以复用为uart。
-  系统日志打印默认使用uart0(p9、p10)
-  假设当前系统主频为hclk且不分频，需要的波特率为baud，实际硬件配置的寄存器为divisor = (hclk) / (16 * baud)。当丢失的小数部分大于2%时，此波特率不支持会乱码。

### 2.7.  Timer

-  系统共有6个硬件timer，其中4个已经被协议栈、osal调度器等软件资源所使用，其余2个供应用使用。
-  时钟源固定4MHz，硬件不可分频。驱动中为了计算方便，软件将其4分频。
-  位宽为32bit，即最大计数值为0xFFFFFFFF。
-  支持中断方式和非中断方式。
-  支持free-running mode和user-defined count mode。前者减到0后，自动加载0xFFFFFFFF。后者减到0后，自动加载用户预先配置的值。SDK默认使用user-defined count mode。
- 当系统休眠时，timer信息会丢失，唤醒后需要重新配置。

### 2.8.  WDT

- watchdog时钟为32.768Khz
-  watchdog喂狗周期可选择2S、4S、8S、16S、32S、64S、128S、256S。
-  watchdog使用轮询模式。当选择轮询方式时，超过喂狗周期未喂狗，系统将产生复位。
- 当系统休眠时，watchdog信息会丢失，唤醒后需要重新配置。

### 2.9. DMA
- 支持4通道DMA

#### 2.9.1.dma中断函数
函数名称：void dw_dmac_irqhandler(int32_t idx)
函数功能：dma中断处理函数。

#### 2.9.2.获取dma空闲通道
函数名称：int32_t csi_dma_alloc_channel(void)
函数功能：获取dma空闲通道。

#### 2.9.3.释放dma通道
函数名称：void csi_dma_release_channel(int32_t ch)
函数功能：释放dma通道。

#### 2.9.4.配置dma通道
函数名称：int32_t csi_dma_config_channel(int32_t ch, dma_config_t *config, dma_event_cb_t cb_event, void *cb_arg)
函数功能：配置dma通道。

#### 2.9.5.启动dma传输
函数名称：void csi_dma_start(int32_t ch, void *psrcaddr,void *pdstaddr, uint32_t length)
函数功能：启动dma传输。

#### 2.9.6.停止dma通道。
函数名称：void csi_dma_stop(int32_t ch)
函数功能：停止dma通道。

#### 2.9.7.查询某通道dma状态
函数名称：dma_status_e csi_dma_get_status(int32_t ch)
函数功能：查询某通道dma状态

### 2.10. SPIFLASH

#### 2.10.1. spif初始化

函数名称：spiflash_handle_t csi_spiflash_initialize(int32_t idx, spiflash_event_cb_t cb_event)
函数功能：spif初始化。

#### 2.10.2. spif去初始化
函数名称：int32_t csi_spiflash_uninitialize(spiflash_handle_t handle)
函数功能：spif去初始化。

#### 2.10.3. 读flash
函数名称：int32_t csi_spiflash_read(spiflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
函数功能：读flash,读取cnt个word。

#### 2.10.4. 烧写flash
函数名称：int32_t csi_spiflash_program(spiflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
函数功能：烧写flash，烧写cnt个word。

#### 2.10.5. 擦除flash
函数名称：int32_t csi_spiflash_erase_sector(spiflash_handle_t handle, uint32_t addr)
函数功能：擦除1个扇区，该扇区地址是addr。

#### 2.10.6. 擦除flash
函数名称：int32_t csi_spiflash_erase_chip(spiflash_handle_t handle)
函数功能：擦除扇区，整片。

#### 2.10.7. 信息查询接口
函数名称：spiflash_info_t *csi_spiflash_get_info(spiflash_handle_t handle)
函数功能：获取flash当前信息。

#### 2.10.8. 状态查询接口
函数名称：spiflash_status_t csi_spiflash_get_status(spiflash_handle_t handle)
函数功能：获取flash当前状态。

#### 2.10.9. 功耗控制接口
函数名称：int32_t csi_spiflash_power_control(spiflash_handle_t handle, /*csi_power_stat_e*/ uint8_t state)
函数功能：配置flash进入deep sleep或者从deep sleep唤醒。

### 2.11. RF

#### 2.11.1. 头文件包含

```c
#include "rf_phy_driver.h"
```

#### 2.11.2. 发射功率设置

函数名称：void rf_phy_set_txPower(uint8 txPower)
函数功能：设置发射功率

参数说明:

txPower：发射功率，参考下面宏定义设置

| txPower：发射功率         | 参数值 | 备注 |
| ------------------------- | ------ | ---- |
| RF_PHY_TX_POWER_EXTRA_MAX | 0x3f   | 8dbm |
| RF_PHY_TX_POWER_5DBM      | 0x1d   |      |
| RF_PHY_TX_POWER_4DBM      | 0x17   |      |
| RF_PHY_TX_POWER_3DBM      | 0x15   |      |
| RF_PHY_TX_POWER_0DBM      | 0x0d   |      |
| RF_PHY_TX_POWER_N2DBM     | 0x0a   |      |
| RF_PHY_TX_POWER_N5DBM     | 0x06   |      |
| RF_PHY_TX_POWER_N6DBM     | 0x05   |      |
| RF_PHY_TX_POWER_N10DBM    | 0x03   |      |
| RF_PHY_TX_POWER_N15DBM    | 0x02   |      |
| RF_PHY_TX_POWER_N20DBM    | 0x01   |      |

#### 2.11.2. RF负载电容设置

函数名称：XTAL16M_CAP_SETTING(x)
函数功能：设置RF负载电容，校准频偏

参数说明:

| x :RF负载电容值 | 参数值        | 备注 |
| --------------- | ------------- | ---- |
|                 | 范围: 0 ~  31 |      |

#### 2.11.3. RF RX Burst

函数名称：void    rf_phy_dtm_ext_rx_demod_burst(uint8_t rfChnIdx,int8_t rfFoff,uint8_t xtal_cap,uint8_t pktLength,uint32 rxTimeOut,uint32 rxWindow, int16_t* rxEstFoff,uint8_t* rxEstRssi,uint8_t* rxEstCarrSens,uint16_t* rxPktNum)
函数功能：蓝牙PER收包测试接口

参数说明:

|                        | 参数值                              | 备注 |
| ---------------------- | ----------------------------------- | ---- |
| uint8_t rfChnIdx       | rf channel = 2402+(rfChnIdx<<1)     |      |
| int8_t rfFoff          | rf freq offset = rfFoff*4KHz        |      |
| uint8_t xtal_cap       | 范围: 0 ~  31                       |      |
| uint8_t pktLength      | pkt length(Byte)                    |      |
| uint32 rxTimeOut       | rx on time (ms)                     |      |
| uint32 rxWindow        | rx demod window length(us)          |      |
| int16_t* rxEstFoff     | rx demod estimated frequency offset |      |
| uint8_t* rxEstRssi     | rx demod estimated rssi             |      |
| uint8_t* rxEstCarrSens | rx demod estimated carrier sense    |      |
| uint16_t* rxPktNum     | rx demod received pkt number        |      |

####  2.11.4. RF TX Burst

函数名称：void    rf_phy_dtm_ext_tx_mod_burst(uint8_t txPower, uint8_t rfChnIdx,uint8_t xtal_cap,int8_t rfFoff ,  uint8_t pktType, uint8_t pktLength,uint32 txPktNum,uint32 txPktIntv)
函数功能：蓝牙发包测试接口

参数说明:

|                   | 参数值                                                       | 备注 |
| ----------------- | ------------------------------------------------------------ | ---- |
| uint8_t txPower   | rf tx power                                                  |      |
| uint8_t rfChnIdx  | rf channel = 2402+(rfChnIdx<<1)                              |      |
| uint8_t xtal_cap  | 范围: 0 ~  31                                                |      |
| int8_t rfFoff     | rf freq offset = rfFoff*4KHz                                 |      |
| uint8_t pktType   | modulaiton data type, 0: prbs9, 1: 1111000: 2 10101010       |      |
| uint8_t pktLength | pkt length(Byte)                                             |      |
| uint32 txPktNum   | burst pkt tx number                                          |      |
| uint32 txPktIntv  | txPkt intv,0 txPkt intv is pkt interval =  ceil((L+249)/625) * 625 |      |

####  2.11.4. RF TX SingleTone

函数名称：void void    rf_phy_dtm_ext_tx_singleTone(uint8_t txPower, uint8_t rfChnIdx,uint8_t xtal_cap,int8_t rfFoff ,uint32 testTimeUs)
函数功能：蓝牙发包测试接口

参数说明:

|                   | 参数值                          | 备注 |
| ----------------- | ------------------------------- | ---- |
| uint8_t txPower   | rf tx power                     |      |
| uint8_t rfChnIdx  | rf channel = 2402+(rfChnIdx<<1) |      |
| uint8_t xtal_cap  | 范围: 0 ~  31                   |      |
| int8_t rfFoff     | rf freq offset = rfFoff*4KHz    |      |
| uint32 testTimeUs | test loop active time(ms)       |      |

## 3. 配置

配置文件：package.yaml

| 宏定义名称              | 功能说明  | 功能说明                              | 备注 |
| --------------------- | -------- | ------------------------------------ | ---- |
| PHY_MCU_TYPE          | 4        | MCU型号，TG7120B搭载玄铁CK802 CPU |      |
| AOS_DEFAULT_APP_PRI   | 10       | Task默认优先级                        |      |
| AOS_MAX_APP_PRI       | 14       | 系统支持的APP Task最高优先级            |      |
| CONFIG_KERNEL_PRI_MAX | 14       | 系统支持的KERNEL Task最高优先级         |      |
| CONFIG_SYSTICK_HZ     | 1000     | 系统时钟定义，TG7120B 1个tick为1ms     |      |
| CONFIG_CSI_V1         | 1        | 指定使用的CSI接口，默认使用CSI1.0接口    |      |
| CONFIG_CSI            | "csi1"   | 指定使用的CSI接口，默认使用CSI1.0接口    |      |
    
## 4. 依赖资源

- csi
- hal_csi
- dut_service