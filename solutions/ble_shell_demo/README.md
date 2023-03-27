# 概述

`ble_shell` 是一个BLE测试示例。通过Shell命令，可以测试低功耗蓝牙协议栈的基本功能。
测试命令集可以查看《BLE SHELL用户手册》

# 编译

```bash
make clean;make
```

# 烧录

```bash
make flashall
```

# 启动

烧录完成之后按复位键，串口会有打印输出

注：由于 sdk_chip_IoTGW_CB800 平台与 sdk_chip_d1 平台不支持设置 MAC 地址与蓝牙设备名称，所以与 sdk_chip_phy6220 平台不同，蓝牙协议栈初始化为：ble init 不支持设置蓝牙设备名称：ble name 命令。
