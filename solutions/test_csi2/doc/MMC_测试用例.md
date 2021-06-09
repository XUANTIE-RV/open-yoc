## MMC

### 概要
该文档主要描述CSI2.0自动化测试框架下MMC模块的AT测试命令。 文档中所涉及的命令是用来测试emmc模块和sd模块的接口工作是否正常、接口返回值是否符合预期和该模块的特性功能是否正常。

### 测试原理
PC主机用USB转串口线连接测试板（以下简称A板），作为PC同A板数据通信，命令收发的通道；同时PC会使用USB线连接A板的HAD调试器，作为程序下载的通道；A板上需外接emmc card或sd card。通过PC下发AT指令到A板触发测试程序执行，当A板有测试成功的信息返回PC后，PC判定本次测试执行通过。

### 测试流程
- 测试前应在A板上外接emmc card或sd card
- 发送MMC的AT测试命令进行测试

### 命令列表
以下是MMC模块对应的AT测试命令列表及其功能简要介绍。
| AT命令列表                       | 功能简介                   |
| -------------------------------- | -------------------------- |
| AT+MMC_INTERFACE | 测试API接口的异常参数判断 |
| AT+MMC_REPEAT_INIT | 循环初始化和反初始化mmc host控制器和外接的device |
| AT+MMC_GET_SD_INFO | 获取外接的SD card的信息 |
| AT+MMC_GET_EMMC_INFO | 获取外接的EMMC card的信息 |
| AT+MMC_WRITE_READ_EMMC | EMMC card的读写测试 |
| AT+MMC_WRITE_READ_SD | SD card的读写测试 |
| AT+MMC_EMMC_CROSS_CHECK | 测试emmc在指定的范围内读写是否存在越界 |
| AT+MMC_SD_CROSS_CHECK | 测试sd card在指定的范围内读写是否存在越界 |
| AT+MMC_EMMC_SPEED_TEST | 测试EMMC的读、写速率 |
| AT+MMC_SD_SPEED_TEST | 测试SD card的读、写速率 |
| AT+STRESS_MMC | MMC稳定性测试 |
| AT+MMC_CONFIG | 配置MMC在稳定性测试中会使用到的控制器及device的配置 |


### 参数列表
以下是MMC模块对应的AT测试命令共用参数的简要介绍
| 参数名字 | 释义 | 取值范围 |
| -------- | ---- | ------- |
| mshc_idx | host控制器的idx | 0x0 ~ 0xFF,参考对应SOC支持的控制器个数 |
| speed_mode | host与device的速度模式 | 参考命令详解具体描述 |
| vdd | 供给device的总线电压值配置 | 0:3.3V; 1:1.8V; 2:3V; 3:1.2V; 4:0V |
| bus_width | 设置host与device工作的总线宽度 | 参考命令详解具体描述 |
| clk_freq | 设置的总线工作频率 | 0x0 ~ 0xFFFFFFFF |
| rca | 设置的device 设备ID | 0x0 ~ 0xFFFFFFFF,不能设置为0 |
| boot_partition | 选择测试emmc的哪一个分区 | 1:boot1分区; 2:boot2分区 |
| test_choose | 测试组合选择 | 参考命令详解具体描述 |
| repeat_times | 测试重复的次数 | 0x0 ~ 0xFFFFFFFF |
| block_addr | 指定的block地址 | 0x0 ~ 0xFFFFFFFF, block大小对齐 |
| block_cnt | 指定block的块数 | 0x0 ~ 0xFFFFFFFF |
| timer_idx | 选用哪一个timer用来计时 |
| cell_time | 设置计时的最小单元是多少毫秒 |

### 功能测试命令详解
#### AT+MMC_INTERFACE
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_INTERFACE 				    |
| 命令功能 | 测试API接口的异常参数判断          |
| 可搭配的辅助测试命令 | None                   |
| 示例 	   | 步骤1：PC向A板发送AT+MMC_INTERFACE |
| 备注     | 该测试命令不需要跟参数             |   

#### AT+MMC_REPEAT_INIT
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_REPEAT_INIT=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca,test_choose,repeat_times |
| 命令功能 | 测试MMC控制器和device反复初始化和反初始化 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接device<br> 步骤2：PC向A板发送AT+MMC_REPEAT_INIT=0,0,4,2000000,0x12,0,1000 |
| 备注 | |

**test_choose**
| 值  | 含义 |
| --- | --- |
| 1 | 测试外接sd card的反复初始化和反初始化 |
| 2 | 测试外接emmc card的反复初始化和反初始化 |
| other | 测试MMC host控制器的反复初始化和反初始化 |

#### AT+MMC_GET_SD_INFO
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_GET_SD_INFO=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca |
| 命令功能 | 获取sd card的设备信息 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接sd card<br> 步骤2：PC向A板发送AT+MMC_GET_SD_INFO=0,0,1,0,2000000,0x13 |
| 备注 | 获取到的信息需要验证同sd card的硬件描述一致 |

#### AT+MMC_GET_EMMC_INFO
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_GET_EMMC_INFO=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca |
| 命令功能 | 获取emmc card的设备信息 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接emmc card<br> 步骤2：PC向A板发送AT+MMC_GET_EMMC_INFO=0,0,1,0,2000000,0x13 |
| 备注 | 获取到的信息需要验证同emmc card的硬件描述一致 |


#### AT+MMC_WRITE_READ_EMMC
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_WRITE_READ_EMMC=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca,boot_partition,block_addr,block_cnt,test_choose |
| 命令功能 | 读写外接的emmc card指定区间的数据 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接emmc card<br> 步骤2：PC向A板发送AT+MMC_WRITE_READ_EMMC=0,0x0,0x100,2000000,0,1,0,1,0 |
| 备注 | |

**test_choose**
| 值  | 含义 |
| --- | --- |
| 1 | 读取外接emmc card |
| 2 | 写入外接emmc card |
| other | 读写外接的emmc card |

#### AT+MMC_WRITE_READ_SD
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_WRITE_READ_SD=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca,block_addr,block_cnt,test_choose |
| 命令功能 | 读写外接的sd card指定区间的数据 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接sd card<br> 步骤2：PC向A板发送AT+MMC_WRITE_READ_SD=0,0x0,0x100,2000000,0,0,1,0 |
| 备注 | |

**test_choose**
| 值  | 含义 |
| --- | --- |
| 1 | 读取外接sd card |
| 2 | 写入外接sd card |
| other | 读写外接的sd card |


#### AT+MMC_EMMC_CROSS_CHECK
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_EMMC_CROSS_CHECK=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca,boot_partition,block_addr |
| 命令功能 | 测试emmc在指定的范围内读写是否存在越界 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接emmc card<br> 步骤2：PC向A板发送AT+MMC_EMMC_CROSS_CHECK=0,0x0,0x100,2000000,0,1,0 |
| 备注 | |

#### AT+MMC_SD_CROSS_CHECK
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_SD_CROSS_CHECK=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca,block_addr |
| 命令功能 | 测试sd card在指定的范围内读写是否存在越界 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接sd card<br> 步骤2：PC向A板发送AT+MMC_SD_CROSS_CHECK=0,0x0,0x100,2000000,0,0 |
| 备注 | |


### 性能测试命令详解
#### AT+MMC_WRITE_READ_EMMC
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_EMMC_SPEED_TEST=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca,boot_partition,block_addr,timer_idx,cell_time |
| 命令功能 | 测试EMMC的读、写速率 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接emmc card<br> 步骤2：PC向A板发送AT+MMC_EMMC_SPEED_TEST=0,0x0,0x100,2000000,0,1,0,0,10 |
| 备注 | |



#### AT+MMC_WRITE_READ_SD
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_WRITE_READ_SD=mshc_idx,speed_mode,vdd,bus_width,clk_freq,rca,block_addr,timer_idx,cell_time |
| 命令功能 | 测试sd card在指定的范围内读写是否存在越界 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接sd card<br> 步骤2：PC向A板发送AT+MMC_WRITE_READ_SD=0,0x0,0x100,2000000,0,0,0,10 |
| 备注 | |



### 稳定性测试命令详解
#### AT+MMC_CONFIG
|     |     |
| --- | --- |
| 命令格式 | AT+MMC_CONFIG=device_type,speed_mode,vdd,bus_width,clk_freq,rca |
| 命令功能 | 配置需要测试的mmc控制器和device的配置项 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接sd card<br> 步骤2：PC向A板发送AT+MMC_CONFIG=1,0,0x0,0x100,2000000,0 |
| 备注 | 在配置项设置完成后，必须使用AT+STRESS_MMC开启测试 |

**参数解释**
| 参数名 | 释义 |
| --- | --- |
| device_type | 设置指定的外接设备需要测试的配置项, 1: EMMC; 2: SD; other: EMMC&SD |
| speed_mode | host与device的速度模式 |
| vdd | 供给device的总线电压值配置 |
| bus_width | 设置host与device工作的总线宽度 |
| clk_freq | 设置的总线工作频率 |
| rca | 设置的device设备ID |

#### AT+STRESS_MMC
|     |     |
| --- | --- |
| 命令格式 | AT+STRESS_MMC=device_type,emmc_idx,emmc_cnt,sd_idx,sd_cnt,running_times |
| 命令功能 | 开启MMC同外接device的稳定性测试 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接sd card<br> 步骤2：PC向A板发送AT+STRESS_MMC=1,0,128,0,0,10000 |
| 备注 | 在稳定性测试开启之前，必须使用AT+MMC_CONFIG设置测试配置 |

**参数解释**
| 参数名 | 释义 |
| --- | --- |
| device_type | 设置指定的外接设备需要测试的配置项, 1: EMMC; 2: SD; other: EMMC&SD |
| emmc_idx | emmc同哪一个MMC控制器连接 |
| emmc_cnt | emmc memory有多少个block |
| sd_idx | sd card同哪一个MMC控制器连接 |
| sd_cnt | sd card有多少个block |
| running_times | 设置测试运行的次数 |