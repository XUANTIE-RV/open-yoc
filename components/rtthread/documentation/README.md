# RT-Thread配置文件说明

## rtconfig.h简介

RT-Thread的内核功能与组件使能的配置文件。

## rtconfig.h文件路径

YOC平台下的rtthread组件配置文件rtconfig.h相对路径如下：

```scheme
boards/d1_dockpro_evb/include/rtconfig.h
```

## rtconfig.h配置选项

使用RT-Thread组件，不使用相关组件时只需关注基础配置即可，组件配置按需修改。

### <font color=Red>基础配置</font>

### 线程配置选项：

- `ENABLE_FPU`：此指令可能用于启用系统中的浮点单元（FPU）支持。
- `ENABLE_VECTOR`：用于启用向量操作。
- `ARCH_CPU_64BIT`：此指令表示系统运行在64位体系结构上。
- `RT_NAME_MAX`：此指令定义了 RT-Thread 内核中名称的最大长度。
- `RT_ALIGN_SIZE`：此指令指定了在 RT-Thread 内核中进行内存分配时的对齐大小。
- `RT_THREAD_PRIORITY_64`：此指令表示系统支持64个线程优先级级别。
- `RT_THREAD_PRIORITY_MAX`：此指令定义了系统中的最大线程优先级级别。
- `RT_TICK_PER_SECOND`：此指令设置了 RT-Thread 内核每秒的时钟节拍数。
- `RT_USING_OVERFLOW_CHECK`：此指令启用了系统中的溢出检查。
- `RT_USING_HOOK`：此指令启用了系统中的钩子功能。
- `RT_HOOK_USING_FUNC_PTR`：此指令表示钩子使用函数指针的方式实现。
- `RT_USING_IDLE_HOOK`：此指令启用了系统中的空闲钩子。
- `RT_IDLE_HOOK_LIST_SIZE`：此指令定义了空闲钩子列表的大小。
- `IDLE_THREAD_STACK_SIZE`：此指令设置了空闲线程的堆栈大小。
- `RT_USING_TIMER_SOFT`：此指令启用了系统中的软件定时器。
- `RT_TIMER_THREAD_PRIO`：此指令设置了定时器线程的优先级。
- `RT_TIMER_THREAD_STACK_SIZE`：此指令设置了定时器线程的堆栈大小。
- `RT_MAIN_THREAD_STACK_SIZE`：此指令设置了主线程的堆栈大小。

### 调试配置选项：

- `RT_DEBUG`：此指令用于启用系统中的调试功能。
- `RT_DEBUG_COLOR`：此指令可能用于启用调试信息的彩色输出。
- `RT_DEBUG_SCHEDULER`：此指令设置为 0 时，表示禁用调度器的调试功能。
- `RT_DEBUG_THREAD`：此指令设置为 0 时，表示禁用线程的调试功能。
- `RT_DEBUG_TIMER`：此指令设置为 0 时，表示禁用定时器的调试功能。
- `RT_DEBUG_IRQ`：此指令设置为 0 时，表示禁用中断的调试功能。
- `RT_DEBUG_INIT`：此指令设置为 1 时，表示启用初始化的调试功能。

### 线程IPC配置选项：

- `RT_USING_SEMAPHORE`：信号量，用于线程间的同步和互斥。
- `RT_USING_MUTEX`：互斥锁，用于线程间的互斥访问共享资源。
- `RT_USING_EVENT`：事件，用于线程间的事件通知和等待。
- `RT_USING_MAILBOX`：邮箱，用于线程间的消息传递。
- `RT_USING_MESSAGEQUEUE`：消息队列，用于线程间的消息传递和同步。
- `RT_USING_THREAD_SEM`：线程信号量，用于线程间的同步和互斥。

### 工作队列

- `RT_USING_DEVICE_IPC`：IPC（进程间通信）功能。它允许不同的进程或线程通过设备进行通信和共享资源。
- `RT_UNAMED_PIPE_NUMBER`：可以创建的无名管道的数量。无名管道是一种 IPC 形式，允许相关进程或线程之间进行通信。
- `RT_USING_SYSTEM_WORKQUEUE`：工作队列功能。系统工作队列提供了一种在后台异步执行任务的机制。
- `RT_SYSTEM_WORKQUEUE_STACKSIZE`：工作队列线程的堆栈大小。它指定了为系统工作队列中的任务执行分配的内存量。
- `RT_SYSTEM_WORKQUEUE_PRIORITY`：工作队列线程的优先级。它确定了系统工作队列中的任务相对于其他线程或进程的执行顺序。

### <font color=Red>组件配置</font>

#### finsh组件配置选项：

- `RT_USING_COMPONENTS_INIT`：此指令用于启用 RT-Thread 组件初始化功能，用于在系统启动时初始化各个组件。
- `RT_USING_MSH`：此指令用于启用 RT-Thread 的命令行 shell（MSH），提供了一个交互式的命令行界面。
- `FINSH_USING_MSH`：此指令用于启用 FinSH 命令行 shell，它是 RT-Thread 中的一个命令行解释器。
- `FINSH_THREAD_NAME`：此指令定义了 FinSH 命令行 shell 线程的名称。
- `FINSH_THREAD_PRIORITY`：此指令定义了 FinSH 命令行 shell 线程的优先级。
- `FINSH_THREAD_STACK_SIZE`：此指令定义了 FinSH 命令行 shell 线程的堆栈大小。
- `FINSH_USING_HISTORY`：此指令用于启用 FinSH 命令行 shell 的历史记录功能，可以回溯之前输入的命令。
- `FINSH_HISTORY_LINES`：此指令定义了 FinSH 命令行 shell 历史记录的行数。
- `FINSH_USING_SYMTAB`：此指令用于启用 FinSH 命令行 shell 的符号表功能，可以查看和修改变量和函数。
- `FINSH_CMD_SIZE`：此指令定义了 FinSH 命令行 shell 单个命令的最大长度。
- `MSH_USING_BUILT_IN_COMMANDS`：此指令用于启用 MSH 命令行 shell 的内置命令功能，包括常用的命令如 ls、cd 等。
- `FINSH_USING_DESCRIPTION`：此指令用于启用 FinSH 命令行 shell 的命令描述功能，可以显示命令的描述信息。
- `FINSH_ARG_MAX`：此指令定义了 FinSH 命令行 shell 单个命令的最大参数个数。

#### fastlz组件配置选项：

-   `FASTLZ_SAMPLE_COMPRESSION_LEVEL `:定义 FASTLZ 压缩算法的压缩级别.

#### kawaii_mqtt组件

- `PKG_USING_KAWAII_MQTT`：此指令用于启用 Kawaii MQTT 包，它是一个轻量级的 MQTT（Message Queuing Telemetry Transport）协议实现。
- ` KAWAII_MQTT_LOG_IS_SALOF`：此指令用于配置 Kawaii MQTT 日志输出到 SALOF（Simple Asynchronous Log Output Framework）。
- ` SALOF_USING_LOG`：此指令用于启用 SALOF 日志功能。
- ` SALOF_USING_SALOF`：此指令用于启用 SALOF 框架。
- ` SALOF_BUFF_SIZE 512`：此指令定义了 SALOF 缓冲区的大小。
- ` SALOF_FIFO_SIZE 2048`：此指令定义了 SALOF FIFO（First-In-First-Out）队列的大小。
- ` SALOF_TASK_STACK_SIZE 2048`：此指令定义了 SALOF 任务的堆栈大小。
- ` SALOF_TASK_TICK 50`：此指令定义了 SALOF 任务的时间片大小。
- ` SALOF_LOG_LEVEL 4`：此指令定义了 SALOF 日志的级别。
- ` KAWAII_MQTT_LOG_LEVEL 4`：此指令定义了 Kawaii MQTT 日志的级别。
- ` KAWAII_MQTT_VERSION 4`：此指令定义了 Kawaii MQTT 的版本。
- ` KAWAII_MQTT_KEEP_ALIVE_INTERVAL 100`：此指令定义了 Kawaii MQTT 的心跳间隔，以毫秒为单位。
- ` KAWAII_MQTT_THREAD_STACK_SIZE 4096`：此指令定义了 Kawaii MQTT 线程的堆栈大小。
- ` KAWAII_MQTT_THREAD_PRIO 5`：此指令定义了 Kawaii MQTT 线程的优先级。
- ` KAWAII_MQTT_THREAD_TICK 50`：此指令定义了 Kawaii MQTT 线程的时间片大小。
- ` KAWAII_MQTT_MAX_PACKET_ID 0xFFFE`：此指令定义了 Kawaii MQTT 的最大数据包 ID。
- ` KAWAII_MQTT_TOPIC_LEN_MAX 64`：此指令定义了 Kawaii MQTT 的最大主题长度。
- ` KAWAII_MQTT_ACK_HANDLER_NUM_MAX 64`：此指令定义了 Kawaii MQTT 的最大 ACK（确认）处理器数量。
- ` KAWAII_MQTT_DEFAULT_BUF_SIZE 1024`：此指令定义了 Kawaii MQTT 的默认缓冲区大小。
- ` KAWAII_MQTT_DEFAULT_CMD_TIMEOUT 4000`：此指令定义了 Kawaii MQTT 的默认命令超时时间，以毫秒为单位。
- ` KAWAII_MQTT_MAX_CMD_TIMEOUT 20000`：此指令定义了 Kawaii MQTT 的最大命令超时时间，以毫秒为单位。
- ` KAWAII_MQTT_MIN_CMD_TIMEOUT 1000`：此指令定义了 Kawaii MQTT 的最小命令超时时间，以毫秒为单位。
- ` KAWAII_MQTT_RECONNECT_DEFAULT_DURATION 1000`：此指令定义了 Kawaii MQTT 的默认重新连接间隔，以毫秒为单位。

#### persimmon ui组件配置选项：

- ` PKG_USING_PERSIMMON_SRC`：此指令用于启用 Persimmon 源码包。
- ` PKG_PERSIMMON_CPP_LOGGER`：此指令用于启用 Persimmon 的 C++ 日志记录器。
- ` PKG_PERSIMMON_USE_LOAD_TIME`：此指令用于启用 Persimmon 的加载时间功能。
- ` PKG_PERSIMMON_USE_RENDER_TIME`：此指令用于启用 Persimmon 的渲染时间功能。
- ` PKG_USING_PERSIMMON_SRC_LATEST_VERSION`：此指令用于使用 Persimmon 源码包的最新版本。
- ` PKG_PERSIMMON_USING_DEMO_NONE`：此指令用于禁用 Persimmon 的演示示例。
- ` PKG_PERSIMMON_DEMO_SEL "NONE"`：此指令用于选择 Persimmon 的演示示例，此处选择了 "NONE"，即没有选择任何演示示例。
- ` PKG_USING_JS_PERSIMMON`：此指令用于启用 JS Persimmon。
- ` PKG_USING_JS_PERSIMMON_LATEST_VERSION`：此指令用于使用 JS Persimmon 的最新版本。
- ` PKG_JS_PERSIMMON_THREAD_STACK_SIZE 64`：此指令定义了 JS Persimmon 线程的堆栈大小。
- ` PKG_JS_PERSIMMON_THREAD_PRIORITY 16`：此指令定义了 JS Persimmon 线程的优先级。
- ` PKG_JS_PERSIMMON_ENABLE_LOGO`：此指令用于启用 JS Persimmon 的 Logo。
- ` PKG_JS_PERSIMMON_LOGO_SHOW_TIME_MS 500`：此指令定义了 JS Persimmon Logo 的显示时间，以毫秒为单位。
- ` PKG_JS_PERSIMMON_WAITING_READY_SEM`：此指令用于等待 JS Persimmon 准备就绪的信号量。
- ` PKG_JS_PERSIMMON_ENABLE_DEFAULT_LOGO`：此指令用于启用 JS Persimmon 的默认 Logo。

- ` JSFW_USING_DCM`：此指令用于启用 JSFW（JavaScript Framework）中的 DCM（Data Channel Manager）功能。
- ` JSFW_USING_DATATYPE`：此指令用于启用 JSFW 中的数据类型功能。
- ` JSFW_USING_EMQ`：此指令用于启用 JSFW 中的 EMQ（Event Message Queue）功能。
- ` EMQ_SYSTEM_HUB_CHANNEL_NUM 20`：此指令定义了 EMQ 系统中的 Hub 通道数量。
- ` JSFW_USING_UBJSON`：此指令用于启用 JSFW 中的 UBJSON（Universal Binary JSON）功能。
- ` JSFW_API_LEVEL 2`：此指令定义了 JSFW 的 API 级别。