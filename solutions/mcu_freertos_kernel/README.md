# 概述

`mcu_freertos_kernel` 是一个玄铁RTOS SDK中面向mcu领域的demo。该demo基于freertos原生接口实现，可运行在QEMU环境。这个demo提供了freertos环境下事件(event)、消息队列(message queue)、互斥量(mutex)、信号量(semaphore)、任务(task)、时间(time)和定时器(timer)的使用范例。

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        e902 e902m e902t e902mt e906 e906f e906fd e906p e906fp e906fdp e907 e907f e907fd e907p e907fp e907fdp <br />
        r910 r920 r908 r908fd r908fdv r908-cp r908fd-cp r908fdv-cp <br />
        c906 c906fd c906fdv c908 c908v c908i c910v2 c910v3 c910v3-cp c920v2 c920v3 c920v3-cp <br />
        c907 c907fd c907fdv c907fdvm c907-rv32 c907fd-rv32 c907fdv-rv32 c907fdvm-rv32
- platform: <br />
        smartl xiaohui

以玄铁`c907fdvm`类型为例，可以使用如下命令编译:
```bash
./do_build.sh c907fdvm xiaohui
```

## 运行

示例可以基于玄铁QEMU或FPGA平台运行。

### 基于玄铁QEMU运行

```bash
qemu-system-riscv64 -machine xiaohui -nographic -kernel yoc.elf -cpu c907fdvm
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果

串口看到以下内容输出，表示运行成功：

```
FreeRTOS version:V10.4.3 LTS Patch 3
-------------event_example_main start-------------
example_k_event writes event .
Example_Event waits event 0x1001 
Example_Event,reads event :0x1001
test kernel event successfully!
-------------event_example_main finish-------------

-------------message_example_main start-------------
send_Entry
send_Entry:number of queued mesages : 1
recv_Entry
send_Entry:number of queued mesages : 2
send_Entry:number of queued mesages : 3
send_Entry:number of queued mesages : 4
send_Entry:number of queued mesages : 5
recv message:test is message 0
recv_Entry:number of queued mesages : 4
recv message:test is message 1
recv_Entry:number of queued mesages : 3
recv message:test is message 2
recv_Entry:number of queued mesages : 2
recv message:test is message 3
recv_Entry:number of queued mesages : 1
recv message:test is message 4
recv_Entry:number of queued mesages : 0
message queue recv finish
delete the queue successfully!
-------------message_example_main finish-------------

-------------mutex_example_main start-------------
task2 try to get  mutex, wait forever.
task2 get mutex g_Testmux01 and suspend 100 ms.
task1 try to get  mutex, wait 100ms.
task2 resumed and post the g_Testmux01
task1 get mutex g_Testmux01.
test kernel mutex successfully!
-------------mutex_example_main finish-------------

-------------sem_example_main start-------------
Example_SemTask1 try get sem g_usSem ,timeout 100ms.
Example_SemTask1 post sem g_usSem .
Example_SemTask2 try get sem g_usSem wait forever.
Example_SemTask2 get sem g_usSem and then delay 200ms .
Example_SemTask2 post sem g_usSem .
test kernel semaphore successfully!
-------------sem_example_main finish-------------

-------------task_example_main start-------------
Fail to delete example task itself. 
Enter TaskHi Handler.
Example_TaskLo:Enter TaskLo Handler.
Example_TaskHi:TaskHi vTaskDelay Done and suspend self.
Example_TaskLo:resume Example_TaskHi
Example_TaskHi:test kernel task successfully!
-------------task_example_main finish-------------

-------------time_example_main start-------------
kernel systick is 10 ms
now time = 3500 ms before 19s....
print cnt every 1s for 10 times
-----9
-----8
-----7
-----6
-----5
-----4
-----3
-----2
-----1
-----0
print cnt every 3s for 3 times
-----2
-----1
-----0
now time = 22500 ms after 19s, will be delay  1s....
now time = 23500 ms after delay 1s 
test kernel time successfully!
-------------time_example_main finish-------------

-------------timer_example_main start-------------
start to create timer
g_timercount1=1
tick_last1 = 2450
g_timercount1=2
tick_last1 = 2550
start Timer2
g_timercount2=1
tick_last2 = 2610
g_timercount2=2
tick_last2 = 2620
g_timercount2=3
tick_last2 = 2630
g_timercount2=4
tick_last2 = 2640
g_timercount2=5
tick_last2 = 2650
g_timercount2=6
tick_last2 = 2660
g_timercount2=7
tick_last2 = 2670
g_timercount2=8
tick_last2 = 2680
g_timercount2=9
tick_last2 = 2690
g_timercount2=10
tick_last2 = 2700
test kernel timer successfully!
-------------timer_example_main finish-------------

kernel all examples finish.
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁RTOS SDK用户手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁RTOS SDK用户手册》

2、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁RTOS SDK用户手册》

2、如果出现无法编译情况，请使用`sudo pip install yoctools -U`更新最新版本的yoctools再做尝试。

3、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

4、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。