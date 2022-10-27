# 概述

`ble ibeacon` 是一个简单的蓝牙beacon应用，主要适用于安卓系统

# 编译

```bash
make clean;make BOARD=“board name”
```

# 烧录

```bash
make flashall
```

# 启动

烧录完成之后按复位键，串口会有打印输出

#注意事项
对于beacon设备，若要实现相对准确的距离测量，需根据具体的开发板，测量0m位置处的Tx功率信息，参考做法是测量1米位置处的接收信号强度，则0m位置处功率为1米处接收信号强度+42db，应用中url_list/info_list数组的第四个参数代表0m位置处信号强度，默认为0db,用户可根据实际测量值进行修改
