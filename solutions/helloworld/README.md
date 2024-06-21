# 概述
helloworld是最小系统Helloworld例程，其完成了AliOS Things的内核移植，最后在一个任务是实现周期性打印Helloworld的字符串。

# 使用
## CDK
在CDK的首页，通过搜索helloworld，可以找到helloworld，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://www.xrvm.cn/document?temp=use-cdk-to-get-started-quickly&slug=yocbook) 章节

## 通过命令行
需要先安装[yoctools](https://www.xrvm.cn/document?temp=yoctools&slug=yocbook)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install helloworld
```
### 切换系统内核

#### 切换到RHINO

默认支持RHINO 无需切换,如果需要从RT-Thread修改到RHINO，则根据下一节内容`切换到RT-Thread`下面的内容修改回去。

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
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`ble_host`,`freertos`最终如下所示:
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
#### 切换到FreeRTOS

切换到FreeRTOS需要修改的内容与`切换到RT-Thread`这节的内容相似，只需`切换到RT-Thread`这个小节这个基础上，将对应的组件`freertos`注释打开，然后注释`rtthread`这个组件即可。


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
烧录完成之后系统会自动启动，串口会有打印输出。
