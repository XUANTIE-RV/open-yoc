# 概述

`mcu_rtthread_kernel`   是一个玄铁RTOS SDK中面向mcu领域的demo，该demo基于rtthread原生接口实现，可运行在QEMU环境。这个demo提供了rtthread中事件(event)、消息队列(message queue)、互斥量(mutex)、信号量(semaphore)、任务(task)、时间(time)和定时器(timer)的使用范例。

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

 \ | /
- RT -     Thread Operating System
 / | \     5.1.0 build Dec 19 2024 10:31:54
 2006 - 2024 Copyright by RT-Thread team
-------------event_example_main start-------------
thread2: send event3
thread1: OR recv event 0x8
thread1: delay 1s to prepare the second event
thread2: send event5
thread2: send event3
thread2 leave.
-------------event_example_main finish-------------

-------------message_example_main start-------------
thread2: send message - A
thread1: recv msg from msg queue, the content:A
thread2: send message - B
thread2: send message - C
thread2: send message - D
thread2: send message - E
thread1: recv msg from msg queue, the content:B
thread2: send message - F
thread2: send message - G
thread2: send message - H
thread2: send urgent message - I
thread2: send message - J
thread1: recv msg from msg queue, the content:I
thread2: send message - K
thread2: send message - L
thread2: send message - M
thread2: send message - N
thread2: send message - O
thread1: recv msg from msg queue, the content:C
thread2: send message - P
thread2: send message - Q
thread2: send message - R
thread2: send message - S
thread2: send message - T
thread1: recv msg from msg queue, the content:D
message queue stop send, thread2 quit
thread1: recv msg from msg queue, the content:E
thread1: recv msg from msg queue, the content:F
thread1: recv msg from msg queue, the content:G
thread1: recv msg from msg queue, the content:H
thread1: recv msg from msg queue, the content:J
thread1: AND recv event 0x28
thread1 leave.
thread1: recv msg from msg queue, the content:K
-------------message_example_main finish-------------

-------------mutex_example_main start-------------
rt_thread_entry2 mutex protect ,number1 = mumber2 is 1
rt_thread_entry1 mutex protect ,number1 = mumber2 is 2
rt_thread_entry2 mutex protect ,number1 = mumber2 is 3
rt_thread_entry1 mutex protect ,number1 = mumber2 is 4
rt_thread_entry2 mutex protect ,number1 = mumber2 is 5
rt_thread_entry1 mutex protect ,number1 = mumber2 is 6
thread1: recv msg from msg queue, the content:L
rt_thread_entry2 mutex protect ,number1 = mumber2 is 7
rt_thread_entry1 mutex protect ,number1 = mumber2 is 8
rt_thread_entry2 mutex protect ,number1 = mumber2 is 9
rt_thread_entry1 mutex protect ,number1 = mumber2 is 10
rt_thread_entry2 exit
thread1: recv msg from msg queue, the content:M
rt_thread_entry1 mutex protect ,number1 = mumber2 is 11
rt_thread_entry1 mutex protect ,number1 = mumber2 is 12
rt_thread_entry1 mutex protect ,number1 = mumber2 is 13
rt_thread_entry1 mutex protect ,number1 = mumber2 is 14
rt_thread_entry1 mutex protect ,number1 = mumber2 is 15
thread1: recv msg from msg queue, the content:N
rt_thread_entry1 mutex protect ,number1 = mumber2 is 16
rt_thread_entry1 mutex protect ,number1 = mumber2 is 17
rt_thread_entry1 mutex protect ,number1 = mumber2 is 18
rt_thread_entry1 mutex protect ,number1 = mumber2 is 19
rt_thread_entry1 mutex protect ,number1 = mumber2 is 20
rt_thread_entry1 exit
thread1: recv msg from msg queue, the content:O
thread1: recv msg from msg queue, the content:P
thread1: recv msg from msg queue, the content:Q
thread1: recv msg from msg queue, the content:R
thread1: recv msg from msg queue, the content:S
thread1: recv msg from msg queue, the content:T
thread1: detach mq 
-------------mutex_example_main finish-------------

-------------sem_example_main start-------------
create done. dynamic semaphore value = 0.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
thread1 release a dynamic semaphore.
rt_thread1_entry exit
thread2 take a dynamic semaphore. number = 1
thread2 take a dynamic semaphore. number = 2
thread2 take a dynamic semaphore. number = 3
thread2 take a dynamic semaphore. number = 4
thread2 take a dynamic semaphore. number = 5
thread2 take a dynamic semaphore. number = 6
thread2 take a dynamic semaphore. number = 7
thread2 take a dynamic semaphore. number = 8
thread2 take a dynamic semaphore. number = 9
thread2 take a dynamic semaphore. number = 10
rt_thread2_entry exit
-------------sem_example_main finish-------------

-------------task_example_main start-------------
thread1 count: 0
thread2 count: 0
thread2 count: 1
thread2 count: 2
thread2 count: 3
thread2 count: 4
thread2 count: 5
thread2 count: 6
thread2 count: 7
thread2 count: 8
thread2 count: 9
thread2 exit
thread1 count: 1
thread1 count: 2
thread1 count: 3
thread1 count: 4
thread1 exit
-------------task_example_main finish-------------

-------------time_example_main start-------------
kernel systick is 10 ms
now time = 2510 ms before 19s....
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
now time = 21510 ms after 19s, will be delay  1s....
now time = 22510 ms after delay 1s 
test kernel time successfully!
-------------time_example_main finish-------------

-------------timer_example_main start-------------
periodic timer is timeout 0
periodic timer is timeout 1
one shot timer is timeout
periodic timer is timeout 2
periodic timer is timeout 3
periodic timer is timeout 4
periodic timer is timeout 5
periodic timer is timeout 6
periodic timer is timeout 7
periodic timer is timeout 8
periodic timer is timeout 9
periodic timer was stopped! 
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
