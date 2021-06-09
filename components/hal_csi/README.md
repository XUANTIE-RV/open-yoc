## 概述
hal_csi用于对于不同底层驱动的操作实现，统一封装成hal接口，上层使用相关功能时，统一调用hal层接口，以保证app的通用性。例如GPIO、UART、SPI、I2C、I2S、FLASH、PWM、TIMER、 WDG、 RTC、 ADC、 DAC、 RNG、CAN、USBD、USBH、QSPI、SD、INTERPT、NAND、NOR。

## 组件安装
```bash
yoc init
yoc install hal_csi
```

## 配置
无。

## 接口列表

### ADC

| 函数 | 说明 |
| :--- | :--- |
| hal_adc_init | 初始化指定ADC |
| hal_adc_value_get | 获取ADC采样值 |
| hal_adc_finalize | 关闭指定ADC |

### CAN

| 函数 | 说明 |
| :--- | :--- |
| hal_can_init | 初始化指定CAN端口  |
| hal_can_filter_init | 初始化指定CAN端口滤波器 |
| hal_can_send | 从指定的CAN端口发送数据 |
| hal_can_recv | 从指定的CAN端口接收数据 |
| hal_can_finalize | 关闭指定CAN端口 |

### DAC

| 函数 | 说明 |
| :--- | :--- |
| hal_dac_init | 初始化指定DAC |
| hal_dac_start | 开始DAC输出 |
| hal_dac_stop | 停止DAC输出 |
| hal_dac_set_value | 设置DAC输出值 |
| hal_dac_get_value | 获取当前DAC输出值 |
| hal_dac_finalize | 关闭指定DAC |

### FLASH

| 函数 | 说明 |
| :--- | :--- |
| hal_flash_info_get | 获取指定区域的FLASH信息 |
| hal_flash_erase | 擦除FLASH的指定区域 |
| hal_flash_write | 写FLASH的指定区域 |
| hal_flash_erase_write   | 先擦除再写FLASH的指定区域 |
| hal_flash_read | 读FLASH的指定区域 |
| hal_flash_enable_secure | 使能加密FLASH的指定区域 |
| hal_flash_dis_secure | 关闭加密FLASH的指定区域 |
| hal_flash_addr2offset | 将物理地址转换为分区号和偏移 |

### GPIO

| 函数 | 说明 |
| :--- | :--- |
| hal_gpio_init | 初始化指定GPIO管脚 |
| hal_gpio_output_high   | 使指定GPIO输出高电平 |
| hal_gpio_output_low | 使指定GPIO输出低电平 |
| hal_gpio_output_toggle | 使指定GPIO输出翻转 |
| hal_gpio_input_get | 获取指定GPIO管脚的输入值 |
| hal_gpio_enable_irq | 使能指定GPIO的中断模式，挂载中断服务函数 |
| hal_gpio_disable_irq | 关闭指定GPIO的中断 |
| hal_gpio_clear_irq | 清除指定GPIO的中断状态 |
| hal_gpio_finalize | 关闭指定GPIO |

### I2C

| 函数 | 说明 |
| :--- | :--- |
| hal_i2c_init | 初始化指定I2C端口 |
| hal_i2c_master_send | master模式下从指定的I2C端口发送数据 |
| hal_i2c_master_recv | master模式下从指定的I2C端口接收数据 |
| hal_i2c_slave_send | slave模式下从指定的I2C端口发送数据 |
| hal_i2c_slave_recv | slave模式下从指定的I2C端口接收数据 |
| hal_i2c_mem_write | mem模式（读写I2C存储器）下从指定的I2C端口发送数据 |
| hal_i2c_mem_read | mem模式（读写I2C存储器）下从指定的I2C端口接收数据 |
| hal_i2c_finalize | 关闭指定I2C端口 |

### I2S

| 函数 | 说明 |
| :--- | :--- |
| hal_i2s_init | 初始化指定I2S端口 |
| hal_i2s_send | 从指定的I2S端口发送数据 |
| hal_i2s_recv | 从指定的I2S端口接收数据 |
| hal_i2s_pause | 暂停指定的I2S端口 |
| hal_i2s_resume | 恢复指定的I2S端口 |
| hal_i2s_stop | 停止指定的I2S端口 |
| hal_i2s_finalize | 关闭指定I2S端口 |

### INTERRUPT

| 函数 | 说明 |
| :--- | :--- |
| hal_interpt_init | 中断初始化 |
| hal_interpt_mask | 中断屏蔽 |
| hal_interpt_umask | 放开中断屏蔽 |
| hal_interpt_install | 中断处理程序安装 |

### NAND FLASH

| 函数 | 说明 |
| :--- | :--- |
| hal_nand_init |  NAND FLASH初始化 |
| hal_nand_finalize |  关闭NAND FLASH |
| hal_nand_read_page |  读NAND FLASH页数据 |
| hal_nand_write_page |  写NAND FLASH页数据 |
| hal_nand_read_spare |  读NAND FLASH spare区数据 |
| hal_nand_write_spare |  写NAND FLASH spare区数据 |
| hal_nand_erase_block |  擦除NAND FLASH块数据 |

### NOR FLASH

| 函数 | 说明 |
| :--- | :--- |
| hal_nor_init | NOR FLASH初始化 |
| hal_nor_finalize | 关闭NOR FLASH |
| hal_nor_read | 读NOR FLASH数据 |
| hal_nor_write | 写NOR FLASH数据 |
| hal_nor_erase_block | 擦除NOR FLASH块数据 |
| hal_nor_erase_chip | 擦除NOR FLASH数据 |

### PWM

| 函数 | 说明 |
| :--- | :--- |
| hal_pwm_init | 初始化指定PWM   |
| hal_pwm_start | 开始输出指定PWM |
| hal_pwm_stop | 停止输出指定PWM |
| hal_pwm_para_chg | 修改指定PWM参数 |
| hal_pwm_finalize | 关闭指定PWM |

### QSPI

| 函数 | 说明 |
| :--- | :--- |
| hal_qspi_init | 初始化指定QSPI端口 |
| hal_qspi_send | 从指定的QSPI端口发送数据 |
| hal_qspi_recv | 从指定的QSPI端口接收数据 |
| hal_qspi_command | 设置QSPI命令 |
| hal_qspi_auto_polling | 设置QSPI查询模式 |
| hal_qspi_finalize | 关闭指定QSPI端口 |

### RNG

| 函数 | 说明 |
| :---| :--- |
| hal_random_num_read | 获取随机数 |

### RTC

| 函数 | 说明 |
| :--- | :--- |
| hal_rtc_init | 初始化指定RTC |
| hal_rtc_get_time | 获取指定RTC时间 |
| hal_rtc_set_time | 设置指定RTC时间 |
| hal_rtc_finalize | 关闭指定RTC |

### SD

| 函数 | 说明 |
| :--- | :--- |
| hal_sd_init | SD初始化 |
| hal_sd_blks_read | SD读取 |
| hal_sd_blks_write | SD写入 |  
| hal_sd_erase | SD擦除 |
| hal_sd_stat_get | 获取SD状态 |
| hal_sd_info_get | 获取SD信息 |
| hal_sd_finalize | 关闭SD |

### SPI

| 函数 | 说明 |
| :--- | :--- |
| hal_spi_init | 初始化指定SPI端口 |
| hal_spi_send | 从指定的SPI端口发送数据 |
| hal_spi_recv | 从指定的SPI端口接收数据 |
| hal_spi_send_recv | 从指定的SPI端口发送并接收数据 |
| hal_spi_finalize | 关闭指定SPI端口 |

### TIMER

| 函数 | 说明 |
| :--- | :--- |
| hal_timer_init | 初始化指定TIMER |
| hal_timer_start | 启动指定的TIMER |
| hal_timer_stop | 停止指定的TIMER |
| hal_timer_para_chg | 改变指定TIMER的参数 |
| hal_timer_finalize | 关闭指定TIMER |

### UART

| 函数 | 说明 |
| :--- | :--- |
| hal_uart_init | 初始化指定UART |
| hal_uart_send | 从指定的UART发送数据 |
| hal_uart_recv | 从指定的UART接收数据 |
| hal_uart_recv_II  | 从指定的UART中断方式接收数据 |
| hal_uart_finalize | 关闭指定UART |

### USBD

| 函数 | 说明 |
| :--- | :--- |
| usbd_hal_init | 初始化usb设备驱动 |
| usbd_hal_deinit | 去初始化usb设备驱动 |
| usbd_hal_start | 启动usb设备驱动 |
| usbd_hal_stop | 停止usb设备驱动 |
| usbd_hal_connect | 启用usb设备中断 |
| usbd_hal_disconnect | 禁用usb设备中断 |
| usbd_hal_configure_device | 配置usb设备信息 |
| usbd_hal_unconfigure_device | 取消配置usb设备信息 |
| usbd_hal_set_address | 设置usb设备地址 |
| usbd_hal_ep0_setup | Endpoint0设置（读取设置包） |
| usbd_hal_ep0_read | Endpoint0读取数据包 |
| usbd_hal_ep0_read_stage | Endpoint0读取阶段 |
| usbd_hal_get_ep0_read_result | Endpoint0获取读取结果 |
| usbd_hal_ep0_write | Endpoint0写入数据包 |
| usbd_hal_get_ep0_write_result | 获取endpoint0写入结果 |
| usbd_hal_ep0_stall | 暂停endpoint0 |
| usbd_hal_realise_endpoint | 打开endpoint |
| usbd_hal_endpoint_read | 开始读取endpoint数据 |
| usbd_hal_endpoint_read_result | 读取endpoint数据 |
| usbd_hal_endpoint_write | 开始写入endpoint数据 |
| usbd_hal_endpoint_write_result | 获取写入endpoint的数据状态 |
| usbd_hal_stall_endpoint | 暂停endpoint |
| usbd_hal_unstall_endpoint | 取消暂停endpoint |
| usbd_hal_get_endpoint_stall_state | 获取暂停endpoint的状态 |

### USBH

| 函数 | 说明 |
| :--- | :--- |
| hal_usbh_init | 初始化usb主机控制器 |
| hal_usbh_finalize | 完成usb主机控制器 |
| hal_usbh_port_reset | 重置usb主机控制器端口 |
| hal_usbh_get_speed | 获取设备速度 |
| hal_usbh_pipe_free | 释放主机控件的管道 |
| hal_usbh_pipe_configure | 配置主机控制器的管道 |
| hal_usbh_submit_urb | 提交Urb，开始发送或接收数据 |
| hal_usbh_get_urb_state | 获取Urb传输状态 |

### WDG

| 函数 | 说明 |
| :--- | :--- |
| hal_wdg_init | 初始化指定看门狗 |
| hal_wdg_reload | 重载指定看门狗   |
| hal_wdg_finalize | 关闭指定看门狗   |

## 接口详细说明

### ADC

#### hal_adc_init
`int hal_adc_init(adc_dev_t *adc);`

- 功能描述:
  - 初始化指定ADC。
  
- 参数:
  - `adc`: ADC设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### adc_dev_t

| 成员   | 类型          | 说明     |
| :----- | :------------ | :------- |
| port   | uint8_t       | adc 端口 |
| config | adc_config_t  | adc配置  |
| priv   | void        * | 私有数据 |

#### hal_adc_value_get
`int  hal_adc_value_get(adc_dev_t *adc, void *output, unsigned int timeout);`

- 功能描述:
  - 获取ADC采样值。
  
- 参数:
  - `adc`: ADC设备描述。
  - `output`: 数据缓冲区。
  - `timeout`: 超时时间，单位tick。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_adc_finalize
`int32_t hal_adc_finalize(adc_dev_t *adc);`

- 功能描述:
  - 关闭指定ADC。

- 参数:
  - `adc`: ADC设备描述。

- 返回值:
  - 0: 成功
  - < 0: 失败

### CAN

#### hal_can_init
`int32_t hal_can_init(can_dev_t *can);`
- 功能描述:
  - 初始化指定ADC。
  
- 参数:
  - `can`: CAN设备描述，定义需要初始化的CAN参数。
  
- 返回值:
  - 0: 成功
  - < 0: 失败

##### can_dev_t

| 成员   | 类型          | 说明     |
| :----- | :------------ | :------- |
| port   | uint8_t       | can端口  |
| config | can_config_t  | can配置  |
| priv   | void        * | 私有数据 |

#### hal_can_filter_init
`int32_t hal_can_filter_init(can_dev_t *can, const uint8_t filter_grp_cnt, can_filter_item_t *filter_config);`

- 功能描述:
  - 初始化指定CAN端口。
  
- 参数:
  - `can`: CAN设备描述。
  - `filter_grp_cnt`: 滤波器计数值。
  - `filter_config`: 滤波器配置。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

##### can_filter_item_t

| 成员        | 类型     | 说明                   |
| :---------- | :------- | :--------------------- |
| rtr         | uint8_t  | 0:数据帧，1:远程帧     |
| check_id    | uint32_t | 过滤器识别号           |
| filter_mask | uint32_t | 过滤器掩码编号或识别号 |

#### hal_can_send
`int32_t hal_can_send(can_dev_t *can, can_frameheader_t *tx_header, const void *data, const uint32_t timeout);`

- 功能描述:
  - 从指定的CAN端口发送数据。
  
- 参数:
  - `can`: CAN设备描述。
  - `tx_header`: 发送数据头。
  - `data`: 指向发送缓冲区的数据指针。
  - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。
  
- 返回值:
  - 0: 成功
  - < 0: 失败

##### can_frameheader_t

| 成员 | 类型     | 说明               |
| :--- | :------- | :----------------- |
| id   | uint32_t | can id             |
| rtr  | uint8_t  | 0:数据帧，1:远程帧 |
| dlc  | uint8_t  | <=8                |

#### hal_can_recv
`int32_t hal_can_recv(can_dev_t *can, can_frameheader_t *rx_header, void *data, const uint32_t timeout);`
- 功能描述:
  - 从指定的CAN端口接收数据。
  
- 参数:
  - `can`: CAN设备描述。
  - `rx_header`: 接收数据头。
  - `data`: 指向接收缓冲区的数据指针。
  - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。
  
- 返回值
  - 0: 成功。
  - < 0: 失败。

#### hal_can_finalize
`int32_t hal_can_finalize(can_dev_t *can);`
- 功能描述:
  - 关闭指定CAN端口。
  
- 参数:
  - `can`: CAN设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。
  
### DAC

#### hal_dac_init
`int32_t hal_dac_init(dac_dev_t *dac);`

- 功能描述:
  - 初始化指定DAC。
  
- 参数:
  - `dac`: DAC设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

##### dac_dev_t

| 成员 | 类型          | 说明     |
| :--- | :------------ | :------- |
| port | uint8_t       | dac端口  |
| priv | void        * | 私有数据 |

#### hal_dac_start
`int32_t hal_dac_start(dac_dev_t *dac, uint32_t channel);`

- 功能描述:
  - 开始DAC输出。
  
- 参数:
  - `dac`: DAC设备描述。
  - `channel`:  输出通道号。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_dac_stop
`int32_t hal_dac_stop(dac_dev_t *dac, uint32_t channel);`

- 功能描述:
  - 停止DAC输出。
  
- 参数:
  - `dac`: DAC设备描述。
  - `channel`:  输出通道号。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_dac_set_value
`int32_t hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data);`

- 功能描述:
  - 设置DAC输出值。
  
- 参数:
  - `dac`: DAC设备描述。
  - `channel`:  输出通道号。
  - `data`: 输出值。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_dac_get_value
`int32_t hal_dac_get_value(dac_dev_t *dac, uint32_t channel);`

- 功能描述:
  - 设置DAC输出值。
  
- 参数:
  - `dac`: DAC设备描述。
  - `channel`:  输出通道号。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_dac_finalize
`int32_t hal_dac_finalize(dac_dev_t *dac);`

- 功能描述:
  - 关闭指定DAC。
  
- 参数:
  - `dac`: DAC设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

### FLASH

#### hal_flash_info_get
`int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition);`

- 功能描述:
  - 获取指定区域的FLASH信息。
  
- 参数:
  - `in_partition`: FLASH分区号。
  - `partition`:  分区信息。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

##### hal_logic_partition_t

| 成员                  | 类型         | 说明     |
| :-------------------- | :----------- | :------- |
| partition_owner       | hal_flash_t  | 分区类型 |
| partition_description | const char * | 分区描述 |
| partition_start_addr  | uint32_t     | 起始地址 |
| partition_length      | uint32_t     | 长度     |
| partition_options     | uint32_t     | 读写使能 |

#### hal_flash_info_get
`int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size);`

- 功能描述:
  - 擦除FLASH的指定区域。
  
- 参数:
  - `pno`: FLASH分区号。
  - `off_set`:  偏移量。
  - `size`:  要擦除的字节数。
  
- 返回值:
  - 0: 成功
  - < 0: 失败

#### hal_flash_write
`int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf , uint32_t buf_size);`

- 功能描述:
  - 写FLASH的指定区域。
  
- 参数:
  - `pno`: FLASH分区号。
  - `poff`:  偏移量，写入后其值会刷新为写尾部。
  - `buf`:  指向要写入数据的指针。
  - `buf_size`: 要写入的字节数。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_flash_erase_write
`int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len);`

- 功能描述:
  - 先擦除再写FLASH的指定区域。
  
- 参数:
  - `in_partition`: FLASH分区号。
  - `off_set`:  偏移量，写入后其值会更新为写尾部。
  - `in_buf`:  指向要写入数据的指针。
  - `in_buf_len`: 要擦除和写入的字节数。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_flash_erase_write
`int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t in_buf_len);`
- 功能描述:
  - 读取FLASH的指定区域。
  
- 参数:
  - `in_partition`: FLASH分区号。
  - `off_set`:  偏移量，写入后其值会更新为写尾部。
  - `out_buf`:  数据缓冲区地址。
  - `in_buf_len`: 要读取的字节数。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_flash_enable_secure
`int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size);`

- 功能描述:
  - 使能加密FLASH的指定区域。
  
- 参数:
  - `in_partition`: FLASH分区号。
  - `off_set`:  偏移量，写入后其值会更新为写尾部。
  - `size`:  使能区域字节数。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_flash_dis_secure
`int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size);`

- 功能描述:
  - 关闭加密FLASH的指定区域。
  
- 参数:
  - `partition`: FLASH分区号。
  - `off_set`:  偏移量，写入后其值会更新为写尾部。
  - `size`:  去使能加密区域字节数。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_flash_addr2offset
`int32_t hal_flash_addr2offset(hal_partition_t *in_partition, uint32_t *off_set, uint32_t addr);`

- 功能描述:
  - 将物理地址转换为分区号和偏移。该偏移与hal_partitions分区表配置相关。
  
- 参数:
  - `in_partition`: FLASH分区号。
  - `off_set`:  获取到的该地址在分区内偏移。
  - `addr`:  输入需要查询地址。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

### GPIO

#### hal_gpio_init
`int32_t hal_gpio_init(gpio_dev_t *gpio); `

- 功能描述:
  - GPIO初始化。
  
- 参数:
  - `gpio`: GPIO设备描述，定义需要初始化的GPIO管脚的相关特性。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

##### gpio_dev_t

| 成员   | 类型           | 说明           |
| :----- | :------------- | :------------- |
| port   | uint8_t        | gpio逻辑端口号 |
| config | gpio_config_t  | gpio配置信息   |
| priv   | void         * | 私有数据       |

#### hal_gpio_output_high
`int32_t hal_gpio_output_high(gpio_dev_t *gpio); `

- 功能描述:
  - 某gpio输出高电平。
  
- 参数:
  - `gpio`: GPIO设备描述。
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_gpio_output_low
`int32_t hal_gpio_output_low(gpio_dev_t *gpio); `

- 功能描述:
  - 某gpio输出低电平。
  
- 参数:
  - `gpio`: GPIO设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_gpio_output_toggle
`int32_t hal_gpio_output_toggle(gpio_dev_t* gpio); `

- 功能描述:
  - 某gpio输出翻转。
  
- 参数:
  - `gpio`: GPIO设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_gpio_input_get
`int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value); `

- 功能描述:
  - 获取某gpio管脚输入值。
  
- 参数:
  - `gpio`: GPIO设备描述。
  - `value`: 需要获取的管脚值存放地址。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_gpio_enable_irq
`int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg); `

- 功能描述:
  - 使能指定GPIO的中断模式，挂载中断服务函数，需要预先调用hal_gpio_init，设置IRQ_MODE。
  
- 参数:
  - `gpio`: GPIO设备描述。
  - `trigger`: 中断的触发模式，上升沿、下降沿还是都触发。
  - `handler`: 中断服务函数指针，中断触发后将执行指向的函数v
  - `arg`: 中断服务函数的入参。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_gpio_disable_irq
`int32_t hal_gpio_disable_irq(gpio_dev_t *gpio); `

- 功能描述:
  - 关闭指定GPIO的中断。
  
- 参数:
  - `gpio`: GPIO设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_gpio_clear_irq
`int32_t hal_gpio_clear_irq(gpio_dev_t *gpio); `

- 功能描述:
  - 清除指定GPIO的中断。
  
- 参数:
  - `gpio`: GPIO设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

#### hal_gpio_finalize
`int32_t hal_gpio_finalize(gpio_dev_t *gpio); `

- 功能描述:
  - 关闭指定GPIO，及其中断。
  
- 参数:
  - `gpio`: GPIO设备描述。
  
- 返回值:
  - 0: 成功。
  - < 0: 失败。

### I2C
#### hal_i2c_init
`int32_t hal_i2c_init(i2c_dev_t *i2c);`

- 功能描述:
   - 初始化指定I2C端口。   
##### i2c_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | i2c_config_t | I2C配置结构 |
| priv | void * | 私有数据 |

##### i2c_config_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| address_width | uint32_t | 地址带宽 |
| freq | uint32_t | 时钟频率 |
| mode | uint8_t | 模式 |
| dev_addr | uint16_t | 设备地址 |

- 参数:
   - `i2c`: I2C设备描述，定义需要初始化的I2C参数。  

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2c_master_send
`int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - master模式下从指定的I2C端口发送数据。

- 参数:
   - `i2c`: I2C设备描述。  
   - `dev_addr`: 目标设备地址。 
   - `data`: 指向发送缓冲区的数据指针。  
   - `size`: 要发送的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
#### hal_i2c_master_recv
`int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,uint16_t size, uint32_t timeout);`

- 功能描述:
   - master模式下从指定的I2C端口接收数据。

- 参数:
   - `i2c`: I2C设备描述。  
   - `dev_addr`: 目标设备地址。 
   - `data`: 指向接收缓冲区的数据指针。  
   - `size`: 期望接收的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2c_slave_send
`int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - slave模式下从指定的I2C端口发送数据。

- 参数:
   - `i2c`: I2C设备描述。  
   - `data`: 指向发送缓冲区的数据指针。  
   - `size`: 要发送的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2c_slave_recv
`int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - slave模式下从指定的I2C端口接收数据。

- 参数:
   - `i2c`: I2C设备描述。  
   - `data`: 指向要接收数据的数据指针。  
   - `size`: 要接收的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
#### hal_i2c_mem_write
`int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, const uint8_t *data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - 向指定的设备内存写数据。

- 参数:
   - `i2c`: I2C设备描述。
   - `dev_addr`: 目标设备地址。  
   - `mem_addr`: 内部内存地址。   
   - `mem_addr_size`: 内部内存地址大小。  
   - `data`: 指向要发送数据的数据指针。    
   - `size`: 要发送的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2c_mem_read
`int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, uint8_t  *data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - 从指定的设备内存读数据。

- 参数:
   - `i2c`: I2C设备描述。
   - `dev_addr`: 目标设备地址。  
   - `mem_addr`: 内部内存地址。   
   - `mem_addr_size`: 内部内存地址大小。  
   - `data`: 指向接收缓冲区的数据指针。    
   - `size`: 要接收的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
#### hal_i2c_finalize
`int32_t hal_i2c_finalize(i2c_dev_t *i2c);`

- 功能描述:
   - 关闭指定I2C端口。

- 参数:
   - `i2c`: I2C设备描述。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

### I2S 

#### hal_i2s_init
`int32_t hal_i2s_init(i2s_dev_t *i2s);`
- 功能描述:
   - 初始化指定I2S端口。
##### i2s_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | i2s_config_t | I2S配置结构 |
| priv | void * | 私有数据 |
##### i2s_config_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| freq | uint32_t | 通信频率 |
| mode | hal_i2s_mode_t | 操作模式 |
| standard | hal_i2s_std_t | 通信标准 |
| data_format | hal_i2s_data_format_t | 通信数据格式 |

- 参数:
   - `i2s`: I2S设备描述，定义需要初始化的I2S参数。

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
#### hal_i2s_send
`int32_t hal_i2s_send(i2s_dev_t *i2s, const void *data, uint32_t size, uint32_t timeout);`

- 功能描述:
   - 从指定的I2S端口发送数据。

- 参数:
   - `i2s`: I2S设备描述。
   - `data`: 指向发送缓冲区的数据指针。
   - `size`: 要发送的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。      

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2s_recv
`int32_t hal_i2s_recv(i2s_dev_t *i2s, void *data, uint32_t size, uint32_t timeout);`

- 功能描述:
   - 从指定的I2S端口接收数据。

- 参数:
   - `i2s`: I2S设备描述。
   - `data`: 指向接收缓冲区的数据指针。
   - `size`: 要接收的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。      

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2s_pause
`int32_t hal_i2s_pause(i2s_dev_t *i2s);`

- 功能描述:
   - 暂停指定I2S端口。

- 参数:
   - `i2s`: I2S设备描述。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2s_resume
`int32_t hal_i2s_resume(i2s_dev_t *i2s);`

- 功能描述:
   - 恢复指定I2S端口。

- 参数:
   - `i2s`: I2S设备描述。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2s_stop
`int32_t hal_i2s_stop(i2s_dev_t *i2s);`

- 功能描述:
   - 停止指定I2S端口。

- 参数:
   - `i2s`: I2S设备描述。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_i2s_finalize
`int32_t hal_i2s_finalize(i2s_dev_t *i2s);`

- 功能描述:  
  - 关闭指定I2S端口。
  
- 参数:  
  - `i2s`: I2S设备描述。   
  
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### INTERRUPT

#### hal_interpt_init
`int32_t hal_interpt_init(void);`

- 功能描述:
   - 中断初始化。

- 参数:
   - 无。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
####  hal_interpt_mask
`int32_t hal_interpt_mask(int32_t vec);`

- 功能描述:
   - 屏蔽指定的中断。

- 参数:
   - `vec`: 中断向量。  

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_interpt_umask
`int32_t hal_interpt_umask(int32_t vec);`

- 功能描述:
   - 放开屏蔽指定的中断。

- 参数:
   - `vec`: 中断向量。  

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_interpt_install
`int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, void *para,  char *name);`

- 功能描述:
   - 安装指定的中断处理程序。
   hal_interpt_t定义: 
   `typedef void (*hal_interpt_t)(int32_t vec, void *para);`

- 参数:
   - `vec`: 中断向量。  
   - `handler`: 中断处理程序。 
   - `para`: 中断处理程序参数。 
   - `name`: 中断名称。       

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
### NAND

####  hal_nand_init
`int32_t hal_nand_init(nand_dev_t *nand);`

- 功能描述:
   - NAND FLASH初始化。
##### nand_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| base_addr | uint32_t | 基地址 |
| config | nand_config_t | NAND FLASH配置结构 |
| priv | void * | 私有数据 |

##### nand_config_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| page_size | uint32_t | 页大小 |
| spare_area_size | uint32_t | spare区大小 |
| block_size | uint32_t | 块大小 |
| zone_size | uint32_t | 区大小 |
| zone_number | uint32_t | 区数量 |

- 参数:
   - `nand`: NAND FLASH设备描述。        

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_nand_finalize
`int32_t hal_nand_finalize(nand_dev_t *nand);`

- 功能描述:
   - 关闭指定NAND FLASH。
   
- 参数:
   - `nand`: NAND FLASH设备描述。        

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
####  hal_nand_read_page
`int32_t hal_nand_read_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count);`

- 功能描述:
   - 从NAND FLASH指定页读取数据。   
##### nand_addr_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| page | uint16_t | 页地址 |
| block | uint16_t | 块地址 |
| zone | uint16_t | 区地址 |

- 参数:
   - `nand`: NAND FLASH设备描述。     
   - `addr`: NAND FLASH地址。
   - `data`: NAND 数据缓冲区指针。
   - `page_count`: 要读的页数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nand_write_page
`int32_t hal_nand_write_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count);`

- 功能描述:
   - 往NAND FLASH指定页写入数据。
   
- 参数:
   - `nand`: NAND FLASH设备描述。     
   - `addr`: NAND FLASH地址。
   - `data`: NAND 数据缓冲区指针。
   - `page_count`: 要写的页数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nand_read_spare
`int32_t hal_nand_read_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len);`

- 功能描述:
   - 从NAND FLASH的spare区读取数据。
   
- 参数:
   - `nand`: NAND FLASH设备描述。     
   - `addr`: NAND FLASH地址。
   - `data`: NAND 数据缓冲区指针。
   - `data_len`: 数据字节数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nand_write_spare
`int32_t hal_nand_write_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len);`

- 功能描述:
   - 往NAND FLASH的spare区写入数据。
   
- 参数:
   - `nand`: NAND FLASH设备描述。     
   - `addr`: NAND FLASH地址。
   - `data`: NAND 数据缓冲区指针。
   - `data_len`: 数据字节数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nand_erase_block
`int32_t hal_nand_erase_block(nand_dev_t *nand, nand_addr_t *addr);`

- 功能描述:
   - 擦除NAND FLASH的指定块。
   
- 参数:
   - `nand`: NAND FLASH设备描述。     
   - `addr`: NAND FLASH地址。 

- 返回值:
   - 0: 成功。
   - 非0: 失败。

### NOR FLASH

####  hal_nor_init
`int32_t hal_nor_init(nor_dev_t *nor);`
- 功能描述:
   - NOR FLASH初始化。
##### nor_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| base_addr | uint32_t | 基地址 |
| config | nor_config_t | NOR FLASH配置结构 |
| priv | void * | 私有数据 |
##### nor_config_t 
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| block_size | uint32_t | 块大小 |
| chip_size | uint32_t | 块数 |

- 参数:
   - `nor`: NOR FLASH设备描述。        

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_nor_finalize
`int32_t hal_nor_finalize(nor_dev_t *nor);`

- 功能描述:
   - 关闭NOR FLASH。

- 参数:
   - `nor`: NOR FLASH设备描述。        

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nor_read
`int32_t hal_nor_read(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len);`

- 功能描述:
   - 读NOR FLASH数据。

- 参数:
   - `nor`: NOR FLASH设备描述。 
   - `addr`: 地址。
   - `data`: 数据缓冲区。
   - `len`: 要读取的字节数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nor_write
`int32_t hal_nor_write(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len);`

- 功能描述:
   - 往NOR FLASH写入数据。

- 参数:
   - `nor`: NOR FLASH设备描述。 
   - `addr`: 地址。
   - `data`: 数据缓冲区。
   - `len`: 要写入的字节数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nor_erase_block
`int32_t hal_nor_erase_block(nor_dev_t *nor, uint32_t *addr, uint32_t block_count);`

- 功能描述:
   - 擦除NOR FLASH指定块数据。

- 参数:
   - `nor`: NOR FLASH设备描述。 
   - `addr`: 地址。
   - `block_count`: 要擦除的块数。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

####  hal_nor_erase_chip
`int32_t hal_nor_erase_chip(nor_dev_t *nor, uint32_t *addr);`

- 功能描述:   
  - 擦除NOR FLASH所有数据。
   
- 参数:
   - `nor`: NOR FLASH设备描述。 
   - `addr`: 地址。

- 返回值:
   - 0: 成功。
   - 非0: 失败。
      
### PWM

#### hal_pwm_init
`int32_t hal_pwm_init(pwm_dev_t *pwm);`

- 功能描述:
   - 初始化指定PWM。
     
##### pwm_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | pwm_config_t | PWM配置结构 |
| priv | void * | 私有数据 |
##### pwm_config_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| duty_cycle | uint32_t | DUTY周期 |
| freq | hal_i2s_mode_t | PWM频率 |

- 参数:
   - `pwm`: PWM设备描述，定义需要初始化的PWM参数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_pwm_start
`int32_t hal_pwm_start(pwm_dev_t *pwm);`

- 功能描述:
   - 开始输出指定PWM。

- 参数:
   - `pwm`: PWM设备描述。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_pwm_stop
`int32_t hal_pwm_stop(pwm_dev_t *pwm);`

- 功能描述:
   - 停止输出指定PWM。

- 参数:
   - `pwm`: PWM设备描述。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_pwm_para_chg
`int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para);`

- 功能描述:
   - 修改指定PWM参数。

- 参数:
   - `pwm`: PWM设备描述。  
   - `para`: 新配置参数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_pwm_finalize
`int32_t hal_pwm_finalize(pwm_dev_t *pwm);`

- 功能描述:
   - 关闭指定PWM。

- 参数:
   - `pwm`: PWM设备描述。  

- 返回值:
   - 0: 成功。
   - 非0: 失败。

### QSPI

#### hal_qspi_init
`int32_t hal_qspi_init(qspi_dev_t *qspi);`

- 功能描述:
   - 初始化指定QSPI端口。
     
##### qspi_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | qspi_config_t | PWM配置结构 |
| priv | void * | 私有数据 |
##### qspi_config_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| freq | uint32_t | 通信频率 |

- 参数:
   - `qspi`: QSPI设备描述，定义需要初始化的QSPI参数。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_qspi_send
`int32_t hal_qspi_send(qspi_dev_t *qspi, const uint8_t *data, uint32_t timeout);`

- 功能描述:
   - 从指定的QSPI端口发送数据。

- 参数:
   - `qspi`: QSPI设备描述。  
   - `data`: 指向发送缓冲区的数据指针。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。      

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_qspi_recv
`int32_t hal_qspi_recv(qspi_dev_t *qspi, uint8_t *data, uint32_t timeout);`

- 功能描述:
   - 从指定的QSPI端口接收数据。

- 参数:
   - `qspi`: QSPI设备描述。  
   - `data`: 指向接收缓冲区的数据指针。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。      

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_qspi_command
`int32_t hal_qspi_command(qspi_dev_t *qspi, qspi_cmd_t *cmd, uint32_t timeout);`

- 功能描述:
   - 设置QSPI命令。
##### qspi_cmd_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| instruction | uint32_t | 指令 |
| address | uint32_t | 命令地址 |
| size | uint32_t | 命令大小 |

- 参数:
   - `qspi`: QSPI设备描述。  
   - `cmd`: 命令字。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。      

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_qspi_auto_polling
`int32_t hal_qspi_auto_polling(qspi_dev_t *qspi, uint32_t cmd, uint32_t timeout);`

- 功能描述:
   - 设置QSPI查询模式。

- 参数:
   - `qspi`: QSPI设备描述。  
   - `cmd`: 命令字。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。      

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_qspi_finalize
`int32_t hal_qspi_finalize(qspi_dev_t *qspi);`

- 功能描述:
   - 关闭指定QSPI端口。

- 参数:
   - `qspi`: QSPI设备描述。  

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
### RNG
#### hal_random_num_read
`int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes);`

- 功能描述:
   - 获取随机数。
##### random_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| priv | void * | 私有数据 |

- 参数:
   - `random`: RNG设备描述。  
   - `*buf`: 返回的数据指针。 
   - `bytes`: 数据字节数。 

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
### RTC

#### hal_rtc_init
`int hal_rtc_init(rtc_dev_t *rtc);`

- 功能描述:
   - 初始化指定RTC。
##### rtc_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | rtc_config_t | RTC配置结构 |
| priv | void * | 私有数据 |
##### rtc_config_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| format | uint8_t | 时间格式（DEC 或 BCD） |

- 参数:
   - `rtc`: RTC设备描述。  

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_rtc_get_time
`int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time);`

- 功能描述:
   - 获取指定RTC时间。
##### rtc_time_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| sec | uint8_t | 秒 DEC 格式:值域[0,59], BCD 格式:值域[0x0,0x59]） |
| min | uint8_t | 分（DEC 格式:值域[0,59], BCD 格式:值域[0x0,0x59]） |
| hr | uint8_t | 小时（DEC 格式:值域[0,23], BCD 格式:值域[0x0,0x23]）  |
| weekday | uint8_t | 星期（DEC 格式:值域[1,7], BCD 格式:值域[0x1,0x7]） |
| date | uint8_t | 日（DEC 格式:值域[1,31], BCD 格式:值域[0x1,0x31]） |
| month | uint8_t | 月（DEC 格式:值域[1,12], BCD 格式:值域[0x1,0x12]） |
| year | uint16_t | 年 （DEC 格式:值域[0,9999], BCD 格式:值域[0x0,0x9999]） |

- 参数:
   - `rtc`: RTC设备描述。  
   - `time`: 要获取的时间。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_rtc_set_time
`int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time);`

- 功能描述:
   - 设置指定RTC时间。

- 参数:
   - `rtc`: RTC设备描述。  
   - `time`: 要设置的时间。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_rtc_finalize
`int32_t hal_rtc_finalize(rtc_dev_t *rtc);`

- 功能描述:
   - 关闭指定RTC。

- 参数:
   - `rtc`: RTC设备描述。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

### SD

#### hal_sd_init
`int32_t hal_sd_init(sd_dev_t *sd);`

- 功能描述:
   - SD初始化。

- 参数:
   - `sd`: SD设备描述。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
##### sd_dev_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | sd_config_t | SD配置数据 |
| priv | void * | 私有数据 |

##### sd_config_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| bus_wide | uint32_t | 总线宽度 |
| freq | uint32_t | 频率 |
   
#### hal_sd_blks_read
`int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout);`
			 
- 功能描述:
   - 读取SD数据。

- 参数:
   - `sd`: SD设备描述。 
   - `data`: 数据缓冲区指针。 
   - `blk_addr`: 块地址。 
   - `blks`: 要读取的块数。    
   - `timeout`: 超时时间，单位：ms。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_sd_blks_write
`int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout);`
			  
- 功能描述:
   - 写入SD数据。

- 参数:
   - `sd`: SD设备描述。 
   - `data`: 数据缓冲区指针。 
   - `blk_addr`: 块地址。 
   - `blks`: 要写入的块数。   
   - `timeout`: 超时时间，单位：ms。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。			  

#### hal_sd_erase
`int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr);`

- 功能描述:
   - SD数据擦除。

- 参数:
   - `sd`: SD设备描述。 
   - `blk_start_addr`: 块起始地址。 
   - `blk_end_addr`: 块结束地址。   

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_sd_stat_get
`int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat);`

- 功能描述:
   - 获取SD状态。

- 参数:
   - `sd`: SD设备描述。 
   - `stat`: 状态指针。 

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
#### hal_sd_info_get
`int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info);`

- 功能描述:
   - 获取SD信息。

- 参数:
   - `sd`: SD设备描述。 
   - `stat`: 信息指针。 

- 返回值:
   - 0: 成功。
   - 非0: 失败。

##### hal_sd_info_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| blk_nums | uint32_t | 块数 |
| blk_size | uint32_t | 块大小 |

#### hal_sd_finalize
`int32_t hal_sd_finalize(sd_dev_t *sd);`
- 功能描述:
   - 关闭SD。

- 参数:
   - `sd`: SD设备描述。 

- 返回值:
   - 0: 成功。
   - 非0: 失败。

### SPI
   
#### hal_spi_init
`int32_t hal_spi_init(spi_dev_t *spi);`

- 功能描述:
   - 初始化指定SPI接口。

- 参数:
   - `spi`: SPI设备描述，定义需要初始化的SPI参数。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

##### spi_dev_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | spi_config_t | 配置信息 |
| priv | void* | 用户自定义数据 |

##### spi_config_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| mode | uint32_t | 通信模式 |
| freq | uint32_t | 通信频率 |

#### hal_spi_send
`int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - 从指定的SPI接口发送数据。

- 参数:
   - `spi`: SPI设备描述。
   - `data`: 指向要发送数据的数据指针。
   - `size`: 要发送的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_spi_recv
`int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - 从指定的SPI接口接收数据。

- 参数:
   - `spi`: SPI设备描述。
   - `data`: 指向接收缓冲区的数据指针。
   - `size`: 期望接收的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_spi_send_recv
`int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout);`

- 功能描述:
   - 从指定的SPI接口发送并接收数据。

- 参数:
   - `spi`: SPI设备描述。
   - `tx_data`: 指向发送缓冲区的数据指针。
   - `rx_data`: 指向接收缓冲区的数据指针。
   - `size`: 期望接收的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_spi_finalize
`int32_t hal_spi_finalize(spi_dev_t *spi);`

- 功能描述:
   - 关闭指定SPI接口。

- 参数:
   - `spi`: SPI设备描述。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_timer_init
`int32_t hal_timer_init(timer_dev_t *tim);`

- 功能描述:
   - 初始化指定定时器。

- 参数:
   - `tim`: 定时器设备描述，定义需要初始化的定时器参数。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

##### timer_dev_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | timer_config_t | 配置信息 |
| priv | void * | 用户自定义数据 |

#### hal_timer_start
`int32_t hal_timer_start(timer_dev_t *tim);`

- 功能描述:
   - 启动指定的定时器。

- 参数:
   - `tim`: 定时器设备描述。

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
#### hal_timer_stop
`void hal_timer_stop(timer_dev_t *tim);`

- 功能描述:
   - 停止指定的定时器。

- 参数:
   - `tim`: 定时器设备描述。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_timer_para_chg
`int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para);`

- 功能描述:
   - 改变指定定时器的参数。

- 参数:
   - `tim`: 定时器设备描述。
   - `para`: 定时器配置信息。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_timer_finalize
`int32_t hal_timer_finalize(timer_dev_t *tim);`

- 功能描述:
   - 关闭指定定时器。

- 参数:
   - `tim`: 定时器设备描述。

- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
### UART   

#### hal_uart_init
`int32_t hal_uart_init(uart_dev_t *uart);`

- 功能描述:
   - 初始化指定串口。

- 参数:
   - `uart`: 串口设备描述，定义需要初始化的串口参数。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

##### uart_dev_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | uart_config_t | 配置信息 |
| priv | void* | 用户自定义数据 |

##### uart_config_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| baud_rate | uint32_t | 波特率 |
| data_width | hal_uart_data_width_t | 数据宽度 |
| parity | hal_uart_parity_t | 奇偶校验模式|
| stop_bits | hal_uart_stop_bits_t | 数据停止位数 |
| flow_control | hal_uart_flow_control_t | 流控 |
| mode | hal_uart_mode_t | 发送接收模式 |

##### hal_uart_data_width_t

| 类型 | 说明 |
| :--- | :--- |
| DATA_WIDTH_5BIT | 5位 |
| DATA_WIDTH_6BIT | 6位 |
| DATA_WIDTH_7BIT | 7位 |
| DATA_WIDTH_8BIT | 8位 |
| DATA_WIDTH_9BIT | 9位 |

##### hal_uart_parity_t

| 类型 | 说明 |
| :--- | :--- |
| NO_PARITY | 无校验 |
| ODD_PARITY | 奇校验 |
| EVEN_PARITY | 偶校验 |

##### hal_uart_stop_bits_t

| 类型 | 说明 |
| :--- | :--- |
| STOP_BITS_1 | 1位 |
| STOP_BITS_2 | 2位 |

##### hal_uart_flow_control_t

| 类型 | 说明 |
| :--- | :--- |
| FLOW_CONTROL_DISABLED | 禁用流控 |
| FLOW_CONTROL_CTS | 清除发送 |
| FLOW_CONTROL_RTS | 请求发送 |
| FLOW_CONTROL_CTS_RTS | 支持CTS和RTS流控 |

##### hal_uart_mode_t

| 类型 | 说明 |
| :--- | :--- |
| MODE_TX | 发送模式 |
| MODE_RX | 接收模式 |
| MODE_TX_RX | 支持发送和接收两种模式 |


#### hal_uart_send
`int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout);`

- 功能描述:
   - 从指定的串口发送数据。

- 参数:
   - `uart`: 串口设备描述句柄。
   - `data`: 指向要发送数据的数据指针。
   - `size`: 要发送的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_uart_recv
`int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout);`

- 功能描述:
   - 从指定的串口接收数据。

- 参数:
   - `uart`: 串口设备描述句柄。
   - `data`: 指向接收缓冲区的数据指针。
   - `size`: 期望接收的数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_uart_recv_II
`int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout);`

- 功能描述:
   - 从指定的串口中断方式接收数据。

- 参数:
   - `uart`: 串口设备描述句柄。
   - `data`: 指向接收缓冲区的数据指针。
   - `expect_size`: 期望接收的数据字节数。
   - `recv_size`: 实际接收数据字节数。
   - `timeout`: 超时时间（单位ms），如果希望一直等待设置为HAL_WAIT_FOREVER。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_uart_finalize
`int32_t hal_uart_finalize(uart_dev_t *uart);`

- 功能描述:
   - 关闭指定串口。

- 参数:
   - `uart`: 串口设备描述句柄。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

### WDG

#### hal_wdg_init
`int32_t hal_wdg_init(wdg_dev_t *wdg);`

- 功能描述:
   - 初始化指定的看门狗。

- 参数:
   - `wdg`: 看门狗设备描述。

- 返回值:
   - 0: 成功。
   - -1: 失败。

##### wdg_dev_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | uint8_t | 端口 |
| config | wdg_config_t | 配置信息 |
| priv | void* | 用户自定义数据 |

##### wdg_config_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| timeout | uint32_t | 超时时间 |

#### hal_wdg_reload
`void hal_wdg_reload(wdg_dev_t *wdg);`

- 功能描述:
   - 重载指定看门狗，喂狗。

- 参数:
   - `wdg`: 看门狗设备描述。

- 返回值:
   - 无。

#### hal_wdg_finalize
`int32_t hal_wdg_finalize(wdg_dev_t *wdg);`

- 功能描述:
   - 关闭指定看门狗。

- 参数:
   - `wdg`: 看门狗设备描述。

- 返回值:
   - 0: 成功。
   - -1: 失败。

#### usbd_hal_init
`usbd_stat_t usbd_hal_init(void *pdev);`

- 功能描述:
   - 初始化usb设备驱动。

- 参数:
   - `pdev`: usb设备句柄。

- 返回值:
   - USBD_OK: 成功。
   - USBD_BUSY: 忙。
   - others: 失败。

#### usbd_hal_deinit
`usbd_stat_t usbd_hal_deinit(void *pdev);`

- 功能描述:
   - 去初始化usb设备驱动。

- 参数:
   - `pdev`: usb设备句柄。

- 返回值:
   - USBD_OK: 成功。
   - USBD_BUSY: 忙。
   - others: 失败。

#### usbd_hal_start
`usbd_stat_t usbd_hal_start(void *pdev);`

- 功能描述:
   - 启动usb设备驱动。

- 参数:
   - `pdev`: usb设备句柄。

- 返回值:
   - USBD_OK: 成功。
   - USBD_BUSY: 忙。
   - others: 失败。

#### usbd_hal_stop
`usbd_stat_t usbd_hal_stop(void *pdev);`

- 功能描述:
   - 停止usb设备驱动。

- 参数:
   - `pdev`: usb设备句柄。

- 返回值:
   - USBD_OK: 成功。
   - USBD_BUSY: 忙。
   - others: 失败。

#### usbd_hal_connect
`void usbd_hal_connect(void);`

- 功能描述:
   - 启用usb设备中断。

- 参数:
   - 无。

- 返回值:
   - 无。

#### usbd_hal_disconnect
`void usbd_hal_disconnect(void);`

- 功能描述:
   - 禁用usb设备中断。

- 参数:
   - 无。

- 返回值:
   - 无。

#### usbd_hal_configure_device
`void usbd_hal_configure_device(void);`

- 功能描述:
   - 配置usb设备信息。

- 参数:
   - 无。

- 返回值:
   - 无。

#### usbd_hal_unconfigure_device
`void usbd_hal_unconfigure_device(void);`

- 功能描述:
   - 取消配置usb设备信息。

- 参数:
   - 无。

- 返回值:
   - 无。

#### usbd_hal_set_address
`void usbd_hal_set_address(void *pdev, uint8_t address)`

- 功能描述:
   - 设置usb设备地址。

- 参数:
   - `pdev`: usb设备句柄。
   - `address`: usb设备地址。

- 返回值:
   - 无。

#### usbd_hal_ep0_setup
`void usbd_hal_ep0_setup(uint8_t *buffer);`

- 功能描述:
   - Endpoint0设置（读取设置包）。

- 参数:
   - `buffer`: usb设备句柄。

- 返回值:
   - 无。

#### usbd_hal_ep0_read
`void usbd_hal_ep0_read(void *pdev);`

- 功能描述:
   - Endpoint0读取数据包。

- 参数:
   - `pdev`: usb设备句柄。

- 返回值:
   - 无。

#### usbd_hal_ep0_read_stage
`void usbd_hal_ep0_read_stage(void);`

- 功能描述:
   - Endpoint0读取阶段。

- 参数:
   - 无。

- 返回值:
   - 无。

#### usbd_hal_get_ep0_read_result
`uint32_t usbd_hal_get_ep0_read_result(void *pdev, uint8_t *buffer);`

- 功能描述:
   - Endpoint0获取读取结果。

- 参数:
   - `pdev`: usb设备句柄。
   - `buffer`: 数据包指针。

- 返回值:
   - 无。

#### usbd_hal_ep0_write
`void usbd_hal_ep0_write(void *pdev, uint8_t *buffer, uint32_t size);`

- 功能描述:
   - Endpoint0写入数据包。

- 参数:
   - `pdev`: usb设备句柄。
   - `buffer`: 数据包指针。
   - `size`: 写入数据包的长度。

- 返回值:
   - 无。

#### usbd_hal_get_ep0_write_result
`void usbd_hal_get_ep0_write_result(void);`

- 功能描述:
   - 获取endpoint0写入结果。

- 参数:
   - 无。

- 返回值:
   - 无。

#### usbd_hal_ep0_stall
`void usbd_hal_ep0_stall(void *pdev);`

- 功能描述:
   - 暂停endpoint0。

- 参数:
   - `pdev`: usb设备句柄。

- 返回值:
   - 无。

#### usbd_hal_ep0_read_stage
`bool usbd_hal_realise_endpoint(void *pdev, uint8_t endpoint, uint32_t maxPacket, uint32_t flags);`

- 功能描述:
   - 打开endpoint。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。
   - `maxPacket`: 最大数据包。
   - `flags`: 标志。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### usbd_hal_endpoint_read
`ep_status usbd_hal_endpoint_read(void *pdev, uint8_t endpoint, uint32_t maximumSize);`

- 功能描述:
   - 开始读取endpoint数据。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。
   - `maximumSize`: 读取最大数据量。

- 返回值:
   - ep_status: 端点传输状态。

##### ep_status

| 类型 | 说明 |
| :--- | :--- |
| EP_COMPLETED | 传输完成 |
| EP_PENDING | 正在传输 |
| EP_INVALID | 无效参数 |
| EP_STALLED | 端点已暂停 |

#### usbd_hal_endpoint_read_result
`ep_status usbd_hal_endpoint_read_result(void *pdev, uint8_t endpoint, uint8_t *data, uint32_t *bytesRead);`

- 功能描述:
   - 读取endpoint数据。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。
   - `data`: 接收缓冲区的指针。
   - `bytesRead`: 接收数据量。

- 返回值:
   - ep_status: 端点传输状态。

#### usbd_hal_endpoint_write
`ep_status usbd_hal_endpoint_write(void *pdev, uint8_t endpoint, uint8_t *data, uint32_t size);`

- 功能描述:
   - 开始写入endpoint数据。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。
   - `data`: 写入缓冲区的数据。
   - `size`: 要写入的数据量。

- 返回值:
   - ep_status: 端点传输状态。

#### usbd_hal_endpoint_write_result
`ep_status usbd_hal_endpoint_write_result(void *pdev, uint8_t endpoint);`

- 功能描述:
   - 获取写入endpoint的数据状态。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。

- 返回值:
   - ep_status: 端点传输状态。

#### usbd_hal_stall_endpoint
`void usbd_hal_stall_endpoint(void *pdev, uint8_t endpoint);`

- 功能描述:
   - 暂停endpoint。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。

- 返回值:
   - 无。

#### usbd_hal_unstall_endpoint
`void usbd_hal_unstall_endpoint(void *pdev, uint8_t endpoint);`

- 功能描述:
   - 取消暂停endpoint。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。

- 返回值:
   - 无。

#### usbd_hal_get_endpoint_stall_state
`bool usbd_hal_get_endpoint_stall_state(void *pdev, uint8_t endpoint);`

- 功能描述:
   - 获取暂停endpoint的状态。

- 参数:
   - `pdev`: usb设备句柄。
   - `endpoint`: endpoint编号。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_init
`int hal_usbh_init(void *phost, void **phcd);`

- 功能描述:
   - 初始化usb主机控制器。

- 参数:
   - `phost`: usb主机句柄。
   - `phcd`: 指向usb hcd驱动程序指针的指针。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_finalize
`int hal_usbh_finalize(void *hcd);`

- 功能描述:
   - 完成usb主机控制器。

- 参数:
   - `hcd`: usb hcd驱动程序指针。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_port_reset
`int hal_usbh_port_reset(void *hcd);`

- 功能描述:
   - 重置usb主机控制器端口。

- 参数:
   - `hcd`: usb hcd驱动程序指针。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_get_speed
`int hal_usbh_get_speed(void *hcd);`

- 功能描述:
   - 获取设备速度。

- 参数:
   - `hcd`: usb hcd驱动程序指针。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_pipe_free
`int hal_usbh_pipe_free(void *hcd, uint8_t pipe_num);`

- 功能描述:
   - 释放主机控件的管道。

- 参数:
   - `hcd`: usb hcd驱动程序指针。
   - `pipe_num`: 管道号。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_pipe_configure
`int hal_usbh_pipe_configure(void *hcd, uint8_t index, uint8_t ep_addr, uint8_t dev_addr, uint8_t speed, uint8_t ep_type, uint16_t mps);`

- 功能描述:
   - 配置主机控制器的管道。

- 参数:
   - `hcd`: usb hcd驱动程序指针。
   - `index`: 管道号。
   - `ep_addr`: endpoint地址。
   - `dev_addr`: 设备地址。
   - `speed`: 设备速度。
   - `ep_type`: endpoint类型。 
   - `mps`: 每次传输的最大数据包大小。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_submit_urb
`int hal_usbh_submit_urb(void *hcd, uint8_t pipe_num, uint8_t direction, uint8_t ep_type, uint8_t token, uint8_t *buf, uint16_t length);`

- 功能描述:
   - 提交Urb，开始发送或接收数据。

- 参数:
   - `hcd`: usb hcd驱动程序指针。
   - `pipe_num`: 管道号。
   - `direction`: 传输方向。
   - `ep_type`: endpoint类型。
   - `token`: 传输token。
   - `buf`: 指向要发送或接收的缓冲区的指针。 
   - `length`: 数据长度。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

#### hal_usbh_get_urb_state
`int hal_usbh_get_urb_state(void *hcd, uint8_t pipe_num);`

- 功能描述:
   - 获取Urb传输状态。

- 参数:
   - `hcd`: usb hcd驱动程序指针。
   - `pipe_num`: 管道号。

- 返回值:
   - 0: Idle。
   - 1: Done。
   - 2: Not Ready。
   - 3: Nyet。
   - 4: Error。
   - 5: Stall。


## 示例

### ADC示例

```c
#include <aos/hal/adc.h>

#define ADC1_PORT_NUM 1

/* define dev */
adc_dev_t adc1;

int application_start(int argc, char *argv[])
{
    int ret   = -1;
    int value = 0;

    adc_config_t adc_cfg;

    /* adc port set */
    adc1.port = ADC1_PORT_NUM;

    /* set sampling_cycle */
    adc1.config.sampling_cycle = 100;

    /* init adc1 with the given settings */
    ret = hal_adc_init(&adc1);
    if (ret != 0) {
        printf("adc1 init error !\n");
    }

    /* get adc value */
    ret = hal_adc_value_get(&adc1, &value, HAL_WAIT_FOREVER);
    if (ret != 0) {
        printf("adc1 vaule get error !\n");
    }

    /* finalize adc1 */
    hal_adc_finalize(&adc1);

    while(1) {
        /* sleep 500ms */
        aos_msleep(500);
    };
}
```

### DAC示例

```c
#include <aos/hal/dac.h>

#define DAC1_PORT_NUM 1
#define DAC1_CHANNEL_NUM 1

/* define dev */
dac_dev_t dac1;

int application_start(int argc, char *argv[])
{
    int ret   = -1;
    int value = 0;
    static int count = 0;

    /* dac port set */
    dac1.port = DAC1_PORT_NUM;

    /* init dac1 with the given settings */
    ret = hal_dac_init(&dac1);
    if (ret != 0) {
        printf("dac1 init error !\n");
    }

    value = 10;

    ret = hal_dac_set_value(&dac1, DAC1_CHANNEL_NUM, value);
    if (ret != 0) {
        printf("dac1 set value error !\n");
    }

    /* start dac output */
    ret = hal_dac_start(&dac1, DAC1_CHANNEL_NUM);
    if (ret != 0) {
        printf("dac1 start error !\n");
    }

    while(1) {
        if (count == 10) {
            /* finalize dac1 */
            hal_dac_finalize(&dac1);
        }

        /* sleep 500ms */
        aos_msleep(500);

        count++;
    };
}
```

### FLASH示例

```
uint32_t off = 0; 
char buf[512] = {0};
hal_partition_t in_partition;
uint32_t off_set=0;
hal_logic_partition_t partition_info = {0};
ret = hal_flash_info_get(HAL_PARTITION_APPLICATION,&partition_info);
ret = hal_flash_erase(HAL_PARTITION_APPLICATION,0,512);
ret = hal_flash_write(HAL_PARTITION_APPLICATION,&off,buf,512);
ret = hal_flash_erase_write(HAL_PARTITION_APPLICATION,&off,buf,512);
ret = hal_flash_read(HAL_PARTITION_APPLICATION,&off,buf,512);
ret = hal_flash_enable_secure(HAL_PARTITION_APPLICATION,0,512);
ret = hal_flash_dis_secure(HAL_PARTITION_APPLICATION,0,512);
ret = hal_flash_addr2offset(&in_partition,&off_set,0x400000);
```

### GPIO示例

#### GPIO作为输出

```c
#include <aos/hal/gpio.h>

#define GPIO_LED_IO 18

/* define dev */
gpio_dev_t led;

int application_start(int argc, char *argv[])
{
    int ret = -1;

    /* gpio port config */
    led.port = GPIO_LED_IO;

    /* set as output mode */
    led.config = OUTPUT_PUSH_PULL;

    /* configure GPIO with the given settings */
    ret = hal_gpio_init(&led);
    if (ret != 0) {
        printf("gpio init error !\n");
    }

    /* output high */
    hal_gpio_output_high(&led);

    /* output low */
    hal_gpio_output_low(&led);

    /* toggle the LED every 1s */
    while(1) {

        /* toggle output */
        hal_gpio_output_toggle(&led);

        /* sleep 1000ms */
        aos_msleep(1000);
    };
}
```

注: port为逻辑端口号，其与物理端口号的对应关系见具体的对接实现

#### GPIO作为中断输入

```c
#include <aos/hal/gpio.h>

#define GPIO_BUTTON_IO 5

/* define dev */
gpio_dev_t button1;

/* pressed flag */
int button1_pressed = 0;

void button1_handler(void *arg)
{
    button1_pressed = 1;
}

int application_start(int argc, char *argv[])
{
    int ret = -1;

    /* input pin config */
    button1.port = GPIO_BUTTON_IO;

    /* set as interrupt mode */
    button1.config = IRQ_MODE;

    /* configure GPIO with the given settings */
    ret = hal_gpio_init(&button1);
    if (ret != 0) {
        printf("gpio init error !\n");
    }

    /* gpio interrupt config */
    ret = hal_gpio_enable_irq(&button1, IRQ_TRIGGER_BOTH_EDGES, 
                              button1_handler, NULL);
    if (ret != 0) {
        printf("gpio irq enable error !\n");
    }

    /* if button is pressed, print "button 1 is pressed !" */
    while(1) {
        if (button1_pressed == 1) {
            button1_pressed = 0;
            printf("button 1 is pressed !\n");
        }

        /* sleep 100ms */
        aos_msleep(100);
    };
}
```

### I2C示例

```c
#include "aos/hal/i2c.h"

#define I2C1_PORT_NUM  1
#define I2C_BUF_SIZE   10
#define I2C_TX_TIMEOUT 10
#define I2C_RX_TIMEOUT 10

#define I2C_DEV_ADDR       0x30f
#define I2C_DEV_ADDR_WIDTH 8

/* define dev */
i2c_dev_t i2c1;

/* data buffer */
char i2c_data_buf[I2C_BUF_SIZE];

int application_start(int argc, char *argv[])
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* i2c port set */
    i2c1.port = I2C1_PORT_NUM;

    /* i2c attr config */
    i2c1.config.mode          = I2C_MODE_MASTER;
    i2c1.config.freq          = 30000000;
    i2c1.config.address_width = I2C_DEV_ADDR_WIDTH;
    i2c1.config.dev_addr      = I2C_DEV_ADDR;

    /* init i2c1 with the given settings */
    ret = hal_i2c_init(&i2c1);
    if (ret != 0) {
        printf("i2c1 init error !\n");
    }

    /* init the tx buffer */
    for (i = 0; i < I2C_BUF_SIZE; i++) {
        i2c_data_buf[i] = i + 1;
    }

    /* send 0,1,2,3,4,5,6,7,8,9 by i2c1 */
    ret = hal_i2c_master_send(&i2c1, I2C_DEV_ADDR, i2c_data_buf,
                              I2C_BUF_SIZE, I2C_TX_TIMEOUT);
    if (ret == 0) {
        printf("i2c1 data send succeed !\n");
    }

    ret = hal_i2c_master_recv(&i2c1, I2C_DEV_ADDR, i2c_data_buf,
                              I2C_BUF_SIZE, I2C_RX_TIMEOUT);
    if (ret == 0) {
        printf("i2c1 data received succeed !\n");
    }

    while(1) {
        printf("AliOS Things is working !\n");

        /* sleep 1000ms */
        aos_msleep(1000);
    };
}
```

#### PWM示例

```c
#include <aos/hal/pwm.h>

#define PWM1_PORT_NUM 1

/* define dev */
pwm_dev_t pwm1;

int application_start(int argc, char *argv[])
{
    int ret = -1;
    pwm_config_t pwm_cfg;
    static int count = 0;

    /* pwm port set */
    pwm1.port = PWM1_PORT_NUM;

    /* pwm attr config */
    pwm1.config.duty_cycle = 0.5f; /* 1s */
    pwm1.config.freq       = 300000; /* 1s */

    /* init pwm1 with the given settings */
    ret = hal_pwm_init(&pwm1);
    if (ret != 0) {
        printf("pwm1 init error !\n");
    }

    /* start pwm1 */
    ret = hal_pwm_start(&pwm1);
    if (ret != 0) {
        printf("pwm1 start error !\n");
    }

    while(1) {

        /* change the duty cycle to 30% */
        if (count == 5) {
            memset(&pwm_cfg, 0, sizeof(pwm_config_t));
            pwm_cfg.duty_cycle = 0.3f;

            ret = hal_pwm_para_chg(&pwm1, pwm_cfg);
            if (ret != 0) {
                printf("pwm1 para change error !\n");
            }
        }

        /* stop and finalize pwm1 */
        if (count == 20) {
            hal_pwm_stop(&pwm1);
            hal_pwm_finalize(&pwm1);
        }

        /* sleep 1000ms */
        aos_msleep(1000);
        count++;
    };
}
```

### RNG示例

```c
#include <aos/hal/rng.h>

#define RNG1_PORT_NUM 1

/* define dev */
random_dev_t rng1;

int application_start(int argc, char *argv[])
{
    int ret   = -1;
    int value = 0;

    rng1.port = RNG1_PORT_NUM;

    ret = hal_random_num_read(rng1, &value, sizeof(int));
    if (ret != 0) {
        printf("rng read error !\n");
    }

    while(1) {

        /* sleep 500ms */
        aos_msleep(500);
    };
}
```

### RTC示例

```c
#include <aos/hal/rtc.h>

#define RTC1_PORT_NUM 1

/* define dev */
rtc_dev_t rtc1;

int application_start(int argc, char *argv[])
{
    int ret = -1;

    rtc_config_t rtc_cfg;
    rtc_time_t   time_buf;

    /* rtc port set */
    rtc1.port = RTC1_PORT_NUM;

    /* set to DEC format */
    rtc1.config.format = HAL_RTC_FORMAT_DEC;

    /* init rtc1 with the given settings */
    ret = hal_rtc_init(&rtc1);
    if (ret != 0) {
        printf("rtc1 init error !\n");
    }

    time_buf.sec     = 0;
    time_buf.min     = 0;
    time_buf.hr      = 0;
    time_buf.weekday = 2;
    time_buf.date    = 1;
    time_buf.month   = 1;
    time_buf.year    = 2019;

    /* set rtc1 time to 2019/1/1,00:00:00 */
    ret = hal_rtc_set_time(&rtc1, &time_buf);
    if (ret != 0) {
        printf("rtc1 set time error !\n");
    }

    memset(&time_buf, 0, sizeof(rtc_time_t));

    /* get rtc current time */
    ret = hal_rtc_get_time(&rtc1, &time_buf);
    if (ret != 0) {
        printf("rtc1 get time error !\n");
    }

    /* finalize rtc1 */
    hal_rtc_finalize(&rtc1);

    while(1) {
        /* sleep 500ms */
        aos_msleep(500);
    };
}
```

### SPI示例

```c
#include <aos/hal/spi.h>

#define SPI1_PORT_NUM  1
#define SPI_BUF_SIZE   10
#define SPI_TX_TIMEOUT 10
#define SPI_RX_TIMEOUT 10

/* define dev */
spi_dev_t spi1;

/* data buffer */
char spi_data_buf[SPI_BUF_SIZE];

int application_start(int argc, char *argv[])
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* spi port set */
    spi1.port = SPI1_PORT_NUM;

    /* spi attr config */
    spi1.config.mode  = HAL_SPI_MODE_MASTER;
    spi1.config.freq = 30000000;

    /* init spi1 with the given settings */
    ret = hal_spi_init(&spi1);
    if (ret != 0) {
        printf("spi1 init error !\n");
    }

    /* init the tx buffer */
    for (i = 0; i < SPI_BUF_SIZE; i++) {
        spi_data_buf[i] = i + 1;
    }

    /* send 0,1,2,3,4,5,6,7,8,9 by spi1 */
    ret = hal_spi_send(&spi1, spi_data_buf, SPI_BUF_SIZE, SPI_TX_TIMEOUT);
    if (ret == 0) {
        printf("spi1 data send succeed !\n");
    }

    /* scan spi every 100ms to get the data */
    while(1) {
        ret = hal_spi_recv(&spi1, spi_data_buf, SPI_BUF_SIZE, SPI_RX_TIMEOUT);
        if (ret == 0) {
            printf("spi1 data received succeed !\n");
        }

        /* sleep 100ms */
        aos_msleep(100);
    };
}
```

### TIMER示例

```c
#include <aos/hal/timer.h>

#define TIMER1_PORT_NUM 1

/* define dev */
timer_dev_t timer1;

void timer_handler(void *arg)
{
    static int timer_cnt = 0;

    printf("timer_handler: %d times !\n", timer_cnt++);
}

int application_start(int argc, char *argv[])
{
    int ret = -1;
    timer_config_t timer_cfg;
    static int count = 0;

    /* timer port set */
    timer1.port = TIMER1_PORT_NUM;

    /* timer attr config */
    timer1.config.period         = 1000000; /* 1s */
    timer1.config.reload_mode    = TIMER_RELOAD_AUTO;
    timer1.config.cb             = timer_handler;

    /* init timer1 with the given settings */
    ret = hal_timer_init(&timer1);
    if (ret != 0) {
        printf("timer1 init error !\n");
    }

    /* start timer1 */
    ret = hal_timer_start(&timer1);
    if (ret != 0) {
        printf("timer1 start error !\n");
    }

    while(1) {

        /* change the period to 2s */
        if (count == 5) {
            memset(&timer_cfg, 0, sizeof(timer_config_t));
            timer_cfg.period = 2000000;

            ret = hal_timer_para_chg(&timer1, timer_cfg);
            if (ret != 0) {
                printf("timer1 para change error !\n");
            }
        }

        /* stop and finalize timer1 */
        if (count == 20) {
            hal_timer_stop(&timer1);
            hal_timer_finalize(&timer1);
        }

        /* sleep 1000ms */
        aos_msleep(1000);
        count++;
    };
}
```

### UART示例

```c
#include <aos/hal/uart.h>

#define UART1_PORT_NUM  1
#define UART_BUF_SIZE   10
#define UART_TX_TIMEOUT 10
#define UART_RX_TIMEOUT 10

/* define dev */
uart_dev_t uart1;

/* data buffer */
char uart_data_buf[UART_BUF_SIZE];

int application_start(int argc, char *argv[])
{
    int count   = 0;
    int ret     = -1;
    int i       = 0;
    int rx_size = 0;

    /* uart port set */
    uart1.port = UART1_PORT_NUM;

    /* uart attr config */
    uart1.config.baud_rate    = 115200;
    uart1.config.data_width   = DATA_WIDTH_8BIT;
    uart1.config.parity       = NO_PARITY;
    uart1.config.stop_bits    = STOP_BITS_1;
    uart1.config.flow_control = FLOW_CONTROL_DISABLED;
    uart1.config.mode         = MODE_TX_RX;

    /* init uart1 with the given settings */
    ret = hal_uart_init(&uart1);
    if (ret != 0) {
        printf("uart1 init error !\n");
    }

    /* init the tx buffer */
    for (i = 0; i < UART_BUF_SIZE; i++) {
        uart_data_buf[i] = i + 1;
    }

    /* send 0,1,2,3,4,5,6,7,8,9 by uart1 */
    ret = hal_uart_send(&uart1, uart_data_buf, UART_BUF_SIZE, UART_TX_TIMEOUT);
    if (ret == 0) {
        printf("uart1 data send succeed !\n");
    }

    /* scan uart1 every 100ms, if data received send it back */
    while(1) {
        ret = hal_uart_recv_II(&uart1, uart_data_buf, UART_BUF_SIZE,
                               &rx_size, UART_RX_TIMEOUT);
        if ((ret == 0) && (rx_size == UART_BUF_SIZE)) {
            printf("uart1 data received succeed !\n");

            ret = hal_uart_send(&uart1, uart_data_buf, rx_size, UART_TX_TIMEOUT);
            if (ret == 0) {
                printf("uart1 data send succeed !\n");
            }
        }

        /* sleep 100ms */
        aos_msleep(100);
    };
}
```

### WDG示例

```c
#include <aos/hal/wdg.h>

#define WDG1_PORT_NUM 1

/* define dev */
wdg_dev_t wdg1;

int application_start(int argc, char *argv[])
{
    int ret = -1;
    static int count = 0;

    /* wdg port set */
    wdg1.port = WDG1_PORT_NUM;

    /* set reload time to 1000ms */
    wdg1.config.timeout = 1000; /* 1000ms */

    /* init wdg1 with the given settings */
    ret = hal_wdg_init(&wdg1);
    if (ret != 0) {
        printf("wdg1 init error !\n");
    }

    while(1) {
        /* clear wdg about every 500ms */
        hal_wdg_reload(&wdg1);

        /* finalize wdg1 */
        if (count == 20) {
            hal_wdg_finalize(&wdg1);
        }

        /* sleep 500ms */
        aos_msleep(500);
        count++;
    };
}
```

## 诊断错误码
无。

## 运行资源
无。

## 依赖资源
  - minilibc
  - aos

## 组件参考
无。






