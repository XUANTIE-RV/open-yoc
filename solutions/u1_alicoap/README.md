# 概述

基于u1的alicoap上云示例, 示例中上云数据展示为温湿度及灯状态

# 编译

```bash
make clean;make
```

# 烧录

- 设置拨码SW2 pin2为ON，重新上电后，系统进入刷机模式，等待串口打印`ccccccc...`的信息
- 使用minicom或secureCRT，通过Ymodem协议发送“ck.bin”(所有分区更新需要加入`boards/u1/bootimgs`下的cp.bin及sp.bin)
- 设置拨码SW2 pin2置原位，重启即可


# 命令

### 三要素设置
打开从OCC平台获取的三要素文档`XXXX_cloudparm.txt`,使用 `factory setali` 对应的命令即可

设置完毕重启开发板

