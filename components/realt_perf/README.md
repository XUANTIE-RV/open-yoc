## 概述

`realt_perf`组件是一个适用于裸核和实时操作系统的性能分析工具，用于收集和分析系统的性能数据。它可以帮助开发者分析CPU使用情况，找到性能瓶颈。该组件的主要功能如下：

#### 事件计数
- 跟踪各类硬件PMU事件，例如CPU周期、指令数、缓存命中、分支预测命中等
- 支持按照CPU计数和按线程计数

#### 函数采样统计
- 统计函数的CPU利用率
- 获取各个函数之间的调用关系

## 接口列表

### 基础API
| 函数 | 说明 |
| :--- | :--- |
| perf_record_init | 初始化perf record模块 |
| perf_record_deinit | 销毁perf record模块 |
| perf_record_start | 开始运行perf record |
| perf_record_stop | 停止运行perf record |
| perf_record_per_core_buffer_size | 获取每个核心的记录缓冲区大小 |
| perf_stat_init | 初始化perf_stat模块 |
| perf_stat_deinit | 销毁perf_stat模块 |
| perf_stat_start | 开始运行perf stat |
| perf_stat_stop | 停止运行perf stat并打印结果 |
| perf_get_event_id | 通过事件名称查询事件ID |
| perf_num_cpus_get | 获取系统CPU核心数量 |
| perf_event_list | 在命令行上列出所有事件 |

### 可重写API
| 函数 | 说明 |
| :--- | :--- |
| perf_printf | 格式控制打印函数 |
| perf_malloc | 内存分配函数 |
| perf_realloc | 内存重新分配函数 |
| perf_free | 内存释放函数 |
| perf_record_dump | 记录perf record数据 |
| perf_record_alloc_data_buf | perf record数据记录缓冲区申请 |
| perf_record_free_data_buf | perf record数据记录缓冲区释放 |

## 接口详细说明

### perf_record_init
`int perf_record_init(void);`

- 功能描述:
    - 初始化`perf record`模块，在使用perf record功能前必须调用此函数

- 参数:
    - 无

- 返回值:
    - 0: 成功
    - 其他：失败

### perf_record_deinit
`int perf_record_deinit(void);`

- 功能描述:
    - 销毁`perf record`模块，释放相关资源

- 参数:
    - 无

- 返回值:
    - 0: 成功
    - 其他：失败

### perf_record_start
`int perf_record_start(uint32_t duration_ms, uint32_t frequency);`

- 功能描述:
    - 启动`perf record`采样，按照函数调用关系，统计各个函数的CPU使用率

- 参数:
    - `duration_ms`: 记录时长
    - `frequency`: 采样频率

- 返回值:
    - 0: 成功
    - 其他：失败

### perf_record_stop
`int perf_record_stop(void);`

- 功能描述:
    - 停止`perf record`采样

- 参数:
    - 无

- 返回值:
    - 0: 成功
    - 其他: 失败

### perf_record_per_core_buffer_size
`size_t perf_record_per_core_buffer_size(void);`

- 功能描述:
    - 获取perf record在每个核心上的记录缓冲区大小

- 参数:
    - 无

- 返回值:
    - perf record在每个核心上的记录缓冲区大小

### perf_record_dump

`int perf_record_dump(void);`

- 功能描述:
    - 输出`perf record`数据，定义为弱符号，用户可以自行实现该功能。

- 参数:
    - 无。

- 返回值:
    - 0：成功。
    - 其他：失败。

### perf_stat_init

`void perf_stat_init(void);`

- 功能描述:
    - 初始化`perf-stat`模块，如果使用裸系统，无需执行该函数。如果使用实时操作系统，需要在系统初始化时调用这个函数，在多核场景下，该函数由`CPU0`在其他`CPU`启动之前执行。

- 参数:
    - 无。

- 返回值:
    - 无。

### perf_stat_deinit

`void perf_stat_deinit(void);`

- 功能描述:
    - 销毁`perf-stat`模块，在裸系统运行条件下无需执行该函数。在使用实时操作系统的条件下在该函数在系统关闭时由`CPU0`执行。

- 参数:
    - 无。

- 返回值:
    - 无。

### perf_stat_start

`void perf_stat_start(perf_stat_args_t* args);`

- 功能描述:
    - 根据指定的参数，启动`perf stat`。在多核运行条件下，每个CPU调用一次。

- 参数:
    - `args`：参数结构体

- 返回值:
    - 无。

### perf_stat_stop

`void perf_stat_stop(void);`

- 功能描述:
    - 停止`perf stat`并打印结果。在多核运行条件下，所有CPU各自调用一次，但是只有`CPU0`调用这个函数的时候会进行结果打印。

- 参数:
    - 无。

- 返回值:
    - 无。

### perf_num_cpus_get

`int perf_num_cpus_get(void);`

- 功能描述:
    - 获取系统CPU核心数量

- 参数:
    - 无

- 返回值:
    - 系统CPU核心数量

### perf_printf

`int perf_printf(const char *fmt, ...);`

- 功能描述:
    - 为组件提供格式化输出功能，定义为弱符号，用户可以自行实现该功能。

- 参数:
    - `fmt`：格式化字符串。
    - `...`：可变参数列表。

- 返回值:
    - 0：成功。
    - 其他：失败。

### perf_malloc

`void *perf_malloc(size_t size);`

- 功能描述:
    - 为组件分配内存，定义为弱符号，用户可以自行实现该功能。

- 参数:
    - `size`：要分配的内存大小。

- 返回值:
    - 成功：返回分配的内存地址。
    - 失败：返回 `NULL`。

### perf_realloc

`void *perf_realloc(void *ptr, size_t size);`

- 功能描述:
    - 调整已分配的内存块大小，定义为弱符号，用户可以自行实现该功能。

- 参数:
    - `ptr`：指向先前分配的内存块的指针。
    - `size`：新内存块的大小。

- 返回值:
    - 成功：返回新的内存地址。
    - 失败：返回 `NULL`。

### perf_free

`void perf_free(void* ptr);`

- 功能描述:
    - 释放组件分配的内存，定义为弱符号，用户可以自行实现该功能。

- 参数:
    - `ptr`：要释放的内存块的指针。

- 返回值:
    - 无。

### perf_record_alloc_data_buf

`unsigned long *perf_record_alloc_data_buf(void);`

- 功能描述:
    - 为`perf record`的数据记录分配内存地址，定义为弱符号，用户可以自行实现该功能。

- 参数:
    - 无。

- 返回值:
    - 成功：返回数据缓冲区地址。
    - 失败：返回 `NULL`。

### perf_record_free_data_buf

`void perf_record_free_data_buf(unsigned long *buf);`

- 功能描述:
    - 释放`perf record`记录数据的内存地址，定义为弱符号，用户可以自行实现该功能。

- 参数:
    - `buf`：要释放的记录数据缓冲区地址。

- 返回值:
    - 无。

### perf_get_event_id

`uint16_t perf_get_event_id(char* ev_name);`

- 功能描述:
    - 查询PMU事件名称对应的ID。

- 参数:
    - `ev_name`：事件名称字符串

- 返回值:
    - 正整数: 事件ID
    - 0: 失败

### perf_list_events

`void perf_list_events(void);`

- 功能描述:
    - 在命令行上列出所有事件

- 参数:
    - 无

- 返回值:
    - 无
