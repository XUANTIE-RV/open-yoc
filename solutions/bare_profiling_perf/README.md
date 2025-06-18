# 概述

`bare_profiling_perf` 是一个玄铁RTOS SDK中面向baremetal领域的perf功能使用示例。

# 基于Linux编译运行

## 编译

GCC使用以下命令编译：
```bash
./do_build.sh <cpu> <platform>
```
LLVM使用以下命令编译：
```bash
./do_build_llvm.sh <cpu> <platform>
```

`cpu`和`platform`支持列表：
- cpu: <br />
        `xiaohui`平台的cpu参考 [`components/xuantie_cpu_sdk/xt_rtos_sdk.csv`](../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv) 的 `cpu_list`
- platform: <br />
        xiaohui

以玄铁`c908v`类型为例，可以使用如下命令编译:
```bash
./do_build.sh c908v xiaohui
```

功能选择：
如果运行`perf stat`，将`Makefile`里面的`CONFIG_DEMO_PERF_STAT`宏打开。如果运行`perf record`，将`Makefile`里面的`CONFIG_DEMO_PERF_RECORD`宏打开。每次运行只能选择一个。
```makefile
  CFLAGS += -DCONFIG_DEMO_PERF_STAT=1
  #CFLAGS += -DCONFIG_DEMO_PERF_RECORD=1
```

多核编译：
多核编译时需要将`Makefile`里面的多核的宏`CONFIG_SMP`和`CONFIG_NR_CPUS`打开。
```makefile
# TODO: solution defines
CFLAGS += -DCONFIG_KERNEL_NONE=1 -DCONFIG_DEBUG=1
CFLAGS += -DCONFIG_PROFILING_PERF_PER_CORE_BUFFER_SIZE=262144
CFLAGS += -DCONFIG_PERF_DATA_GDB_MANUAL_DUMP=1
CFLAGS += -DCONFIG_PERF_DATA_SEMIHOST2_DUMP=0
# multi-core
CFLAGS += -DCONFIG_SMP=1 -DCONFIG_NR_CPUS=2
```

## perf record运行

`perf record`示例可以基于玄铁QEMU或FPGA平台运行

### 基于玄铁QEMU运行

单核运行：
```bash
qemu-system-riscv64 -machine xiaohui -nographic -kernel yoc.elf -cpu c908v -gdb tcp::1234
```

smp多核运行：
```bash
qemu-system-riscv64 -machine xiaohui -nographic -kernel yoc.elf -cpu c908v -smp cpus=2 -gdb tcp::1234
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### perf record运行结果
程序运行就会开始采集数据，结束采集有3种情况：
1. 开始采集之后，会以当前时间为起点，经过<duration>毫秒时间后自动结束采集
2. 采集数据的buffer满了，提前退出采集
3. 代码执行中碰到`perf_record_stop`函数的调用

#### 通过GDB手动dump data
##### 单核采集结束时的串口打印如下：
```
bare_profiling_perf start.
cpuid[0] perf record test start.[duration: 5000ms, frequency: 999Hz]
cpuid[0] alloc databuf: 0x500097c0
cpuid[0] ret:2320000
cpuid[0] ret:3312500
cpuid[0] ret:505000
cpuid[0] come to perf_record_stop manual.
cpuid[0] perf record test finish.
cpuid[0] stop record, pdata->tick: 3801
cpuid[0] please use riscv64-unknown-elf-gdb to dump data:
cpuid[0] dump memory perf_rtos_0.data 0x500097c0 0x500097c0+152112 
cpuid[0] when finish dump, please input set g_dump_flag=1 to continue
```

##### 多核采集结束时串口输出如下：
```
bare_profiling_perf start.
cpuid[0] perf record test start.[duration: 5000ms, frequency: 999Hz]
cpuid[0] alloc databuf: 0x5000dc60
cpuid[1] perf record test start.[duration: 5000ms, frequency: 999Hz]
cpuid[1] alloc databuf: 0x5004dc70
cpuid[0] ret:2320000
cpuid[1] ret:2320000
cpuid[1] ret:3312500
cpuid[0] ret:3312500
cpuid[1] ret:505000
cpuid[0] ret:505000
cpuid[1] come to perf_record_stop manual.
cpuid[1] stop record, pdata->tick: 3682
cpuid[0] come to perf_record_stop manual.
cpuid[0] stop record, pdata->tick: 3712
cpuid[0] please use riscv64-unknown-elf-gdb to dump data:
cpuid[0] dump memory perf_rtos_0.data 0x5000dc60 0x5000dc60+148560 
cpuid[0] dump memory perf_rtos_1.data 0x5004dc70 0x5004dc70+88464 
cpuid[0] when finish dump, please input set g_dump_flag=1 to continue
```

当看到 `when finish dump, please input set g_dump_flag=1 to continue`提示信息时

1. 如果是QEMU运行，则需要打开另外一个终端，并输入以下命令：
```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```
请确保`gdbinit`文件里面的内容是 `target remote localhost:1234`

2. FPGA运行时，直接在之前使用`riscv64-unknown-elf-gdb`加载 yoc.elf 的终端界面按下`Ctrl+C`组合按键。

然后再根据串口提示信输入`dump memory ... `相关的命令进行dump。dump完成之后会在当前目录下生成`perf_rtos_0.data`等文件。
如果是多核就多dump几次，相应的当前目录也会生成多个文件。

#### 通过semihost dump data
QEMU不支持使用semihost功能。
使用semihost功能时，需要将`Makefile`的宏进行更改，如下。
```makefile
CFLAGS += -DCONFIG_PERF_DATA_GDB_MANUAL_DUMP=0
CFLAGS += -DCONFIG_PERF_DATA_SEMIHOST2_DUMP=1
```

DebugServer连接FPGA的命令如下：
```
DebugServerConsole -ls -prereset
```
输入以上命令之后，DebugServerConsole的连接界面有如下信息：
```
...
Switch input to Semihosting!
...
```
##### 单核采集结束时的串口打印如下：
```
bare_profiling_perf start.
cpuid[0] perf record test start.[duration: 5000ms, frequency: 999Hz]
cpuid[0] alloc databuf: 0x5000a9d0
cpuid[0] ret:2320000
cpuid[0] ret:3312500
cpuid[0] ret:505000
cpuid[0] come to perf_record_stop manual.
cpuid[0] stop record, pdata->tick: 4827
cpuid[0] start to dump [perf_rtos_0.data] perf data[len: 154960] with semihost.[start: 4050 ms]
cpuid[0] success dump perf_rtos_0.data with semihost.[end: 58081 ms]
cpuid[0] will free databuf[0x5000a9d0]
cpuid[0] free databuf[0x5000a9d0] ok
cpuid[0] perf record test finish.
```

##### 多核采集结束时串口输出如下：
```
bare_profiling_perf start.
cpuid[0] perf record test start.[duration: 5000ms, frequency: 999Hz]
cpuid[1] perf record test start.[duration: 5000ms, frequency: 999Hz]
cpuid[0] alloc databuf: 0x5000ee80
cpuid[1] alloc databuf: 0x5004ee90
cpuid[0] ret:2320000
cpuid[1] ret:2320000
cpuid[0] ret:3312500
cpuid[1] ret:3312500
cpuid[0] ret:505000
cpuid[1] ret:505000
cpuid[0] come to perf_record_stop manual.
cpuid[1] come to perf_record_stop manual.
cpuid[0] stop record, pdata->tick: 4826
cpuid[1] stop record, pdata->tick: 4826
cpuid[0] start to dump [perf_rtos_0.data] perf data[len: 154912] with semihost.[start: 4077 ms]
cpuid[0] success dump perf_rtos_0.data with semihost.[end: 58160 ms]
cpuid[0] start to dump [perf_rtos_1.data] perf data[len: 116288] with semihost.[start: 58166 ms]
cpuid[0] success dump perf_rtos_1.data with semihost.[end: 100471 ms]
cpuid[0] will free databuf[0x5000ee80]
cpuid[1] no need dump.
cpuid[0] free databuf[0x5000ee80] ok
cpuid[1] will free databuf[0x5004ee90]
cpuid[1] free databuf[0x5004ee90] ok
cpuid[0] perf record test finish.
cpuid[1] perf record test finish.
```

数据采集完毕之后，会在连接DebugServer端目录生成`perf_rtos_0.data`等文件。多核会生成多个文件。

### perf.data数据分析
使用realt_perf组件下的`stackcollapse.py`脚本进行解析。
如：
```bash
python3 ../../components/realt_perf/scripts/stackcollapse.py perf_rtos_0.data yoc.elf > 0.txt
```
```bash
python3 ../../components/realt_perf/scripts/stackcollapse.py perf_rtos_1.data yoc.elf > 1.txt
```

得到的结果如下：
```
Samples: 4829; Frequency: 999Hz; Duration: 5000ms, Elapsed: 4834ms
+-----+------------+---------+-------------+------------------------------------------------------------------------------+
| CPU | Num Sample | Percent | Symbol      | Call-Graph                                                                   |
+-----+------------+---------+-------------+------------------------------------------------------------------------------+
|  0  |    4800    |  99.40% | mdelay      | [100.00%] mdelay <- perf_record_test <- main                                 |
+-----+------------+---------+-------------+------------------------------------------------------------------------------+
|  0  |     25     |  0.52%  | test_func1  | [92.00%] test_func1                                                          |
|     |            |         |             | [8.00%] test_func1 <- test_func3 <- test_func4 <- perf_record_test <- main   |
+-----+------------+---------+-------------+------------------------------------------------------------------------------+
|  0  |     3      |  0.06%  | perf_printf | [100.00%] perf_printf <- perf_record_test <- main                            |
+-----+------------+---------+-------------+------------------------------------------------------------------------------+
|  0  |     1      |  0.02%  | test_func2  | [100.00%] test_func2 <- test_func3 <- test_func4 <- perf_record_test <- main |
+-----+------------+---------+-------------+------------------------------------------------------------------------------+
```

#### 表头含义:
Samples：总的采样次数

Frequency：采样频率

Duration：期望的采样时间

Elapsed：实际消耗的时间

#### 表格释义:
CPU: 当前采用的CPU，0表示cpu0，1表示cpu1，以此类推

Num Sample: 当前函数的采样次数

Percent: 采样占总采样的百分比

Symbol: 函数名称

Call-Graph: 函数调用关系，调用链前面的百分比表示函数被当前调用链调用的占比

### 制作火焰图
使用realt_perf组件下`stackcollapse.py`完成解析以后，每个.data文件会生成一个对应的.fold文件，继续使用`flamegraph.pl`脚本进行解析。
```bash
../../components/realt_perf/scripts/flamegraph.pl 0.fold > 0.svg
../../components/realt_perf/scripts/flamegraph.pl 1.fold > 1.svg
```
可以得到矢量图文件`0.svg`和`1.svg`，然后用浏览器或者其他软件打开即可。

## perf stat运行

`perf stat`示例只能通过FPGA运行。

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

#### 单核运行结果
程序开始运行后，会延时一段事件并统计PMU数据，统计完成后，串口输出如下：
````
bare_profiling_perf start.
cpuid[0] start, delaying for seconds, please wait...
cpuid[0] stop

Performance counter stat for system wide

150155957                      cycles                          #1.000                CPUs utilized
50056517                       instructions-retired             16667937.000        /sec
139924768                      l1-icache-access                 46592480.000        /sec
11                             l1-icache-miss                   3.663               /sec
0                              itlb-miss                        0.000               /sec
0                              jtlb-miss                        0.000               /sec
3058                           condition-branch-mispred         1018.260            /sec
19985918                       condition-branch                 6654958.000         /sec
13                             indirect-branch-miss             4.329               /sec
4550                           indirect-branch                  1515.070            /sec
16866                          store-instruction                5616.081            /sec
18109                          l1-dcache-load-access            6029.978            /sec

3.003162 seconds time elapsed
````
显示格式与`linux perf stat`类似，表格左边第一列是硬件PMU计数器的读数，中间一列是PMU事件名称，右边一列是平均每秒的计数（即PMU读数除以时间）。

#### 多核运行结果
```
bare_profiling_perf start.
cpuid[0] start, delaying for seconds, please wait...
cpuid[1] start, delaying for seconds, please wait...
cpuid[0] stop
cpuid[1] stop

Performance counter stat for system wide

300321224                      cycles                          #2.000                CPUs utilized
100003338                      instructions-retired             33297940.000        /sec
120129382                      l1-icache-access                 39999276.000        /sec
15                             l1-icache-miss                   4.995               /sec
0                              itlb-miss                        0.000               /sec
0                              jtlb-miss                        0.000               /sec
6119                           condition-branch-mispred         2037.433            /sec
39926567                       condition-branch                 13294281.000        /sec
20                             indirect-branch-miss             6.659               /sec
9099                           indirect-branch                  3029.678            /sec
33693                          store-instruction                11218.700           /sec
36195                          l1-dcache-load-access            12051.787           /sec

3.003289 seconds time elapsed
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁RTOS SDK用户手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁RTOS SDK用户手册》

2、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁RTOS SDK用户手册》

2、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

3、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。

