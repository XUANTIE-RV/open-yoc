# 概述
bl606p_c906_ipc_demo是核间通信ipc在c906侧的demo，主要完成了c906侧作为ipc服务端、客户端、输出和ipc的cli的功能。

# 使用

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install bl606p_c906_ipc_demo
```

### 编译&安装

注意：
    该demo只适用于bl606p开发板。

1. 编译

   ```bash
   make clean
   make
   ```
编译完成之后会在当前目录下生成`yoc.bin`文件。

2. 安装

   ```
   make install
   ```
将yoc.bin转换成.h文件，并拷贝至ipc_demo相对应的board组件下供集成。


# 运行

此工程编译出来的bin文件并不单独运行，而是集成到ipc_demo里面，由ipc_demo来运行。具体请参考ipc_demo的README.md文件。

## 作为客户端

作为客户端进行数据发送

收到类似打印，表示数据发送成功

```
[C906]<app_task>ipc async done
```

### 作为服务端

作为服务端接收数据

收到类似打印，表示数据接收完成

```
[C906]<app_task>ipc service test start
[C906]<ser10->1>music recv ok, msg->req_data = hello world
```

## cli功能

e907侧发送指定的cli命令之后，c906侧收到类似打印，表示cli命令执行成功

```
[C906]<ipc_cli>ipc cli task, cli_data = cli
```

## 输出功能

package.yaml添加CONFIG_USE_LOG_IPC和CONFIG_HAL_UART_DISABLED宏，即可实现输出功能。具体的表现为c906侧的log在ipc_demo中输出。