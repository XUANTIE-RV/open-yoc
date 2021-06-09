## 概述

`sdk_genie_mesh` 为天猫精灵mesh sdk的入口。
整个sdk包含：

- 直接关联天猫精灵3个应用solution：
   - 灯应用：`genie_mesh_light_ctl`
   - 通用固件：`genie_mesh_node_ctl`
   - 多键开关应用：`genie_mesh_switches`
- 通过上述3个应用关联天猫精灵mesh sdk涉及到的组件如`bt_mesh`，`bt_host`等。
- 通过上述3个应用关联天猫精灵定制mesh芯片TG7120B与TG7121B的驱动与板级支持包等。

## 组件安装

```bash
yoc init
yoc install sdk_genie_mesh
```
安装sdk_genie_mesh，会自动安装天猫精灵3个应用solution以及3个应用关联的components与boards。

## 配置

无

## 接口

无

## 编译示例

以genie_mesh_light_ctl为例

### 编译TG7120B上运行的程序

```bash
cd solutions/genie_mesh_light_ctl/
make clean
make
```
生成固件：
完整的烧录固件：solutions/genie_mesh_light_ctl/generated/total_image.hexf
OTA固件：solutions/genie_mesh_light_ctl/generated/fota.bin

### 编译TG7121B上运行的程序

```bash
cd solutions/genie_mesh_light_ctl/
make clean
make SDK=sdk_chip_TG7121B
```
生成固件：
完整的烧录固件：solutions/genie_mesh_light_ctl/generated/total_image.hex
OTA固件：solutions/genie_mesh_light_ctl/generated/fota.bin

**需要注意由于TG7121B内存有限，所以默认关闭了日志打印。**
​
如果需要打开TG7121B日志打印，需要关闭genie OTA功能，并调整分区表，做如下修改。
- 在boards/TG7121B_evb/configs/config.yaml中修改如下：
```bash
partitions:
  - { name: init,    address: 0x18000000, size: 0x007000, update: NONE }
  - { name: kv,      address: 0x18007000, size: 0x002000, update: NONE }
  - { name: imtb,    address: 0x18009000, size: 0x002000, update: NONE }
  - { name: prim,    address: 0x1800b000, size: 0x035000, update: FULL }
  - { name: misc,    address: 0x18040000, size: 0x01e000, update: NONE }
  #- { name: prim,    address: 0x1800b000, size: 0x028000, update: FULL }
  #- { name: misc,    address: 0x18033000, size: 0x02b000, update: NONE }
  - { name: stack,   address: 0x1805e000, size: 0x021000, update: NONE }
  - { name: otp,     address: 0x1807f000, size: 0x001000, update: NONE }
```
- 在components/chip_TG7121B/drivers/aos_hal_flash.c中修改如下：
```bash
static const hal_logic_partition_t app = {
.partition_owner = HAL_FLASH_SPI,
.partition_description = "prim",
.partition_start_addr = 0x1800b000,
.partition_length = 0x35000,
//.partition_length = 0x28000,
.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
};
static const hal_logic_partition_t misc = {
.partition_owner = HAL_FLASH_SPI,
.partition_description = "misc",
//.partition_start_addr = 0x18033000,
//.partition_length = 0x2b000,
.partition_start_addr = 0x18040000,
.partition_length = 0x1e000,
.partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
};
```
- 使能日志：在boards/TG7121B_evb/package.yaml中关闭NDEBUG定义。
```bash
    #NDEBUG: 1
```
- 重新编译固件。
```bash
cd solutions/genie_mesh_light_ctl/
make clean
make SDK=sdk_chip_TG7121B
```
- 擦除整片flash，再重新烧录固件。
​

**注意：调试完毕后，生产固件需要回退上述修改，重新使能OTA，默认关闭日志打印。**

## 错误码

无

## 依赖资源

- 灯应用：genie_mesh_light_ctl
- 通用固件：genie_mesh_node_ctl
- 多键开关应用：genie_mesh_switches