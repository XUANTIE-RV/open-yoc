



# WIFI_PROV测试操作手册

## 1. 概要

操作手册分为环境准备和测试NETMGR两部分内容。环境准备主要介绍测试环境中所需的软硬件工具准备和安装。测试章节主要介绍程序编译及其编译前的准备工作，以及测试执行前的准备工作和执行中的具体操作步骤，明确具体的测试结果。

本操作文档以W800为例。



## 2. 环境准备

### 2.1硬件准备

| 设备   | 数量 | 功用                     |
| ------ | ---- | ------------------------ |
| PC     | 1    | 测试人员代码编写以及运行 |
| 开发板 | 1    | 测试对象                 |
| CKLink | 1    | 烧写工具                 |
| 串口线 | 2    | 烧写及查看log日志        |



### 2.2工具软件准备

| 软件名称        | 版本     | 功用   | 下载链接                                                     |
| --------------- | -------- | ------ | ------------------------------------------------------------ |
| 工具链          | V3.10.21 | 编译   | https://occ.t-head.cn/community/download?id=3782044712072974336 |
| CskyDebugServer | V5.10.4  | 调试用 | https://occ.t-head.cn/community/download_detail?id=616215132330000384 |

​    在Ubuntu子系统中（下列操作除了强调Windows客户端以外，其他操作都在Ubuntu子系统中完成），将工具链加入到环境变量中，例如csky-elfabiv2-tools-x86_64-minilibc-20200502.tar.gz解压后存放在自定义目录下，可在 该目录下找到 csky-elfabiv2-tools-x86_64-minilibc-20200502文件夹，文件夹内有 bin 文件夹， 将 bin 文件夹路径加入到环境变量。

将该路径加入到环境变量，执行命令： 

```yaml
export PATH=$PATH: /mnt/d/tools/csky-abiv2-elf-gcc/bin
```

将上述命令的路径替换为工具链实际所在目录路径即可。



### 2.3 开发板连接CskyDebugServer 

使用时，将串口和仿真接口通过数据线连接到电脑 USB 接口。在 windows 桌面双击打开 CskyDebugServer，连接开发板，连接成功会显示以下内容： 

![](debug_server.png)



## 3. 测试

**以W800为例。**

下载yoc代码。目录如下

```shell
mkdir temp
cd temp
sudo pip install yoctools
yoc init git@gitlab.alibaba-inc.com:thead_test_solutions/manifest.git
yoc install test_wifi_provisioning
cd solutions/test_wifi_provisioning
```

测试代码目录

 ![](code_tree.png)

### 3.1 package.yaml修改

**首先需要修改测试代码目录下的package.yaml。主要需要修改以下3处。**



**1、修改依赖board。**

 将solution字段的board_name改为实际测试的board组件。

```c
solution:
  board_name: board_w800                  //改为实际测试的board组件
```



**2、修改依赖组件。**

 将depends字段的board_w800改为实际测试的board组件，再添加实际测试需要依赖的组件。

```yaml
depends:                           #可在该字段下添加实际测试需要依赖的组件
  - minilibc: v7.4.0
  - console: v7.4.0
  - drivers: v7.4.0
  - board_w800: v7.4.0            #改为实际测试的board组件
  - wifi_provisioning: v7.4.0                          
```

**3、修改board初始化代码**

修改测试代码目录下app/src/init/init.c

- 修改串口端口 （w800使用uart0作为log输出口）

```c
//配置测试用的串口端号
#define CONSOLE_IDX 0                 //实际所用的uart的idx值
```

- 修改测试所用的驱动

```c
void board_yoc_init()
{
    board_init();
    event_service_init(NULL);
    uart_csky_register(CONSOLE_IDX);
    flash_csky_register(0);

    console_init(CONSOLE_IDX, 115200, 512);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");
    wifi_w800_register(NULL);  //修改为实际所用驱动

    utask_new("at&cli", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    board_cli_init();
}
```

### 3.3 程序编译及烧写

**1、编译。**

测试目录下执行make命令编译程序：

```shell
make clean;make
```

编译成功后会在目录下的generated内生成images.zip。



**2、烧写。**

烧录之前我们有个准备工作需要做一下：

测试目录下创建一个.gdbinit，文件中放入如下内容:

```shell
target remote 172.31.0.196:1025
```

.gdbinit文件中的ip 地址从 CskyDebugServer 打印中获取, 连接 CskyDebugServer 步骤详情请见2.3节 。

测试目录下执行烧写命令：

```shell
make flashall
```

### 3.4 测试执行

**1、烧录测试代码后，重启板子，看到打印Welcome to CLI...，说明已进入测试状态。**

```shell
Welcome to CLI...
> 
```



**2、发送指令进行测试**

- API测试：

通过串口软件发送api测试命令：

```shell
test wifi_provising_api
```

串口终端会打印测试数据。

 ![](api_case_result.png)

- function测试：

通过串口软件发送function测试命令(具体步骤参考《WIFI_PROV测试用例集.xlsx》wifi_prov_function sheet)：

```shell
wifi_provising register
wifi_provising method_id
wifi_provising start
wifi_provising stop
```

输入`wifi_provising start`后，可以使用手机，打开WiFi，搜索类似于 YOC[xx:xx:xx:xx:xx:xx]的AP，然后点击连接，

 <img src="find_ap.png" style="zoom: 33%;" />

输入一个可以使用的wifi ssid和wifi psk，会出现`wifi pair got passwd...`

 ![](wifi_prov_start.png)