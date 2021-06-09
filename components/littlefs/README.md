# 小型文件系统（littlefs）

## 概述

一个为微控制器设计的小故障安全文件系统。

- 掉电恢复能力: 设计用于处理随机电源故障。所有文件操作都有很强的写时拷贝保证，如果断电，文件系统将恢复到上一次已知的良好状态。

- 动态磨损均衡: 设计考虑到闪存，并提供动态块磨损均衡。此外，littlefs可以检测坏块并在它们周围工作。

- 有限RAM/ROM: 被设计为使用少量内存。RAM的使用是严格限制的，这意味着RAM的使用不会随着文件系统的增长而改变。文件系统不包含无界递归，动态内存仅限于可静态提供的可配置缓冲区。

在lfs.h的评论中可以找到详细的文档（或者至少是目前可用的尽可能多的细节）。
littlefs采用了一种配置结构，定义了文件系统的运行方式。配置结构为文件系统提供了块设备操作和维度、可调整的参数（用于在性能上权衡内存使用情况）以及可选的静态缓冲区（如果用户希望避免动态内存）。
littlefs的状态存储在lfs\t类型中，由用户分配，允许同时使用多个文件系统。使用lfs\u t和configuration struct，用户可以格式化块设备或挂载文件系统。
挂载后，littlefs提供了一整套类似POSIX的文件和目录功能，但文件系统结构的分配必须由用户提供。
所有POSIX操作，比如remove和rename，都是原子的，即使在断电的情况下也是如此。此外，在对文件调用sync或close之前，文件更新实际上不会提交到文件系统。

## 组件安装

```bash
yoc init
yoc install littlefs
```

## 配置

```c
// Maximum name size in bytes, may be redefined to reduce the size of the
// info struct. Limited to <= 1022. Stored in superblock and must be
// respected by other littlefs drivers.
#define LFS_NAME_MAX 255

// Maximum size of a file in bytes, may be redefined to limit to support other
// drivers. Limited on disk to <= 4294967296. However, above 2147483647 the
// functions lfs_file_seek, lfs_file_size, and lfs_file_tell will return
// incorrect values due to using signed integers. Stored in superblock and
// must be respected by other littlefs drivers.
#define LFS_FILE_MAX 2147483647

// Maximum size of custom attributes in bytes, may be redefined, but there is
// no real benefit to using a smaller LFS_ATTR_MAX. Limited to <= 1022.
#define LFS_ATTR_MAX 1022
```

## 接口列表

littlefs接口如下所示：

| 函数 | 说明 |
| :--- | :--- |
| lfs_format | 用littlefs格式化块设备 |
| lfs_mount | 挂载文件系统 |
| lfs_unmount | 卸载文件系统 |
| lfs_remove | 删除文件系统 |
| lfs_rename | 重命名或移动文件或目录 |
| lfs_stat | 查找有关文件或目录的信息 |
| lfs_fstat | 查找有关文件的信息 |
| lfs_getattr | 获取自定义属性 |
| lfs_setattr | 设置自定义属性 |
| lfs_removeattr | 删除自定义属性 |
| lfs_file_open | 打开一个文件 |
| lfs_file_opencfg | 打开具有额外配置的文件 |
| lfs_file_close |  关闭文件 |
| lfs_file_sync | 同步存储上的文件 |
| lfs_file_read | 从文件读取数据 |
| lfs_file_write | 将数据写入文件 |
| lfs_file_seek | 更改文件的位置 |
| lfs_file_truncate | 将文件的大小截断为指定的大小 |
| lfs_file_tell | 返回文件的位置 |
| lfs_file_rewind | 将文件的位置更改为文件的开头 |
| lfs_file_size | 返回文件的大小 |
| lfs_mkdir | 创建目录 |
| lfs_dir_open | 打开目录 |
| lfs_dir_close | 关闭目录 |
| lfs_dir_read | 读取目录中的条目 |
| lfs_dir_seek | 更改目录的位置 |
| lfs_dir_tell | 返回目录的位置 |
| lfs_dir_rewind | 将目录的位置更改为目录的开头 |
| lfs_fs_size | 查找文件系统的当前大小 |
| lfs_fs_traverse | 遍历文件系统正在使用的所有块 |
| lfs_migrate | 尝试迁移以前版本的littlefs |

## 接口详细说明

### 枚举定义

#### lfs_type
| 类型 | 说明 |
| :--- | :--- |
| LFS_TYPE_REG | file types |
| LFS_TYPE_DIR | file types |
| LFS_TYPE_SPLICE | internally used types |
| LFS_TYPE_NAME | internally used types |
| LFS_TYPE_STRUCT | internally used types |
| LFS_TYPE_USERATTR | internally used types |
| LFS_TYPE_FROM | internally used types |
| LFS_TYPE_TAIL | internally used types |
| LFS_TYPE_GLOBALS | internally used types |
| LFS_TYPE_CRC | internally used types |
| LFS_TYPE_CREATE | internally used type specializations |
| LFS_TYPE_DELETE | internally used type specializations |
| LFS_TYPE_SUPERBLOCK | internally used type specializations |
| LFS_TYPE_DIRSTRUCT | internally used type specializations |
| LFS_TYPE_CTZSTRUCT | internally used type specializations |
| LFS_TYPE_INLINESTRUCT |internally used type specializations  |
| LFS_TYPE_SOFTTAIL | internally used type specializations |
| LFS_TYPE_HARDTAIL |internally used type specializations  |
| LFS_TYPE_MOVESTATE | internally used type specializations |
| LFS_FROM_NOOP | internal chip sources |
| LFS_FROM_MOVE | internal chip sources |
| LFS_FROM_USERATTRS | internal chip sources |

#### lfs_open_flags
| 类型 | 说明 |
| :--- | :--- |
| LFS_O_RDONLY | Open a file as read only |
| LFS_O_WRONLY | Open a file as write only |
| LFS_O_RDWR | Open a file as read and write |
| LFS_O_CREAT | Create a file if it does not exist |
| LFS_O_EXCL | Fail if a file already exists |
| LFS_O_TRUNC | Truncate the existing file to zero size |
| LFS_O_APPEND | Move to end of file on every write |
| LFS_F_DIRTY | File does not match storage |
| LFS_F_WRITING | File has been written since last flush |
| LFS_F_READING | File has been read since last flush |
| LFS_F_ERRED | An error occured during write |
| LFS_F_INLINE | Currently inlined in directory entry |
| LFS_F_OPENED | File has been opened |

#### lfs_whence_flags
| 类型 | 说明 |
| :--- | :--- |
| LFS_SEEK_SET | Seek relative to an absolute position |
| LFS_SEEK_CUR | Seek relative to the current file position |
| LFS_SEEK_END | Seek relative to the end of the file |

### 结构体定义

#### lfs_config

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| context | void* | 传递给block驱动代码的上下文 |
| read | int32_t | 从设备读数据 |
| prog | int32_t | 向设备写入数据，block设备在写入前必须已经erase了 |
| erase | int32_t | 擦除block |
| sync | int32_t | sync块设备的状态 |
| read_size | lfs_size_t | 最小的读取单元大小  |
| prog_size | lfs_size_t | 最小的写入数据单元大小，也是数据metadata pair中tag的对齐尺寸 |
| block_size | lfs_size_t | 最小的擦除单元大小。可以比flash的实际block尺寸大。但是对于ctz类型的文件，block size是最小的分配单元。同时block size必须是read size和program size的倍数，block size会存储在superblock中 |
| block_count | lfs_size_t | 属于文件系统的block数量，block count会存储在superblock中 |
| block_cycles | int32_t | 文件系统进行垃圾回收时的block的擦除次数，推荐取值100-1000.值越大垃圾回收的次数越少，性能越好 |
| cache_size | lfs_size_t | littlefs需要一个read cache，一个program cache，每个文件也需要一个cache。cache越大性能越好，会减少会flash的访问次数，cache必须是block的read size和program size的倍数，同时是block size的因数 |
| lookahead_size | lfs_size_t | lookahead buffer的尺寸。lookahead buffer主要是block alloctor在分配块的时候用到。lookahead size必须是8的倍数，因为它是采用bitmap的形式存储的 |
| read_buffer | void* | cache size大小的read buffer，可以静态分配也可以动态分配 |
| prog_buffer | void* | cache size大小的program buffer，可以静态分配也可以动态分配 |
| lookahead_buffer | void* | lookahead_size大小的lookahead buffer，且是32-bit对齐的，即可以静态分配也可以动态分配  |
| name_max | lfs_size_t | 文件名的最大长度，这个值会存储在superblock中 |
| file_max | lfs_size_t | 文件的最大长度，存储在superblock中 |
| attr_max | lfs_size_t | 用户属性的最大长度 |

#### lfs_info

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| type | uint8_t | 文件类型, either LFS_TYPE_REG or LFS_TYPE_DIR |
| size | lfs_size_t | 文件大小, only valid for REG files,Limited to 32-bits |
| name | char | 文件名 |

#### lfs_attr
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| type | uint8_t | 8位属性类型，由用户提供，用于标识属性 |
| buffer | void* | 指向包含属性的缓冲区的指针 |
| size | lfs_size_t | 以字节为单位的属性大小，限制为LFS_ATTR_MAX |

#### lfs_file_config
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| buffer | void* | cache size长度的buffer，可以静态分配也可以动态分配 |
| attrs | struct lfs_attr* | 用户属性，读文件时，attr存储从flash上读取的文件用户属性，写入文件时，attr存放用户指定的文件属性并会写入到flash中 |
| attr_count | lfs_size_t | 用户属性的长度 |

#### lfs_cache_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| block | lfs_block_t | cache中的数据属于的block |
| off | lfs_off_t | cache中的数据在block上的偏移地址 |
| size | lfs_size_t | cache的大小 |
| buffer | uint8_t* | cache数据的存放地址 |

#### lfs_mdir_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| pair | lfs_block_t | dir的metadata pair所在的block |
| rev | uint32_t | metadata pair的revision |
| off | lfs_off_t | tag的偏移地址 |
| etag | uint32_t | tag标志 |
| count | uint16_t | 计数 |
| erased | bool | 擦除标志 |
| split | bool | metadata pair是否是链表 |
| tail | lfs_block_t | 用于metadata pair的链表 |

#### lfs_dir_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| next | struct lfs_dir* | 指向下一个结点 |
| id | uint16_t | 编号 |
| type | uint8_t | 类型 |
| m | lfs_mdir_t | 目录 |
| pos | lfs_off_t | 位置 |
| head | lfs_block_t | 块头内容 |

#### lfs_ctz
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| head | lfs_block_t | 当前块头指针地址 |
| size | lfs_size_t | 块大小 |

#### lfs_file_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| next | struct lfs_file* | 指向下一个节点 |
| id | uint16_t | metadata tag中的id，在文件open时获取 |
| type | uint8_t | LFS_TYPE_REG 或者 LFS_TYPE_DIR |
| m | lfs_mdir_t | 文件所在的目录的metadata pair |
| ctz | struct lfs_ctz | 指向大文件的CTZ skip-list。对于小文件则直接inline了，无需CTZ skip-list |
| flags | uint32_t | lfs_open_flags中的值 |
| pos | lfs_off_t | 文件访问时的偏移 |
| block | lfs_block_t | file当前的block |
| off | lfs_off_t | 在block内的offset |
| cache | lfs_cache_t | 文件访问时的cache |
| cfg | const struct lfs_file_config* | 文件open时的配置参数，包含一个buffer以及用户属性 |

#### lfs_superblock_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| version | uint32_t | 版本内容 |
| block_size | lfs_size_t | 块大小 |
| block_count | lfs_size_t | 块计数 |
| name_max | lfs_size_t | 名称最大长度 |
| file_max | lfs_size_t | 文件最大长度 |
| attr_max | lfs_size_t | 属性最大长度 |

#### lfs_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| rcache | lfs_cache_t | 读缓存 |
| pcache | lfs_cache_t | 程序缓存 |
| root | lfs_block_t | 根目录所在块 |
| mlist | struct lfs_mlist* | 元数据对链表 |
| seed | uint32_t | block alloctor的随机数生成的种子 |
| gstate | struct lfs_gstate | 用于目录操作sync的global state |
| gpending | struct lfs_gstate | 用于目录操作sync的global pending |
| gdelta | struct lfs_gstate | 用于目录操作sync的global delta |
| free | struct lfs_free |  lookahead buffer，用于分配free block |
| cfg | const struct lfs_config* | 文件系统的配置参数 |
| name_max | lfs_size_t | 文件名的最大长度，和superblock中的name_max值相同 |
| file_max | lfs_size_t | 文件的最大长度，和superblock中的file_max值相同 |
| attr_max | lfs_size_t | 用户属性的最大长度，和superblock中的attr_max值相同 |
| lfs1 | struct lfs1* | 结构体 |

接口的具体形式如下：

### lfs_format
`int lfs_format(lfs_t *lfs, const struct lfs_config *config);`

- 功能描述:
   - 用littlefs格式化块设备。

- 参数:
   - `lfs`: 文件系统句柄。
   - `config`: 配置参数结构体。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_mount
`int lfs_mount(lfs_t *lfs, const struct lfs_config *config);`

- 功能描述:
   - 挂载文件系统。

- 参数:
   - `lfs`: 文件系统句柄。
   - `config`: 配置参数。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_unmount
`int lfs_unmount(lfs_t *lfs);`

- 功能描述:
   - 卸载文件系统。

- 参数:
   - `lfs`: 文件系统句柄。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_remove
`int lfs_remove(lfs_t *lfs, const char *path);`

- 功能描述:
   - 删除文件或目录。

- 参数:
   - `lfs`: 文件系统句柄。
   - `path`: 路径。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_rename
`int lfs_rename(lfs_t *lfs, const char *oldpath, const char *newpath);`

- 功能描述:
   - 重命名或移动文件或目录。

- 参数:
   - `lfs`: 文件系统句柄。
   - `oldpath`: 原路径。
   - `newpath`: 新路径。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_stat
`int lfs_stat(lfs_t *lfs, const char *path, struct lfs_info *info);`

- 功能描述:
   - 查找有关文件或目录的信息。

- 参数:
   - `lfs`: 文件系统句柄。
   - `path`: 路径。
   - `info`: 信息。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_fstat
`int lfs_fstat(lfs_t *lfs, lfs_file_t *file, struct lfs_info *info);`

- 功能描述:
   - 查找有关文件的信息。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。
   - `info`: 信息。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_getattr
`lfs_ssize_t lfs_getattr(lfs_t *lfs, const char *path, uint8_t type, void *buffer, lfs_size_t size);`

- 功能描述:
   - 获取自定义属性。

- 参数:
   - `lfs`: 文件系统句柄。
   - `path`: 路径。
   - `type`: 文件类型。
   - `buffer`: 缓冲区的指针。
   - `size`: 文件大小。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_setattr
`int lfs_setattr(lfs_t *lfs, const char *path, uint8_t type, const void *buffer, lfs_size_t size);`

- 功能描述:
   - 设置自定义属性。

- 参数:
   - `lfs`: 文件系统句柄。
   - `path`: 路径。
   - `type`: 文件类型。
   - `buffer`: 缓冲区的指针。
   - `size`: 文件大小。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_removeattr
`int lfs_removeattr(lfs_t *lfs, const char *path, uint8_t type);`

- 功能描述:
   - 删除自定义属性。

- 参数:
   - `lfs`: 文件系统句柄。
   - `path`: 路径。
   - `type`: 文件类型。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_open
`int lfs_file_open(lfs_t *lfs, lfs_file_t *file, const char *path, int flags);`

- 功能描述:
   - 打开一个文件。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。
   - `path`: 路径。
   - `flags`: 文件标志。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_opencfg
`int lfs_file_opencfg(lfs_t *lfs, lfs_file_t *file, const char *path, int flags, const struct lfs_file_config *config);`

- 功能描述:
   - 打开具有额外配置的文件。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。
   - `path`: 路径。
   - `flags`: 标志。
   - `config`: 配置参数。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_close
`int lfs_file_close(lfs_t *lfs, lfs_file_t *file);`

- 功能描述:
   - 关闭文件。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_sync
`int lfs_file_sync(lfs_t *lfs, lfs_file_t *file);`

- 功能描述:
   - 同步存储上的文件。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_read
`lfs_ssize_t lfs_file_read(lfs_t *lfs, lfs_file_t *file, void *buffer, lfs_size_t size);`

- 功能描述:
   - 从文件读取数据。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。
   - `buffer`: 读出的数据缓存。
   - `size`: 读出的数据大小。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_write
`lfs_ssize_t lfs_file_write(lfs_t *lfs, lfs_file_t *file, const void *buffer, lfs_size_t size);`

- 功能描述:
   - 将数据写入文件。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。
   - `buffer`: 写入数据的缓存。
   - `size`: 写入数据大小。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_seek
`lfs_soff_t lfs_file_seek(lfs_t *lfs, lfs_file_t *file, lfs_soff_t off, int whence);`

- 功能描述:
   - 更改文件的位置。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。
   - `off`: 偏移量。
   - `whence`: 位置标志。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_truncate
`int lfs_file_truncate(lfs_t *lfs, lfs_file_t *file, lfs_off_t size);`

- 功能描述:
   - 将文件的大小截断为指定的大小。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。
   - `size`: 文件大小。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_tell
`lfs_soff_t lfs_file_tell(lfs_t *lfs, lfs_file_t *file);`

- 功能描述:
   - 返回文件的位置。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_rewind
`int lfs_file_rewind(lfs_t *lfs, lfs_file_t *file);`

- 功能描述:
   - 将文件的位置更改为文件的开头。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_file_size
`lfs_soff_t lfs_file_size(lfs_t *lfs, lfs_file_t *file);`

- 功能描述:
   - 返回文件的大小。

- 参数:
   - `lfs`: 文件系统句柄。
   - `file`: 文件。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_mkdir
`int lfs_mkdir(lfs_t *lfs, const char *path);`

- 功能描述:
   - 创建目录。

- 参数:
   - `lfs`: 文件系统句柄。
   - `path`: 路径。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_dir_open
`int lfs_dir_open(lfs_t *lfs, lfs_dir_t *dir, const char *path);`

- 功能描述:
   - 打开目录。

- 参数:
   - `lfs`: 文件系统句柄。
   - `dir`: 目录。
   - `path`: 路径。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_dir_close
`int lfs_dir_close(lfs_t *lfs, lfs_dir_t *dir);`

- 功能描述:
   - 关闭目录。

- 参数:
   - `lfs`: 文件系统句柄。
   - `dir`: 目录。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_dir_read
`int lfs_dir_read(lfs_t *lfs, lfs_dir_t *dir, struct lfs_info *info);`

- 功能描述:
   - 读取目录中的条目。

- 参数:
   - `lfs`: 文件系统句柄。
   - `dir`: 目录。
   - `info`: 信息。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_dir_seek
`int lfs_dir_seek(lfs_t *lfs, lfs_dir_t *dir, lfs_off_t off);`

- 功能描述:
   - 更改目录的位置。

- 参数:
   - `lfs`: 文件系统句柄。
   - `dir`: 目录。
   - `off`: 偏移量。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_dir_tell
`lfs_soff_t lfs_dir_tell(lfs_t *lfs, lfs_dir_t *dir);`

- 功能描述:
   - 返回目录的位置。

- 参数:
   - `lfs`: 文件系统句柄。
   - `dir`: 目录。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_dir_rewind
`int lfs_dir_rewind(lfs_t *lfs, lfs_dir_t *dir);`

- 功能描述:
   - 将目录的位置更改为目录的开头。

- 参数:
   - `lfs`: 文件系统句柄。
   - `dir`: 目录。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_fs_size
`lfs_ssize_t lfs_fs_size(lfs_t *lfs);`

- 功能描述:
   - 查找文件系统的当前大小。

- 参数:
   - `lfs`: 文件系统句柄。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_fs_traverse
`int lfs_fs_traverse(lfs_t *lfs, int (*cb)(void*, lfs_block_t), void *data);`

- 功能描述:
   - 遍历文件系统正在使用的所有块。

- 参数:
   - `lfs`: 文件系统句柄。
   - `cb`: 回调。
   - `data`: 数据。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

### lfs_migrate
`int lfs_migrate(lfs_t *lfs, const struct lfs_config *cfg);`

- 功能描述:
   - 尝试迁移以前版本的littlefs。

- 参数:
   - `lfs`: 文件系统句柄。
   - `cfg`: 配置信息。

- 返回值:
   - 0: 成功 。
   - <0: 失败。

## 示例

Here's a simple example that updates a file named boot_count every time main runs. The program can be interrupted at any time without losing track of how many times it has been booted and without corrupting the filesystem:
```c
#include "lfs.h"

// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = 128,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};

// entry point
int main(void) {
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    printf("boot_count: %d\n", boot_count);
}
```

## 诊断错误码
| 错误码 | 错误码说明 |
| :--- | :--- |
| LFS_ERR_OK | No error |
| LFS_ERR_IO | Error during device operation |
| LFS_ERR_CORRUPT | Corrupted |
| LFS_ERR_NOENT | No directory entry |
| LFS_ERR_EXIST | Entry already exists |
| LFS_ERR_NOTDIR | Entry is not a dir |
| LFS_ERR_ISDIR | Entry is a dir |
| LFS_ERR_NOTEMPTY | Dir is not empty |
| LFS_ERR_BADF | Bad file number |
| LFS_ERR_FBIG | File too large |
| LFS_ERR_INVAL | Invalid parameter |
| LFS_ERR_NOSPC | No space left on device |
| LFS_ERR_NOMEM | No more memory available |
| LFS_ERR_NOATTR | No data/attr available |
| LFS_ERR_NAMETOOLONG | File name too long |

## 运行资源
无。

## 依赖资源

  - csi
  - aos

## 组件参考
无。
