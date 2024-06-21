# 概述
xplayer_demo是一个xplayer播放器框架命令行使用示例。

# 使用
## CDK
在CDK的首页，通过搜索xplayer_demo，可以找到xplayer_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://www.xrvm.cn/document?temp=use-cdk-to-get-started-quickly&slug=yocbook) 章节

## 通过命令行
需要先安装[yoctools](https://www.xrvm.cn/document?temp=yoctools&slug=yocbook)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install xplayer_demo
```

### 切换系统内核

#### 切换到RHINO

默认支持RHINO 无需切换, 如果需要从其他系统内核切换到RHINO，需要修改回去即可。

#### 切换到RT-Thread

##### D1平台

比如在develop分支上面，需要修改`component/sdk_chip_d1/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`ble_host`,`freertos` 最终如下所示:
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
  #- ble_host: develop
```

##### bl606P平台

比如在develop分支上面，需要修改`component/sdk_chip_bl606p_e907/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`bl606p_bthost`,`bl606p_blimpls`,`freertos`如下所示
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
  #- bl606p_bthost: develop
  #- bl606p_blimpls: develop
  ......
```

##### ch2601平台

比如在develop分支上面，需要修改`component/sdk_chip_ch2601/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`freertos`如下所示
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
```

##### f133平台

比如在develop分支上面，需要修改`component/sdk_chip_f133/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`ble_host`,`freertos` 最终如下所示:
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
  #- ble_host: develop
  ......
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的 XuanTie Debug Server 中显示的对应的内容。

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

#### f133平台

1. 编译

```bash
make clean
make SDK=sdk_chip_f133
```

2. 烧写

```bash
make flashall SDK=sdk_chip_f133
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

#### cv181xh_huashanpi_evb 平台

1. 编译

```bash
make clean
make SDK=sdk_chip_cv181xh_bga
```

2. 烧写

```bash
make flashall SDK=sdk_chip_cv181xh_bga
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

