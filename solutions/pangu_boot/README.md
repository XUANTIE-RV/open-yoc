# 概述

`pangu_boot` 是一个基于`pangu`芯片的bootloader方案

# 编译

```bash
make clean;make
```

# 烧录

编译完成之后会产生boot.bin文件。
请使用以下命令烧录至flash对应的位置。或者拷贝到应用方案相应的位置，打包成一个镜像包进行烧录。

```bash
product flashbin boot.bin <flash_addr> -f PanGu_CK804EF_QSPIFlash_CDK.elf -v
```
