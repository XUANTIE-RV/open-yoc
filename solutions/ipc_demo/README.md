# 概述
ipc_demo是核间通信ipc在e907侧的demo，主要实现了e907作为ipc的服务端、客户端、输出和ipc的cli的功能。

# 使用
## CDK
在CDK的首页，通过搜索ipc_demo，可以找到ipc_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://www.xrvm.cn/document?temp=use-cdk-to-get-started-quickly&slug=yocbook) 章节

## 通过命令行
需要先安装[yoctools](https://www.xrvm.cn/document?temp=yoctools&slug=yocbook)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install ipc_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的 XuanTie Debug Server 中显示的对应的内容。

ipc_demo只适用于bl606p的e907

1. 编译

   ```bash
   // 在c906侧执行过make clean && make && make install之后执行以下命令
   make clean
   make
   ```

2. 烧录

   ```bash
   make flashall
   ```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后按下复位按键，串口会有打印输出。

## 作为服务端

作为服务端接收数据

收到类似打印，表示数据接收完成

```
[E907]<app_task>ipc service test start
[E907]<ser20->0>music recv ok, music = hello world
```

## 作为客户端

作为客户端发送数据

收到类似打印，表示数据发送成功

```
[E907]<app_task>ipc async done
```

## cli功能

当收到类似打印时，表示开始运行cli功能

```
[E907]<app_task>internuclear test start
```

输入cli命令

```cli
ipcdemo ipc_log
```

## 输出功能

907侧能显示c906侧的log