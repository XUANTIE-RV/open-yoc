# 概述

aos组件包括aos操作系统API(实现操作系统接口的定义)、cxx初始化、设备管理、eventloop、uService、分区信息获取、内存管理debug、异常处理、环形buffer、KV、系统信息获取、软狗、系统日志等接口的定义。

# 使用

##  任务

在 RTOS 中，任务（Task）是程序独立运行的最小单位，由操作系统的任务调度系统实现任务的并发执行。每个任务都有独立的栈空间与任务控制块 （TCB）。RTOS 采用优先级抢占高度策略，每个任务都要备注以下基础信息：

- 任务栈：可采用动态分配与静态分配的方式，确保栈的大小能满足任务运行时不会造成栈溢出
- 任务主函数：是任务运行的主体程序，该函数的退出，表示任务的结束
- 任务优先级：高优先级的任务会抢占运行，合理的多任务同步设计可避免低优先级任务得不到运行机会
- 任务状态：是任务调度器调度决策的基础，合理的任务状态是多任务程序设计的基础

任务的基本操作：

- 创建：`aos_task_new`; `aos_task_new_ext`
- 放弃时间片：`void aos_task_yield();`
- 获取当前任务：`void aos_task_yield();`
- 任务退出：`void aos_task_exit(int *code*);`

##  内存管理

RTOS 提供多种内存管理方案，满足不同应用的需要。动态内存管理是最常见的一种方式。常见的操作有：

分配内存：`void *aos_malloc(unsigned int size);` `void *aos_calloc(unsigned int size, int num);`
重新分配内存： `void *aos_realloc(void *mem, unsigned int size);`
内存释放：`void aos_free(void *mem);`

##  互斥锁

在编程中，引入了对象互斥锁的概念，来保证共享数据操作的完整性。每个对象都对应于一个可称为" 互斥锁" 的标记，这个标记用来保证在任一时刻，只能有一个线程访问该对象。互斥锁的基本四个操作：

- 创建 Create `int aos_mutex_new(aos_mutex_t *mutex);`
- 加锁 Lock `int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout);`
- 解锁 Unlock `int aos_mutex_unlock(aos_mutex_t *mutex);`
- 销毁 Destroy `void aos_mutex_free(aos_mutex_t *mutex);`

##  信号量

信号量(Semaphore)，有时被称为信号灯，是在多线程环境下使用的一种设施，是可以用来保证两个或多个关键代码段不被并发调用。在进入一个关键代码段之前，线程必须获取一个信号量；一旦该关键代码段完成了，那么该线程必须释放信号量。其它想进入该关键代码段的线程必须等待直到第一个线程释放信号量。为了完成这个过程，需要创建一个信号量VI，然后将Acquire Semaphore VI以及Release Semaphore VI分别放置在每个关键代码段的首末端。确认这些信号量VI引用的是初始创建的信号量。信号量的基本四个操作：

- 创建 Create `int aos_sem_new(aos_sem_t *sem, int count);`
- 等待信号 Wait  `int aos_sem_wait(aos_sem_t *sem, unsigned int timeout);` 
- 释放信号 Signal `void aos_sem_signal(aos_sem_t *sem);`
- 销毁 Destroy `void aos_sem_free(aos_sem_t *sem);` 

##  消息队列

消息队列提供了一种从一个任务向另一个任务发送数据的方法。  每个数据都被认为含有一个类型，接收的任务可以独立地接收含有不同类型的数据结构。消息队列的特点：

1. 消息队列是消息的链表，具有特定的格式，存放在内存中并由消息队列标识符标识
2. 消息队列允许一个或多个任务向它写入与读取消息
3. 消息队列可以实现消息的随机查询,消息不一定要以先进先出的次序读取,也可以按消息的类型读取.比FIFO更有优势

消息队列的基本操作：

- 创建 Create：`int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg);`
- 发送消息 Send： `int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size);`
- 接收消息 Recv： `int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, unsigned int *size);`
- 销毁 Destroy： `void aos_queue_free(aos_queue_t *queue);`

## 事件标志组

事件标志组是一种实现任务间通信的机制，可用于实现任务间的同步。事件标志组采用位来表示一个事件，通常采用支持32位的变量，其中每一位表示一种事件类型（0表示该事件类型未发生、1表示该事件类型已经发生）。事件标志组具备以下特点：

- 可以是任意一个事件发生时唤醒任务进行事件处理，也可以是几个事件都发生后才唤醒任务进行事件处理
- 多次向任务发送同一事件类型，等效于只发送一次
- 允许多个任务对同一事件进行读写操作
- 事件仅用于任务间的同步，不提供数据传输功能

事件标志组支持的基本操作：

创建 Create：`int aos_event_new(aos_event_t *event, unsigned int flags);`

事件设置 Set： `int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char *opt*);`

事件获取 Get： `int aos_event_get(aos_event_t *event, `
`unsigned int flags, unsigned char opt,unsigned int *actl_flags, unsigned int timeout);`

销毁 Destroy： `void aos_event_free(aos_event_t *event);`

## 定时器

定时器是由操作系统提供的一类系统接口，它构建在硬件定时器基础之上，使系统能够提供不受数目限制的定时器服务。

定时器支持的基本操作：

创建 Create：`int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg, int ms, int repeat);`

启动 Start：`int aos_timer_start(aos_timer_t *timer);`

停止 Stop：`int aos_timer_stop(aos_timer_t *timer);`

销毁 Destory：`void aos_timer_free(aos_timer_t *timer);`

改变 Change：`int aos_timer_change(aos_timer_t *timer, int ms);`

