# 概述

本实例提供了一个人脸识别的DEMO。

![image.png](./resource/framework.png)

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install face_recognition_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的 XuanTie Debug Server 中显示的对应的内容。

烧录完成之后，按板子上的复位键即可看到串口输出。

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 联网配置

通过串口终端输入:

```cli
kv set wifi_ssid <your_wifi_ssid>
kv set wifi_psk <your_wifi_password>
```

- your_wifi_ssid：你的wifi名字
- your_wifi_password：你的wifi密码

# 运行

```cli
...

```
