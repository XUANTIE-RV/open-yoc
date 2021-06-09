## 概述

KV存储系统是YOC中一个以Key-Value方式进行持久化存储的轻量级组件，主要为基于nor flash的小型MCU设备提供通用的Key-Value持久化存储接口。该系统采用极小的代码及内存开销，支持断电保护、磨损均衡等功能。
KV存储系统支持只读模式与读写模式共存，只读模式可以用于工厂生产数据，读写模式可用于运行时的数据存储。

### 断电保护特性
断电保护的设计是为了在修改KV时，能保证KV不会被破坏的事务处理机制，即要写入失败，要么写入成功，对于已经存的KV，写失败后，KV的值仍然为旧值。对于不存在的KV，写失败后，KV不存在。

###  磨损均衡特性
在通常的应用中，部分KV被经常修改，由于FLASH物理特性，擦写的次数有一定的限制，擦写次数超过次数时，该块会损坏不能使用。磨损均衡的设计是将KV的写入分散到多 block上，避免存储在固定的位置上，达到磨损均衡的效果。磨损均衡主要依赖以下两个策略来实现：

- 异地更新策略
  Key-Value键值对采用顺序写入、异地更新的方式，即不再在原存储位置擦除重写，而是在其余空闲位置写入新键值并将原键值标记无效等待回收。这样既可以减少flash的擦除操作次数，又可以提高flash的空间利用率，也避免了对“特定”存储区块过度使用的问题。

- 垃圾回收策略
  当free block总数接近gc下限时，会触发gc操作。flash数据在gc前，存在有效键值和键值交织的情况；gc后，把有效文件数据归并到free block，原区域则被擦除并置入free block。gc循环向后搬运键值。

####  坏块处理特性
Nor flash有一定的擦写次数限制，如果达到这个限制，或者由于物理方面的损坏，会导致这些block写入有问题。KV系统采用直接片上链式读取，擦除与写入时校验数据正确性，不采用特殊的标识信息来处理坏块，当 block 擦除失败或者写失败后，会重新申请新的数据块，避免坏块被错误使用。

## 组件安装
```bash
yoc init
yoc install kv
```

## 配置
无。

## 接口列表

| 函数 | 说明 |
| :--- | :--- |
| aos_kv_init | KV初始化 |
| aos_kv_flash_init | KV初始化FLASH  |
| aos_kv_reset | KV复位 |
| aos_kv_set | KV项设置 |
| aos_kv_setfloat | 设置浮点数类型的KV项 |
| aos_kv_setint | 设置整数类型的KV项 |
| aos_kv_get | 获取KV项 |
| aos_kv_getfloat | 获取浮点数类型的KV项 |
| aos_kv_getint | 获取整数类型的KV项 |
| aos_kv_setstring | 设置字符串类型的KV项 |
| aos_kv_getstring | 获取字符串类型的KV项 |
| aos_kv_del | 删除KV项 |

## 接口详细说明

### aos_kv_init
`int aos_kv_init(const char *partname);`

- 功能描述:
   - 初始化KV组件。

- 参数:
   - `partname`: 分区名称。

- 返回值:
   - 0: 初始化成功。
   - -1: 初始化失败。

### aos_kv_flash_init
`int aos_kv_flash_init(const char *flashname, int addr, int block_num);`

- 功能描述:
   - 初始化KV组件。

- 参数:
   - `flashname`: FLASH名称。
   - `addr`：起始地址。
   - `block_num`：块数目(KV项数目)。

- 返回值:
   - 0: 初始化成功。
   - -1: 初始化失败。

### aos_kv_reset
`int aos_kv_reset(void);`

- 功能描述:
   - 所有KV项复位，恢复成出厂配置。

- 参数:
  - 无

- 返回值:
   - 0: 复位成功。
   - -1: 复位失败。

### aos_kv_set
`int aos_kv_set(const char *key, void *value, int len, int sync);`

- 功能描述:
   - 设置KV项。

- 参数:
   - `key`: KEY名称。
   - `value`：指向值的指针。
   - `len`：值的长度。
   - `sync`：是否立即写入flash的标记(建议一直设置为1，即立即写入)。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### aos_kv_setfloat
`int aos_kv_setfloat(const char *key, float v);`

- 功能描述:
   - 设置值为浮点数的KV项。

- 参数:
   - `key`: KEY名称。
   - `v`：值。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### aos_kv_setint
int aos_kv_setint(const char *key, int v);

- 功能描述:
   - 设置值为整数的KV项。

- 参数:
   - `key`: KEY名称。
   - `v`：值。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### aos_kv_get
int aos_kv_get(const char *key, void *buffer, int *buffer_len);

- 功能描述:
   - 获取指定KV项。

- 参数:
   - `key`: KEY名称（入参）。
   - `buffer`：指向值的缓冲区。
   - `buffer_len`：函数调用前buffer_len为缓冲区长度，函数调用后为值的长度。

- 返回值:
   - 0: 获取成功。
   - -1: 获取失败。

### aos_kv_getfloat
`int aos_kv_getfloat(const char *key, float *value);·

- 功能描述:
   - 获取值为浮点数的指定KV项。

- 参数:
   - `key`: KEY名称（入参）。
   - `value`：指向值的指针（出参）。

- 返回值:
   - 0: 获取成功。
   - -1: 获取失败。
   
### aos_kv_getint
`int aos_kv_getint(const char *key, int *value);`

- 功能描述:
   - 获取值为整数的指定KV项。

- 参数:
   - `key`: KEY名称（入参）。
   - `value`：指向值的指针（出参）。

- 返回值:
   - 0: 获取成功。
   - -1: 获取失败。

### aos_kv_setstring
`int aos_kv_setstring(const char *key, const char *v);`

- 功能描述:
   - 设置值为字符串类型的指定KV项。

- 参数:
   - `key`: KEY名称。
   - `v`：值（字符串）。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### aos_kv_getstring  
`int aos_kv_getstring(const char *key, char *value, int len);`

- 功能描述:
   - 获取值为值为字符串类型的指定KV项。

- 参数:
   - `key`: KEY名称。
   - `vlue`：指向值的缓冲区。
   - `len`：函数调用前len为缓冲区长度，函数调用后为值的长度。

- 返回值:
   - 大于0: 获取成功。
   - -1: 获取失败。
   
### aos_kv_del
`int aos_kv_del(const char *key);`

- 功能描述:
   - 删除指定KV项。

- 参数:
   - `key`: KEY名称。

- 返回值:
   - 0:  删除成功。
   - -1: 删除失败。
   
## 示例

### 初始化示例

```c
#include <aos/kv.h>

void board_yoc_init()
{

    aos_kv_init("kv");

}
```

### 获取KV项示例
```c
#include <aos/kv.h>

int ai_kv_get_vol(int type)
{
    char key[8];
    snprintf(key, sizeof(key), "vol%d", type);

    int value;
    int ret = aos_kv_getint(key, &value);
    return ret == 0?value:60;
}
```

### 设置KV项示例
```c
#include <aos/kv.h>

int ai_kv_set_vol(int type, int vol)
{
    char key[8];
    snprintf(key, sizeof(key), "vol%d", type);
    return aos_kv_setint(key, vol);
}
```

## 诊断错误码
无。

## 运行资源

无。

## 依赖资源
  - minilibc
  - aos

## 组件参考
无。