# 概述

HRS Profile提供了标准心率服务，包含了心率测量、RR 区间、能量累计、皮肤接触等功能。

HRS应用主要实现了蓝牙广播、建立连接以及心率数据推送的功能。

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
