# 概述
基于bl606p 和 ch2601芯片的csi codec 测试demo, 只测试minialsa所需相关csi codec api。

# 使用
## CDK
在CDK的首页，通过搜索codec_demo，可以找到codec_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://www.xrvm.cn/document?temp=use-cdk-to-get-started-quickly&slug=yocbook) 章节

## 通过命令行
需要先安装[yoctools](https://www.xrvm.cn/document?temp=yoctools&slug=yocbook)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install codec_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的 XuanTie Debug Server 中显示的对应的内容。

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
烧录完成之后按下复位按键，串口会有打印输出。

通过串口cli输入以下命令

## output

主要测试播放相关api，以下所有命令必须先从output play开始。

### 播放

```cli
output play 
```

### 暂停

```cli
output pause
```

### 恢复

```cli
output resume
```

### 静音

```cli
output mute
```

### 取消静音

```cli
output unmute
```

### 设置增益

```cli
output gain
```

## input

主要是录音相关api

```cli
input
```

在收到类似打印，表示录音大约2000ms数据已完成

```
read size err(0)(0)
```

使用gdb挂入，dump出所需数据，使用相关音频软件查看rec.pcm的录音质量

```
dump binary memory rec.pcm g_read_buffer.buffer g_read_buffer.buffer+g_input_size 
```

## loop

主要用于测试音频输入输出质量

```cli
loop
```
注:播放音乐的时长是大约5000ms，录音的时长大约是2000ms。

在收到类似打印，表示loop完成

```
read buffer is enough 
audio tx stop
```

使用gdb挂入，dump出所需数据，使用相关音频软件查看rec.pcm的录音质量

```
dump binary memory rec.pcm g_read_buf.buffer g_read_buf.buffer+g_recv_size 
```

