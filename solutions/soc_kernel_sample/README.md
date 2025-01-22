# 概述

`soc_kernel_sample` 是一个玄铁RTOS SDK中面向soc领域(使用osal封装)的示例，可运行在玄铁QEMU环境。该demo提供了基于osal接口下事件(event)、消息队列(message queue)、互斥量(mutex)、信号量(semaphore)、任务(task)、时间(time)和定时器(timer)的使用范例。

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform> <kernel>
```
- cpu: <br />
        e902 e902m e902t e902mt e906 e906f e906fd e906p e906fp e906fdp e907 e907f e907fd e907p e907fp e907fdp <br />
        r910 r920 r908 r908fd r908fdv r908-cp r908fd-cp r908fdv-cp <br />
        c906 c906fd c906fdv c908 c908v c908i c910v2 c910v3 c910v3-cp c920v2 c920v3 c920v3-cp <br />
        c907 c907fd c907fdv c907fdvm c907-rv32 c907fd-rv32 c907fdv-rv32 c907fdvm-rv32
- platform: <br />
        smartl xiaohui
- kernel: <br />
        freertos rtthread

以玄铁`c907fdvm`类型为例，可以使用如下命令编译:
```bash
./do_build.sh c907fdvm xiaohui rtthread
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
​
```cli
###Welcome to YoC, based XuanTie c907fdvm###
[Dec 19 2024,10:45:21]
[   0.000]<I>[INIT]<app_task>Build:Dec 19 2024,10:45:21
[   0.010]<I>[app]<app_task>kernel version : RT-Thread: 50100

-------------event_example_main start-------------
[   0.010]<I>[soc_event_test]<app_task>example_k_event writes event .
[   0.010]<I>[soc_event_test]<example_event_task>Example_Event waits event 0x1001
[   0.010]<I>[soc_event_test]<example_event_task>reads event :0x1001.
[   0.010]<I>[soc_event_test]<example_event_task>test kernel event successfully!
(cli-uart)# -------------event_example_main finish-------------

-------------message_example_main start-------------
[   0.510]<I>[soc_message_q]<recv_task>send_entry start .
[   0.510]<I>[soc_message_q]<send_task>send_entry start .
[   0.510]<I>[soc_message_q]<send_task>send_entry:number of queued mesages : 1 .
[   0.520]<I>[soc_message_q]<send_task>send_entry:number of queued mesages : 2 .
[   0.530]<I>[soc_message_q]<send_task>send_entry:number of queued mesages : 3 .
[   0.540]<I>[soc_message_q]<send_task>send_entry:number of queued mesages : 4 .
[   0.550]<I>[soc_message_q]<send_task>send_entry:number of queued mesages : 5 .
[   0.610]<I>[soc_message_q]<recv_task>recv message:test is message 0, recv message size:64

[   0.610]<I>[soc_message_q]<recv_task>number of queued mesages : 4 .
[   0.620]<I>[soc_message_q]<recv_task>recv message:test is message 1, recv message size:64

[   0.620]<I>[soc_message_q]<recv_task>number of queued mesages : 3 .
[   0.630]<I>[soc_message_q]<recv_task>recv message:test is message 2, recv message size:64

[   0.630]<I>[soc_message_q]<recv_task>number of queued mesages : 2 .
[   0.640]<I>[soc_message_q]<recv_task>recv message:test is message 3, recv message size:64

[   0.640]<I>[soc_message_q]<recv_task>number of queued mesages : 1 .
[   0.650]<I>[soc_message_q]<recv_task>recv message:test is message 4, recv message size:64

[   0.650]<I>[soc_message_q]<recv_task>number of queued mesages : 0 .
[   0.760]<I>[soc_message_q]<recv_task>queue recv finish .
-------------message_example_main finish-------------

-------------mutex_example_main start-------------
[   1.010]<I>[soc_mutex_test]<mutex_task1>mutex_task2 lock
[   1.010]<I>[soc_mutex_test]<mutex_task1>delay 200ms
[   1.010]<I>[soc_mutex_test]<mutex_task1>delay 10ms
[   1.020]<I>[soc_mutex_test]<mutex_task1>task1 lock
[   1.120]<I>[soc_mutex_test]<mutex_task1>timeout and try to get  mutex, wait forever
[   1.210]<I>[soc_mutex_test]<mutex_task1>mutex_task2 unlock
[   1.210]<I>[soc_mutex_test]<mutex_task1>task1 unlock
-------------mutex_example_main finish-------------

-------------sem_example_main start-------------
[   1.910]<I>[soc_sem_test]<task_hi task>task_hi_task start,get sem wait forever
[   1.910]<I>[soc_sem_test]<task_hi task>sem wait
[   1.910]<I>[soc_sem_test]<task_hi task>will delay 200ms
[   1.910]<I>[soc_sem_test]<task_lo task>task_lo_task start, try to wait 100ms
[   2.020]<I>[soc_sem_test]<task_lo task>aos_sem_wait timeout, will wait forever
[   2.110]<I>[soc_sem_test]<task_hi task>sem signal
[   2.110]<I>[soc_sem_test]<task_lo task>sem signal
-------------sem_example_main finish-------------

-------------task_example_main start-------------
[   2.710]<I>[soc_task_test]<task_hi task>task_hi_task start
[   2.710]<I>[soc_task_test]<task_lo task>task_lo_task start
[   2.720]<I>[soc_task_test]<task_hi task>task_hi_task delay end,start to suspend self
[   2.760]<I>[soc_task_test]<task_hi task>suspend self successful
[   2.760]<I>[soc_task_test]<task_lo task>resume  task_hi_task success
-------------task_example_main finish-------------

-------------time_example_main start-------------
kernel systick is 10 ms
now time = 3210 ms before 19s....
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
now time = 22210 ms after 19s, will be delay  1s....
now time = 23210 ms after delay 1s 
test kernel time successfully!
-------------time_example_main finish-------------

-------------timer_example_main start-------------
[  23.710]<I>[soc_timer_test]<app_task>timer1 start
[  23.710]<I>[soc_timer_test]<app_task>delay 1000 ms
[  24.710]<I>[soc_timer_test]<app_task>timer1 stop
[  24.710]<I>[soc_timer_test]<app_task>timer1 call success
[  24.710]<I>[soc_timer_test]<app_task>timer2 start
[  24.710]<I>[soc_timer_test]<app_task>delay 1010 ms
[  25.720]<I>[soc_timer_test]<app_task>timer2 stop
[  25.720]<I>[soc_timer_test]<app_task>every 100ms call once
[  25.720]<I>[soc_timer_test]<app_task>the 0 trigger time of the timer is  24810 ms
[  25.720]<I>[soc_timer_test]<app_task>the 1 trigger time of the timer is  24910 ms
[  25.720]<I>[soc_timer_test]<app_task>the 2 trigger time of the timer is  25010 ms
[  25.720]<I>[soc_timer_test]<app_task>the 3 trigger time of the timer is  25110 ms
[  25.720]<I>[soc_timer_test]<app_task>the 4 trigger time of the timer is  25210 ms
[  25.720]<I>[soc_timer_test]<app_task>the 5 trigger time of the timer is  25310 ms
[  25.720]<I>[soc_timer_test]<app_task>the 6 trigger time of the timer is  25410 ms
[  25.720]<I>[soc_timer_test]<app_task>the 7 trigger time of the timer is  25510 ms
[  25.720]<I>[soc_timer_test]<app_task>the 8 trigger time of the timer is  25610 ms
[  25.720]<I>[soc_timer_test]<app_task>the 9 trigger time of the timer is  25710 ms
[  25.720]<I>[soc_timer_test]<app_task>timer test success
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

