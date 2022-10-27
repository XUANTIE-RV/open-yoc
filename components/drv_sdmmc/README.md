

## 概述

包含SD卡，SDIO，MMC 的协议层的驱动，不包含SD控制器驱动。

## 配置

无

## 接口列表

| 函数                   | 说明                                |
| :--------------------- | :---------------------------------- |
| SDIO_Init              | sdio初始化                          |
| SDIO_Deinit            | sdio去初始化                        |
| SDIO_IO_Write_Direct   | IO direct write transfer function   |
| SDIO_IO_Read_Direct    | IO direct read transfer function    |
| SDIO_IO_Write_Extended | IO extended write transfer function |
| SDIO_IO_Read_Extended  | IO extended read transfer function  |



## 接口详细说明

### SDIO_IO_Write_Direct

`status_t SDIO_IO_Write_Direct(sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *data, bool raw);`

- 功能描述:
  - IO direct write transfer function。
- 参数:
  - `card`:  Card descriptor
  - `func`: IO numner
  - `regAddr`:  address
  - `data`: data pinter to write
  - `raw`：flag, indicate read after write or write only
- 返回值:
  - kStatus_Success
  - kStatus_SDMMC_TransferFailed

### SDIO_IO_Read_Direct

`status_t SDIO_IO_Read_Direct(sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *data);`

- 功能描述:
  - IO direct readtransfer function。
- 参数:
  - `card`:  Card descriptor
  - `func`: IO numner
  - `regAddr`:  address
  - `data`: data pinter to write
- 返回值:
  - kStatus_Success
  - kStatus_SDMMC_TransferFailed

### SDIO_IO_Write_Extended

`status_t SDIO_IO_Write_Extended(sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *buffer, uint32_t count, uint32_t flags);`

- 功能描述:
  -  IO extended writetransfer function。
- 参数:
  - `card`:  Card descriptor
  - `func`: IO numner
  - `regAddr`:  address
  - `buffer`: data pinter to write
  - `count`：data count
  - `flags`：write flags
- 返回值:
  - kStatus_Success
  - kStatus_SDMMC_TransferFailed
  - kStatus_SDMMC_SDIO_InvalidArgument

### SDIO_IO_Read_Extended

`status_t SDIO_IO_Read_Extended(sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *buffer, uint32_t count, uint32_t flags);`

- 功能描述:
  -  IO extended read transfer function。
- 参数:
  - `card`:  Card descriptor
  - `func`: IO numner
  - `regAddr`:  address
  - `buffer`: data pinter to write
  - `count`：data count
  - `flags`：write flags
- 返回值:
  - kStatus_Success
  - kStatus_SDMMC_TransferFailed
  - kStatus_SDMMC_SDIO_InvalidArgument



## 示例代码

无

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。