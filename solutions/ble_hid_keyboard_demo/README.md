# 概述

蓝牙解决方案里的HID Keyboard是一个典型的BLEPeripheral应用。本方案是一个HID Keyboard示例程序，实例中包括应用初始化、报告描述符定义、蓝牙协议栈事件处理、键盘指示灯控制、按键键值的上报、电池电量的更新上报和设备信息的配置等。

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
