# TEST_CSI2.0 SOLUTION操作介绍

**一、CDK工具操作（以pangu开发板介绍）**<br />**1.概要**<br />操作手册分为环境准备和测试两部分内容。环境准备主要介绍测试环境中所需的软硬件工具准备和安装。测试章节主要介绍程序编译及其编译前的准备工作，以及测试执行前的准备工作和执行中的具体操作步骤，明确具体的测试结果。<br />**2.环境准备**<br />**2.1硬件准备**<br />电脑×1，测试开发板×2，电源线×2，连接DebugServer数据线×2，USB转串口线×2<br />**2.2工具软件准备**<br />1.电脑安装CDK工具，CDK工具下载地址：[https://www.xrvm.cn/activities/cdk](https://www.xrvm.cn/activities/cdk)<br />![](./images_csi20/1.png)<br />![](./images_csi20/2.png)<br />**2.3工程创建**<br />2.3.1打开CDK，进入到CDK工具页面<br />2.3.2点击工具栏的Project->New IoT Project<br />![](./images_csi20/3.png)<br />2.3.3创建Workspace Name，选择保存路径，并点击OK<br />![](./images_csi20/4.png)<br />2.3.4在弹出来的“IoT Project Wizard”窗口中，选择：玄铁-》chip_pangu（这里测试的开发板是pangu,因此这里选择chip_pangu，若为其他芯片，则需要选择其他），并点击Next<br />![](./images_csi20/5.png)<br />2.3.5选择“pangu_cpu0”，并点击Next<br />![](./images_csi20/6.png)<br />2.3.6选择pangu_demo,并点击Finish<br />![](./images_csi20/7.png)<br />2.3.7等待加载完成，CDK工程就创建成功<br />![](./images_csi20/8.png)<br />**3.文件结构介绍**<br />**3.1app\include\csi\driver**<br />###csi2.0代码头文件（此处无需修改）<br />![image.png](./images_csi20/9.png)<br />**3.2app\src\csi\driver**<br />###csi2.0测试代码源文件（此处无需修改）<br />![image.png](./images_csi20/11.png)<br />**3.3driver_tests_main.c和app_main.c**<br />驱动测试入口函数（此处无需修改）<br />![image.png](./images_csi20/11.png)<br />**3.4doc目录**（此处无需修改，其中USER_GUIDE.md为test_csi2.0 solution用户操作指南）<br />![image.png](./images_csi20/12.png)<br />**3.5宏开关**

```yaml
def_config:
  CONFIG_CLI: 1
  CONFIG_DEBUG: 3
#  CONFIG_DMA: 1
  CONFIG_INIT_TASK_STACK_SIZE: 2048 
  CONFIG_ARCH_INTERRUPTSTACK: 1024
  CONFIG_TEE_CA: 1
#  CONFIG_SYSTEM_SECURE: 1
# CONFIG_KERNEL_NONE: 1
#  CONFIG_RTC: 1
#  CONFIG_UART: 1
#  CONFIG_ADC: 1
#  CONFIG_AES: 1
#  CONFIG_CODEC: 1
#  CONFIG_EFUSE: 1
#  CONFIG_GPIO: 1
#  CONFIG_I2S: 1
#  CONFIG_IIC: 1
#  CONFIG_INTNEST: 1
#  CONFIG_MBOX: 1
#  CONFIG_PWM: 1
#  CONFIG_RSA: 1
#  CONFIG_SHA: 1
#  CONFIG_SPI: 1
#  CONFIG_TICK: 1
#  CONFIG_TIMER: 1
#  CONFIG_TRNG: 1
#  CONFIG_WDT: 1
# CONFIG_SPIFLASH: 1
#  CONFIG_EFLASH: 1
```
**4.操作过程**<br />**4.1替换镜像文件**<br />（1）备注：因为加解密模块需要关闭 CONFIG_TEE_CA: 1宏，打开CONFIG_SYSTEM_SECURE: 1宏，所以需要用户手动替换PACKAGE中的pangu_cpu0(v7.4-dev)的文件<br />（2）用户根据自己的安装路径找到CDK工具中的PACK组件在自己电脑上的安装位置<br />![image.png](./images_csi20/13.png)<br />（3）将app/BOOTIMGS/newboot下的boot文件和boot.elf文件拷贝到C-Sky/CDK/CSKY/PACK/pangu_cpu0/v7.4-dev/bootimgs目录下，并且替换bootimgs中对应的两个文件，操作如图：<br />![image.png](./images_csi20/14.png)<br />![image.png](./images_csi20/15.png)<br />选择》》"替换目标中的文件"<br />（4）将app/newboot下的config.yaml拷贝到C-Sky/CDK/CSKY/PACK/pangu_cpu0/v7.4-dev/configs目录下，替换掉文件夹中的config.yaml文件<br />![image.png](./images_csi20/16.png)<br />（5）若要替换回原来的镜像，则将app/BOOTIMGS/oldboot目录下的boot、boot.elf和config.yaml文件按照上述方法替换即可，并且打开 CONFIG_TEE_CA: 1宏，关闭CONFIG_SYSTEM_SECURE: 1宏<br />**4.2编译**<br />（1）方法一：点击CDK工具中的build<br />![](./images_csi20/17.png)<br />
<br />（2）方法二：鼠标右击工程选择rebuild<br />![](./images_csi20/18.png)<br />（3）编译成功后，如图所示：<br />![](./images_csi20/19.png)<br />**4.3加载elf文件**<br />（1）打开CDK工具的命令行窗口，如图所示：<br />![image.png](./images_csi20/20.png)<br />（2）打开后如图所示：<br />![image.png](./images_csi20/21.png)<br />（3）进入到工程下的Obj文件夹：<br />![image.png](./images_csi20/22.png)<br />（4）使用CSKY工具加载pangu_demo.elf文件<br />![image.png](./images_csi20/23.png)<br />（5）加载第一块开发板<br />##这里以加载到远程服务器上的开发板为例，输入Call csky-elfabiv2-gdb ./pangu_demo.elf，然后在gdb命令中输入target remote 172.16.201.173:1045，回车确认输入并连接开发板，打开串口工具，连接<br />![image.png](./images_csi20/24.png)<br />（6）然后输入命令lo<br />（7）加载完成之后再输入c<br />![image.png](./images_csi20/25.png)<br />（8）成功之后串口打印log如图所示：<br />![image.png](./images_csi20/26.png)<br />（9）加载第二块开发板<br />##这里以加载到远程服务器上的开发板为例，重新打开一个新的命令行窗口，同样进入到Obj目录下，输入Call csky-elfabiv2-gdb ./pangu_demo.elf，然后在gdb命令输入target remote 172.16.201.173:1050，回车确认输入并连接开发板，打开串口工具，连接<br />![image.png](./images_csi20/27.png)<br />（10）然后输入命令lo<br />（11）加载完成之后再输入c<br />![image.png](./images_csi20/28.png)<br />（12）成功之后串口打印log如图所示：<br />![image.png](./images_csi20/29.png)<br />**4.4烧写**<br />（1）配置烧写路径：点击工具栏Flash-》Configure Flash Tool -》Debug<br />![image.png](./images_csi20/30.png)<br />![image.png](./images_csi20/31.png)<br />##这里以下载到远程服务器为例，选择Remote ICE-》Settings，输入IP：172.16.201.173   PORT：1050，点击OK,当第一次烧写成功以后，将程序烧写到另外一块开发板上，再重新输入ip和PORT，这里第二块板子的IP：172.16.201.173   PORT：1045<br />（2）烧写第一块开发板<br />![image.png](./images_csi20/32.png)<br />（3）点击load![image.png](./images_csi20/33.png)<br />（4）正在烧录：<br />![image.png](./images_csi20/34.png)

（5）烧录成功：<br />![image.png](./images_csi20/35.png)<br />（6）烧写第二块开发板<br />![image.png](./images_csi20/36.png)<br />（7）点击load![image.png](./images_csi20/37.png)<br />（8）正在烧录：<br />![image.png](./images_csi20/38.png)

（9）烧录成功：<br />![image.png](./images_csi20/39.png)<br />**4.5执行**<br />（1）打开串口工具，连接两块开发板<br />![image.png](./images_csi20/40.png)<br />![image.png](./images_csi20/41.png)<br />（2）两块开发板分别按下复位键，板子启动<br />![image.png](./images_csi20/42.png)<br />（3）进行模块的AT指令测试<br />**5.示例**<br />**5.1csi2.0版本的UART模块测试示例**<br />###以UART模块为例，对UART进行异步收发测试###<br />（1）将A板和B板的PB24 PB25用连接线交叉连接，如下图所示：<br />![](./images_csi20/43.png)<br />（2）打开csi2.0版本的UART宏开关，def_config设置如下：
```yaml
def_config:
  CONFIG_CLI: 1
  CONFIG_DEBUG: 3
  CONFIG_INIT_TASK_STACK_SIZE: 2048 
  CONFIG_ARCH_INTERRUPTSTACK: 1024
  CONFIG_TEE_CA: 1CONFIG_UART: 1
  CONFIG_UART: 1
```
（3）修改编译版本<br />（3-1）选择你所测试的芯片名字，右击选择Options for "chip_pangu(v7.4-dev)"(这里测试芯片是pangu_cpu0，所以在chip_pangu(v7.4-dev)上右击），如下图所示：<br />![image.png](./images_csi20/44.png)<br />（3-2）设置测试版本：这里配置pangu_cpu0为csi2.0版本，配置如下：<br />![image.png](./images_csi20/45.png)<br />备注：如果测试版本为csi1.0，则配置为：CONFIG_CSI=csi1;CONFIG_CSI_V1=1<br />（4）编译<br />（4-1）右击工程选择rebuild

![](./images_csi20/46.png)<br />（4-2）编译成功如图：<br />![](./images_csi20/47.png)<br />（5）烧写<br />（5-1）配置烧写路径：点击工具栏Flash-》Configure Flash Tool -》Debug<br />![image.png](./images_csi20/48.png)<br />![image.png](./images_csi20/49.png)<br />##这里以下载到远程服务器为例，选择Remote ICE-》Settings，输入IP：172.16.201.173   PORT：1050，点击OK,当第一次烧写成功以后，将程序烧写到另外一块开发板上，再重新输入ip和PORT，这里第二块板子的IP：172.16.201.173   PORT：1045<br />（5-2）烧写第一块开发板<br />![image.png](./images_csi20/50.png)<br />（5-3）点击load![image.png](./images_csi20/51.png)<br />（5-4）正在烧录：<br />![image.png](./images_csi20/52.png)

（5-5）烧录成功：<br />![image.png](./images_csi20/53.png)<br />（5-6）烧写第二块开发板<br />![image.png](./images_csi20/54.png)<br />（5-7）点击load![image.png](./images_csi20/55.png)<br />（5-8）烧录成功：<br />![image.png](./images_csi20/56.png)<br />（6）开发板启动<br />（6-1）打开串口工具，连接两块开发板<br />![image.png](./images_csi20/57.png)<br />![image.png](./images_csi20/58.png)<br />（6-2）两块开发板分别按下复位键，板子启动成功<br />![image.png](./images_csi20/59.png)<br />（7）AT命令测试<br />（7-1）分别使用AT+PINMUX_CONFIG指令将A板和B板对应的PB24、PB25引脚复用成UART功能引脚，成功后A板、B板打印如下log<br />![](./images_csi20/60.png)<br />（7-2）向A板发送AT+UART_ASYNC_RECEIVE=9600,2,2,0,0（UART异步接收指令：参数1：波特率，参数2：数据位个数，参数3：校验位个数，参数4：停止位个数，参数5：流控）指令，打印Ready后向B板发送指令AT+UART_ASYNC_SEND=9600,2,2,0,0UART异步发送指令：参数1：波特率，参数2：数据位个数，参数3：校验位个数，参数4：停止位个数，参数5：流控），测试完成后，A板和B板都会打印OK，即为测试成功<br />》》》向A板发送AT+UART_ASYNC_RECEIVE=9600,2,2,0,0指令后打印如下log：<br />![](./images_csi20/61.png)<br />》》》再向B板发送AT+UART_ASYNC_SEND=9600,2,2,0,0指令，打印如下log：<br />![](./images_csi20/62.png)<br />》》》测试完成后，A板和B板都会打印OK，即为测试成功,对应log如下：

![](./images_csi20/63.png)<br />**<br />**5.2csi2.0版本的RTC模块测试示例**<br />###以RTC模块为例，测试RTC模块的计时功能和报警功能###<br />（1）RTC模块无需连线，也无需进行引脚功能复用，跳过此步骤<br />（2）打开csi2.0版本的RTC宏开关，因为测试RTC需要关闭关闭 CONFIG_TEE_CA: 1，打开CONFIG_SYSTEM_SECURE: 1，所以def_config设置如下：
```yaml
def_config:
  CONFIG_CLI: 1
  CONFIG_DEBUG: 3
  CONFIG_INIT_TASK_STACK_SIZE: 2048 
  CONFIG_ARCH_INTERRUPTSTACK: 1024
  CONFIG_SYSTEM_SECURE: 1
  CONFIG_RTC: 1
```
（3）镜像替换<br />（3-1）备注：因为加解密模块需要关闭 CONFIG_TEE_CA: 1宏，所以需要用户手动替换PACKAGE中的pangu_cpu0(v7.4-dev)的文件<br />（3-2）用户根据自己的安装路径找到CDK工具中的PACK组件在自己电脑上的安装位置<br />![image.png](./images_csi20/64.png)<br />（3-3）将app/newboot下的boot文件和boot.elf文件拷贝到C-Sky/CDK/CSKY/PACK/pangu_cpu0/v7.4-dev/bootimgs目录下，并且替换bootimgs中对应的两个文件，操作如图：<br />![image.png](./images_csi20/65.png)<br />![image.png](./images_csi20/66.png)<br />选择》》"替换目标中的文件"<br />（3-4）将app/newboot下的config.yaml拷贝到C-Sky/CDK/CSKY/PACK/pangu_cpu0/v7.4-dev/configs目录下，替换掉文件夹中的config.yaml文件<br />![image.png](./images_csi20/67.png)<br />（3-5）若要替换回原来的镜像，则将app/BOOTIMGS/oldboot目录下的boot、boot.elf和config.yaml文件按照上述方法替换即可，并且打开 CONFIG_TEE_CA: 1宏，关闭CONFIG_SYSTEM_SECURE: 1宏<br />（4）修改编译版本<br />（4-1）选择你所测试的芯片名字，右击选择Options for "chip_pangu(v7.4-dev)"(这里测试芯片是pangu_cpu0，所以在chip_pangu(v7.4-dev)上右击），如下图所示：<br />![image.png](./images_csi20/68.png)<br />（4-2）设置测试版本：这里配置pangu_cpu0为csi2.0版本，配置如下：<br />![image.png](./images_csi20/69.png)<br />（5）编译并加载<br />（5-1）鼠标右击工程选择rebuild<br />![](./images_csi20/70.png)<br />（5-2）编译成功，如图所示：<br />![](./images_csi20/71.png)<br />（5-3）打开CDK工具的命令行窗口，如图所示：<br />![image.png](./images_csi20/72.png)<br />打开后如图所示：<br />![image.png](./images_csi20/73.png)<br />（5-4）进入到工程下的Obj文件夹：<br />![image.png](./images_csi20/74.png)<br />（5-5）使用CSKY工具加载pangu_demo.elf文件<br />![image.png](./images_csi20/75.png)<br />（5-6）加载elf文件到开发板<br />##这里以加载到远程服务器上的开发板为例，输入Call csky-elfabiv2-gdb ./pangu_demo.elf，然后在gdb命令中输入target remote 172.16.201.173:1045，回车确认输入并连接开发板，打开串口工具，连接<br />![image.png](./images_csi20/76.png)<br />（5-7）输入命令lo，加载完成之后再输入c<br />![image.png](./images_csi20/77.png)<br />（5-8）成功之后串口打印log如图所示：<br />![image.png](./images_csi20/78.png)<br />（6）RTC模块的功能测试<br />（6-1）RTC模块的报警功能测试：<br />发送命令AT+RTC_ALARM=0,120,11,31,23,59,59（RTC模块的报警功能测试：参数1：RTC设备号，参数2：年，参数3：月，参数4：日，参数5：小时，参数6：分钟，参数7：秒）到A板，测试完成后， A板对应串口会打印OK，出现如下log，即为测试成功：<br />![image.png](./images_csi20/79.png)<br />（6-2）RTC模块的计时功能测试：<br />发送命令AT+RTC_TIME=0,120,11,31,23,59,59（RTC模块的计时功能测试：参数1：RTC设备号，参数2：年，参数3：月，参数4：日，参数5：小时，参数6：分钟，参数7：秒）到A板，测试完成后， A板对应串口会打印OK，出现如下log，即为测试成功：<br />![image.png](./images_csi20/80.png)<br />**6.文件索引**<br />![image.png](./images_csi20/94.png)<br /><br /><br /><br /><br /><br />**二、命令行操作（以ch2601开发板介绍）**<br />**1.概要**<br />操作手册分为环境准备和测试两部分内容。环境准备主要介绍测试环境中所需的软硬件工具准备和安装。测试章节主要介绍程序编译及其编译前的准备工作，以及测试执行前的准备工作和执行中的具体操作步骤，明确具体的测试结果。<br />**2.环境准备**<br />**2.1硬件准备**<br />电脑×1，测试开发板（ch2601）×2，电源线×2，连接DebugServer数据线×2，USB转串口线×2<br />**2.2工具软件准备**<br />1）在Linux系统下安装yoctools工具，版本大于2.0.X版本，yoctools安装命令：
```yaml
sudo pip install yoctools
```
2）新建一个文件夹download，用来下载solution
```yaml
mkdir download
```
3）在此目录下初始化yoc
```yaml
yoc init -a
```
4）查看所有solution
```yaml
yoc list -r -s
```
5）下载solution
```yaml
yoc install test_csi2 -b v2.0.0#以test_csi2为例
```
下载成功出现如图所示的三个文件夹：<br />![](./images_csi20/82.png)<br />**3.示例**<br />**3.1csi2.0版本的DMA模块测试示例**<br />###以DMA模块为例，测试其功能###<br />1）修改test_csi2/package.yaml文件：进入solutions/test_csi
```yaml
ran@D-C02208X8-0358:/mnt/d/demo/solutions/test_csi2$ vi package.yaml
```
2）在package.yaml文件的build_config/include将所要测试版本的头文件和源文件包含，并且在def_config下打开所需的宏开关，如需要测试DMA模块，需要打开对应模块的宏开关，最后build_config/include、build_config/source_file和def_config设置如下：<br />build_config/include设置：（此处无需修改）
```yaml
# 编译参数
build_config:
  include:
    - app/include
    - app/include/csi/atserver
    - app/include/csi/common
    - app/include/csi/driver
```
build_config/source_file设置：（此处无需修改）
```yaml
# 源文件
source_file:
source_file:
  - app/src/app_main.c
  - app/src/driver_tests_main.c
  - app/src/init/*.c
  - app/src/csi/common/*.c
  - app/src/csi/atserver/*.c
  - app/src/csi/driver/pinmux/*.c
  - app/src/csi/driver/pin/*.c
  - app/src/csi/driver/aes/*.c ? <CONFIG_AES>
  - app/src/csi/driver/rsa/*.c ? <CONFIG_RSA>
  - app/src/csi/driver/dma/*.c ? <CONFIG_DMA>
  - app/src/csi/driver/sha/*.c ? <CONFIG_SHA>
  - app/src/csi/driver/trng/*.c ? <CONFIG_TRNG>
  - app/src/csi/driver/wdt/*.c ? <CONFIG_WDT>
  - app/src/csi/driver/gpio/*.c ? <CONFIG_GPIO>
  - app/src/csi/driver/timer/*.c ? <CONFIG_TIMER>
  - app/src/csi/driver/iic/*.c ? <CONFIG_IIC>
  - app/src/csi/driver/spi/*.c ? <CONFIG_SPI>
  - app/src/csi/driver/mbox/*.c ? <CONFIG_MBOX>
  - app/src/csi/driver/uart/*.c ? <CONFIG_UART>
  - app/src/csi/driver/pwm/*.c ? <CONFIG_PWM>
  - app/src/csi/driver/tick/*.c ? <CONFIG_TICK>
  - app/src/csi/driver/i2s/*.c ? <CONFIG_I2S>
  - app/src/csi/driver/codec/*.c ? <CONFIG_CODEC>
  - app/src/csi/driver/intnest/*.c ? <CONFIG_INTNEST>
  - app/src/csi/driver/mbox/*.c ? <CONFIG_MBOX>
  - app/src/csi/driver/efuse/*.c ? <CONFIG_EFUSE>
  - app/src/csi/driver/adc/*.c ? <CONFIG_ADC>
  - app/src/csi/driver/spiflash/*.c ? <CONFIG_SPIFLASH>
```
depends设置:
```yaml
depends:
  - sdk_chip_ch2601: v7.4.y
```

def_config设置：
```yaml
def_config:
  CONFIG_ARCH_INTERRUPTSTACK: 1024
  CONFIG_CLI: 1
  CONFIG_CPU_E906: 1
  CONFIG_CSI_V2: 1
  CONFIG_DEBUG: 3
  CONFIG_INIT_TASK_STACK_SIZE: 2048
  CONFIG_SUPPORT_TSPEND: 1
  CONFIG_XIP: 1
#  CONFIG_TEE_CA: 1
#  CONFIG_SYSTEM_SECURE: 1
#  CONFIG_NUTTXMM_NONE: 1
#  CONFIG_KERNEL_NONE: 1
  CONFIG_DMA: 1
# CONFIG_AES: 1
# CONFIG_CRC: 1
# CONFIG_UART: 1
# CONFIG_ADC: 1
# CONFIG_CODEC: 1
# CONFIG_EFUSE: 1
# CONFIG_GPIO: 1
# CONFIG_I2S: 1
# CONFIG_IIC: 1
# CONFIG_INTNEST: 1
# CONFIG_MBOX: 1
# CONFIG_PWM: 1
# CONFIG_RSA: 1
# CONFIG_SHA: 1
# CONFIG_SPI: 1
# CONFIG_TICK: 1
# CONFIG_TIMER: 1
# CONFIG_TRNG: 1
# CONFIG_WDT: 1
# CONFIG_SPIFLASH: 1
```

3）编译<br />》》》进入solutions/test_csi2/目录，执行make命令
```yaml
ran@D-C02208X8-0358:/mnt/d/demo/solutions/test_csi2$ make clean;make
```
4）编译成功如图所示：<br />![image.png](./images_csi20/83.png)<br />6）加载<br />》》》新建.gdbinit文件，内容如下：<br />备注：因测试用开发板位于远程，IP地址为172.16.201.173，PORT为1050，所以设置如下：

```yaml
target remote 172.16.201.173:1050
set $pc=Reset_Handler
```
》》》加载elf文件到对应开发板中，执行命令如下:<br />![image.png](./images_csi20/84.png)<br />7）打开串口<br />![image.png](./images_csi20/85.png)<br />开发板启动成功，打印如下log<br />![image.png](./images_csi20/86.png)<br />8）执行AT指令<br />（1）DMA功能测试：<br />发送命令AT+DMA_MEMORY_TO_MEMORY=0,0,2,0,1,100,4096 （DMA内存传输功能测试：参数1：设备号，参数2：通道号，参数3：数据宽度，参数4：源地址变化类型，目的地址变化类型，参数5：传输数据时，硬件的一组是多少字节，参数6：传输长度）到A板，测试完成后， A板对应串口会打印OK，出现如下log，即为测试成功：<br />![](./images_csi20/87.png)<br />（2）DMA模块的通道申请和释放功能测试：<br />发送命令AT+DMA_CHANNEL_ALLOC_FREE=0,0（DMA模块的通道申请和释放功能测试：参数1：设备号，参数2：通道号）到A板，测试完成后， A板对应串口会打印OK，出现如下log，即为测试成功：<br />![](./images_csi20/88.png)<br />
<br />4.文件索引<br />![image.png](./images_csi20/94.png)

**三、结果**

用户自测之后，将测试结果按下图格式填入doc/example目录下的word文档中：

![image.png](./images_csi20/95.png)
