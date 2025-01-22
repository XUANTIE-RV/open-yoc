# 概述

`bare_core_tcm` 是一个玄铁RTOS SDK中面向baremetal领域的玄铁处理器紧耦合存储模块(TCM)使用示例。

示例代码中将tcm_code.c对应的代码段等放到_itcm_code中(参考solutions/bare_core_tcm/gcc_flash_tcm_smartl.ld链接脚本中的用法)。系统启动后将_itcm_code等段中的数据拷贝到ITCM/DTCM指定的地址空间中并使能，最后执行tcm_code.c相关函数。

玄铁处理器对于TCM设计有以下特点：

- ITCM/DTCM的基地址不是固定的，可通过设置对应寄存器灵活配置地址空间。可参考示例代码中csi_dtcm_set_base_addr和csi_itcm_set_base_addr

- ITCM/DTCM的使能需要单独设置对应寄存器。可参考示例代码中csi_dtcm_enable和csi_itcm_enable

- ITCM/DTCM的区域大小有soc设计决定。玄铁提供的参考设计ITCM/DTCM默认均为32KB。具体大小也可通过接口csi_itcm_get_size获取

- 对于smartl平台，solutions/bare_core_tcm/gcc_flash_tcm_smartl.ld链接脚本中ITCM/DTCM的基地址分别设置为0x30000000和0x30008000。对于xiaohui平台，solutions/bare_core_tcm/gcc_flash_tcm_xiaohui.ld链接脚本中ITCM/DTCM的基地址分别设置为0x50200000和0x50300000。

- 链接脚本中的ITCM和DTCM地址是虚拟的，不占用实际外设等地址空间。于此同时代码中若使用时，需要与这两个地址对应起来

注意：该示例当前仅支持在玄铁带硬件tcm模块的处理器型号上编译运行

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        e907 e907f e907fd e907p e907fp e907fdp <br />
        r908 r908fd r908fdv r908-cp r908fd-cp r908fdv-cp <br />
- platform: <br />
        smartl xiaohui

以玄铁`e907fdp`类型为例，可以使用如下命令编译:
```bash
./do_build.sh e907fdp smartl
```

## 运行

示例可以基于玄铁QEMU或FPGA平台运行。

### 基于玄铁QEMU运行

```bash
qemu-system-riscv32 -machine smartl -cpu e907fdp -nographic -kernel yoc.elf
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果
正常运行串口输出内容参考如下
```
bare_core_tcm demo start!
[9]: execute in tcm region
[8]: execute in tcm region
[7]: execute in tcm region
[6]: execute in tcm region
[5]: execute in tcm region
[4]: execute in tcm region
[3]: execute in tcm region
[2]: execute in tcm region
[1]: execute in tcm region
[0]: execute in tcm region
bare_core_tcm runs success!
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁RTOS SDK用户手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁RTOS SDK用户手册》

2、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁RTOS SDK用户手册》

2、如果出现无法编译情况，请使用`sudo pip install yoctools -U`更新最新版本的yoctools再做尝试。

3、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

4、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。

