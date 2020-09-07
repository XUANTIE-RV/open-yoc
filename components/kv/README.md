# 概述

KV存储系统是YOC中一个以Key-Value方式进行持久化存储的轻量级组件，主要为基于nor flash的小型MCU设备提供通用的Key-Value持久化存储接口。该系统采用极小的代码及内存开销，支持断电保护、磨损均衡等功能。

KV存储系统支持只读模式与读写模式共存，只读模式可以用于工厂生产数据，读写模式可用于运行时的数据存储。

# 断电保护特性

断电保护的设计是为了在修改KV时，能保证KV不会被破坏的事务处理机制，即要写入失败，要么写入成功，对于已经存的KV，写失败后，KV的值仍然为旧值。对于不存在的KV，写失败后，KV不存在。

# 磨损均衡特性

在通常的应用中，部分KV被经常修改，由于FLASH物理特性，擦写的次数有一定的限制，擦写次数超过次数时，该块会损坏不能使用。磨损均衡的设计是将KV的写入分散到多 block上，避免存储在固定的位置上，达到磨损均衡的效果。磨损均衡主要依赖以下两个策略来实现：

- 异地更新策略

  Key-Value键值对采用顺序写入、异地更新的方式，即不再在原存储位置擦除重写，而是在其余空闲位置写入新键值并将原键值标记无效等待回收。这样既可以减少flash的擦除操作次数，又可以提高flash的空间利用率，也避免了对“特定”存储区块过度使用的问题。


- 垃圾回收策略

  当free block总数接近gc下限时，会触发gc操作。flash数据在gc前，存在有效键值和键值交织的情况；gc后，把有效文件数据归并到free block，原区域则被擦除并置入free block。gc循环向后搬运键值。


## 坏块处理特性

Nor flash有一定的擦写次数限制，如果达到这个限制，或者由于物理方面的损坏，会导致这些block写入有问题。KV系统采用直接片上链式读取，擦除与写入时校验数据正确性，不采用特殊的标识信息来处理坏块，当 block 擦除失败或者写失败后，会重新申请新的数据块，避免坏块被错误使用。

# 示例

包含头文件：

```c
#include <kvset.h>

static kv_t g_kv;

static int kv_flash_erase(kvblock_t *block, int pos, int size)
{
    int ret = pos + size <= block->size;
    if (ret)
        partition_erase(block->kv->handle, block->id * block->size + pos, 1);

    return ret;
}

static int kv_flash_write(kvblock_t *block, int pos, void *data, int size)
{
    int ret = pos + size <= block->size;
    if (ret)
        partition_write(block->kv->handle, block->id * block->size + pos, data, size);

    return ret;
}

static int kv_flash_read(kvblock_t *block, int pos, void *data, int size)
{
    int ret;
    ret = partition_read(block->kv->handle, block->id * block->size + pos, data, size);
    return ret;
}

static flash_ops_t flash_ops = {
    .write = kv_flash_write,
    .erase = kv_flash_erase,
    .read = kv_flash_read
};

void kv_test(const char *partition)
{
    kv_t *kv = &g_kv;
    memset(kv, 0, sizeof(kv_t));
    kv->handle = partition_open(partition);
    kv->ops    = &flash_ops;

    if (kv->handle >= 0) {
        partition_info_t *lp = hal_flash_get_info(kv->handle);
        aos_assert(lp);

        uint8_t *mem        = (uint8_t *)(lp->start_addr + lp->base_addr);
        int      block_size = lp->sector_size;
        int      block_num  = lp->length / lp->sector_size;

        kv_init(kv, mem, block_num, block_size);

        kv_set(kv, "test", "1234", 5);

        char buf[10];
        kv_get(kv, "test", buf, 5);

        printf("kv get:%s\n", buf);
    }
}

```