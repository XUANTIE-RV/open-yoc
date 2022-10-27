

## 概述

yoc 的 tydb(T-head YOC Debug Bridge),debug工具 主要功能有：
• 上位机和设备之间上传/下载/删除文件
• 运行设备的shell（命令）
• 支持 ymodem 协议固件下载功能
• 录音功能(3/5路 5路录音需flac压缩传输)

## 配置

无

## 接口列表

| 函数                                                             | 说明
| :----------------------------------------------------------------| :---------------------------------- |
| void adb_cmd_pushinfo(char *cmd, int type, char *data);          | 收到 AT+PUSHINFO 指令时调用，解析文件大小、给上位机上报 payload大小、offset信息
| void adb_cmd_pullinfo(char *cmd, int type, char *data);          | 收到 AT+PULLINFO 指令时调用，判断文件是否存在；给上位机上报 文件大小、payload信息
| void adb_cmd_pushstart(char *cmd, int type, char *data);         | 收到 AT+PUSHSTART 指令时调用，控制 adb push开始
| void adb_cmd_pushfinish(char *cmd, int type, char *data);        | 收到 AT+PUSHFINISH 指令时调用，控制 adb push结束
| void adb_cmd_pullstart(char *cmd, int type, char *data);         | 收到 AT+PULLSTART 指令时调用，控制 adb pull开始
| void adb_cmd_pullfinish(char *cmd, int type, char *data);        | 收到 AT+PULLFINISH 指令时调用，控制 adb pull结束
| void adb_cmd_pushpost(char *cmd, int type, char *data);          | 收到 AT+PUSHPOST 指令时调用，上位机发送AT+PUSHPOST=size 告知下位机准备好接收 size大小的数据
| void adb_cmd_pullreq(char *cmd, int type, char *data);           | 收到 AT+PULLREQ 指令时调用，上位机发送 AT+PULLREQ，请求设备端发送数据
| void adb_cmd_delete(char *cmd, int type, char *data);            | 收到 AT+ADBDELETE 指令时调用，删除设备端指令文件
| void adb_cmd_adbok(char *cmd, int type, char *data);             | 收到 AT+ADBOK 指令时调用，上位机告知设备端已准备好接收数据或接收完数据
| void adb_cmd_devices(char *cmd, int type, char *data);           | 收到 AT+ADBDEVICES 指令时调用，上位机获取设备id
| void adb_cmd_cli(char *cmd, int type, char *data);               | 收到 AT+ADBCLI 指令时调用，上位机在adb模式下执行cli命令
| void adb_cmd_start(char *cmd, int type, char *data);             | 收到 AT+ADBSTART 指令时调用，上位机开始ADB交互，tydb工具开始执行调用该命令
| void adb_cmd_stop(char *cmd, int type, char *data);              | 收到 AT+ADBSTOP 指令时调用，上位机停止ADB交互，tydb工具执行完毕调用该命令
| void adb_cmd_exit(char *cmd, int type, char *data);              | 收到 AT+ADBEXIT 指令时调用，设备端切换到cli命令模式
| adb_status_e  adb_get_status(void);                              | 获取adb 状态
| void adb_cmd_tramsmit_start(char *cmd, int type, char *data);    | 收到 AT+RECORDSTART 指令时调用 上位机控制设备端开始透传数据(目前只用于控制录音开始)
| void adb_cmd_tramsmit_stop(char *cmd, int type, char *data);     | 收到 AT+RECORDSTOP 指令时调用  上位机控制设备端停止透传数据(目前只用于控制录音结束)
| int  adb_cmd_transmit_write(uint8_t *buff, int size);            | 外部调用该接口往adb内部ringbuffer塞数据，目前用于录音
| int adb_get_transmit_channel(void);                              | 外部获取数据传输通道，目前用于获取录音channel
| int adb_get_compress_flag(void);                                 | 外部获取数据传输时是否压缩
| int adb_transmit_raw_data_add(uint32_t size);                    | 外部使用，更新adb已传输数据大小，目前用于录音压缩传输
| int adb_start(void);                                             | adb初始化时使用, 函数内部 申请内存、初始化信号量、创建"adb_task"任务

## 接口详细说明

### adb_cmd_pushinfo

`void adb_cmd_pushinfo(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PUSHINFO 指令时调用，解析文件大小、给上位机上报 payload大小、offset信息
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_pullinfo

`void adb_cmd_pullinfo(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PULLINFO 指令时调用，判断文件是否存在；给上位机上报 文件大小、payload信息
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_pushstart

`void adb_cmd_pushstart(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PUSHSTART 指令时调用，控制 adb push开始
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_pushfinish

`void adb_cmd_pushfinish(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PUSHFINISH 指令时调用，控制 adb push结束
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_pullstart

`void adb_cmd_pullstart(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PULLSTART 指令时调用，控制 adb pull开始
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_pullfinish

`void adb_cmd_pullfinish(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PULLFINISH 指令时调用，控制 adb pull结束
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_pushpost

`void adb_cmd_pushpost(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PUSHPOST 指令时调用，上位机发送AT+PUSHPOST=size 告知下位机准备好接收 size大小的数据
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_pullreq

`void adb_cmd_pullreq(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+PULLREQ 指令时调用，上位机发送 AT+PULLREQ，请求设备端发送数据
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_delete

`void adb_cmd_delete(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+ADBDELETE 指令时调用，删除设备端指令文件
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_adbok

`void adb_cmd_adbok(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+ADBOK 指令时调用，上位机告知设备端已准备好接收数据或接收完数据
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_devices

`void adb_cmd_devices(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+ADBDEVICES 指令时调用，上位机获取设备id
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_cli

`void adb_cmd_cli(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+ADBCLI 指令时调用，上位机在adb模式下执行cli命令
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_start

`void adb_cmd_start(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+ADBSTART 指令时调用，上位机开始ADB交互，tydb工具开始执行调用该命令
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_stop

`void adb_cmd_stop(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+ADBSTOP 指令时调用，上位机停止ADB交互，tydb工具执行完毕调用该命令
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_exit

`void adb_cmd_exit(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+ADBEXIT 指令时调用，设备端切换到cli命令模式
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_tramsmit_start

`void adb_cmd_tramsmit_start(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+RECORDSTART 指令时调用 上位机控制设备端开始透传数据(目前只用于控制录音开始)
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_cmd_tramsmit_stop

`void adb_cmd_tramsmit_stop(char *cmd, int type, char *data);`

- 功能描述:
  - 收到 AT+RECORDSTOP 指令时调用  上位机控制设备端停止透传数据(目前只用于控制录音结束)
- 参数:
  - `cmd`:  cmd
  - `type`: 区分 cmd 类型(TEST_CMD, READ_CMD, WRITE_CMD, EXECUTE_CMD)
  - `data`:  AT指令 = 后的数据
- 返回值:
  - 无

### adb_start

`int adb_start(void);`

- 功能描述:
  - adb初始化时使用, 函数内部 申请内存、初始化信号量、创建"adb_task"任务
- 参数:
  - 无
- 返回值:
  - 无意义

### adb_get_status

`adb_status_e  adb_get_status(void);`

- 功能描述:
  - 获取 adb 状态
- 参数:
  - 无
- 返回值:
  - adb 目前状态

### adb_cmd_transmit_write

`int  adb_cmd_transmit_write(uint8_t *buff, int size);`

- 功能描述:
  - 外部调用该接口往adb内部ringbuffer塞数据，目前用于录音
- 参数:
  - `buff`:  数据起始地址
  - `size`:  数据大小
- 返回值:
  - 0  成功
  - -1 失败

### adb_get_transmit_channel

`int adb_get_transmit_channel(void);`

- 功能描述:
  - 外部获取数据传输通道，目前用于获取录音channel
- 参数:
  - 无
- 返回值:
  - 0  通道数

### adb_get_compress_flag

`int adb_get_compress_flag(void);`

- 功能描述:
  - 外部获取数据传输时是否压缩
- 参数:
  - 无
- 返回值:
  - 1  压缩
  - 0  不压缩

### adb_transmit_raw_data_add

`int adb_transmit_raw_data_add(uint32_t size);`

- 功能描述:
  - 外部使用，更新adb已传输数据大小，目前用于录音压缩传输
- 参数:
  - `size`:  传输数据增加大小
- 返回值:
  - 1  传输数据达到指定大小，结束传输
  - 0  传输数据未达到指定大小

## 示例代码

无

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。