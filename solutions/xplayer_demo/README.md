# 概述
xplayer_demo是一个xplayer播放器框架命令行使用示例。

# 使用
## CDK
在CDK的首页，通过搜索xplayer_demo，可以找到xplayer_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install xplayer_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

#### D1平台

1. 编译

```bash
make clean
make SDK=sdk_chip_d1
```

2. 烧写

```bash
make flashall SDK=sdk_chip_d1
```

#### bl606P平台

1. 编译

```bash
make clean
make SDK=sdk_chip_bl606p_e907
```

2. 烧写

```bash
make flashall SDK=sdk_chip_bl606p_e907
```

#### ch2601平台

由于CH2601 RAM资源比较有限，所以针对xplayer demo需要使用另一个连接脚本`gcc_flash_heap_bigger.ld`，具体在component/chip_ch2601下。需要替换
`component/chip_ch2601/package.ymal`文件中的如下部分：

```yaml
hw_info:
  arch_name: riscv
  vendor_name: thead
  cpu_name: e906
  ld_script: gcc_flash_heap_bigger.ld    # gcc_flash.ld
  toolchain_prefix: riscv64-unknown-elf
  flash_program: ch2601_flash.elf
```

由于资源受限，CH2601平台的xplayer demo暂不支持网络歌曲的播放。

1. 编译

```bash
make clean
make SDK=sdk_chip_ch2601
```

2. 烧写

```bash
make flashall SDK=sdk_chip_ch2601
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后按下复位按键系统启动，串口会有打印输出。

## 播放内置内存中的歌曲

```cli
xplayer play inter
```

## 播放内置文件系统(littlefs)中的歌曲

1. 查看内置音乐

   ```cli
   ls /mnt
   ```

2. 播放

   ```cli
   xplayer play file:///mnt/pingfan_10s.mp3
   ```

## 播放网络歌曲

```cli
xplayer play https://yocbook.oss-cn-hangzhou.aliyuncs.com/av_repo/alibaba.mp3
```

**播放网络歌曲之前请先通过ifconfig ap your_ssid your_pwd命令配置网络**

## 停止播放

```cli
xplayer stop
```

**开始播放之前需要先停止上一次的播放**

