# 概述

基于`pangu`芯片的player demo

# 编译

```bash
make clean;make
```

# 烧录

```
cp gdbinit .gdbinit
```

修改IP地址，IP地址从DebugServer处获取。

```
target remote 192.168.56.1:1025
```

```bash
make flashall
```

# 命令

命令通过串口cli输入。

## 停止播放

```
player stop
```

**开始播放之前需要先停止上一次的播放**

## 播放内置歌曲

```
player start pf
```

## 播放网络歌曲

```
player start http://xx.xx.xx.xx/hello.mp3
```