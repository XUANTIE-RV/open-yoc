# 概述

VFS用于为各类文件（包括设备和文件系统的文件）提供了一个统一的用户操作接口

# 设备驱动

# 文件系统

用户可以将具体的文件系统注册到 VFS 中，如 FatFS、SPIFFS 等

# 接口定义

## 初始化虚拟文件系统

```C
int vfs_init(void);
```

* 参数
  * 无
* 返回值
  * 0：成功
  * < 0：失败

## 通过路径打开文件

```C
int aos_open(const char *path, int flags);
```

* 参数
  * path: 文件或者设备的路径
  * flags: 打开操作模式
* 返回值
  * 文件描述符：成功
  * < 0：失败

* flags:

|参数       |描述   |
| --------- | -------------- |
|O_RDONLY   | 只读方式打开文件 |
|O_WRONLY   | 只写方式打开文件 |
|O_RDWR     | 以读写方式打开文件 |
|O_CREAT    | 如果要打开的文件不存在，则建立该文件 |
|O_APPEND   | 当读写文件时会从文件尾开始移动，也就是所写入的数据会以附加的方式添加到文件的尾部 |
|O_TRUNC    | 如果文件已经存在，则清空文件中的内容 |
|O_EXCL     | 如果存在指定文件，返回出错 |

## 关闭文件

```C
int aos_close(int fd);
```

* 参数
  * fd: 文件描述符
* 返回值
  * 0：成功
  * < 0：失败

## 读取文件内容

```C
ssize_t aos_read(int fd, void *buf, size_t nbytes);
```

* 参数
  * fd: 文件描述符
  * nbytes: 需要读取的字节数
  * buf: 读取到缓冲区的指针
* 返回值
  * 实际读取到的字节数
  * 0：读取数据到达文件结尾
  * < 0：失败

## 向文件写入内容

```C
ssize_t aos_write(int fd, const void *buf, size_t nbytes);
```

* 参数
  * fd: 文件描述符
  * nbytes: 需要写入的字节数
  * buf: 数据缓冲区的指针
* 返回值
  * 实际写入的字节数
  * < 0：失败

## 发送特定命令控制接口

```C
int aos_ioctl(int fd, int cmd, unsigned long arg);
```

* 参数
  * fd: 文件描述符
  * cmd: 特定控制命令
  * arg: 命令的参数
* 返回值
  * 任何返回值

## 在打开的文件描述符上执行下面描述的操作，操作由 cmd 确定

```C
int aos_fcntl(int fd, int cmd, int val);
```

* 参数
  * fd: 文件描述符
  * cmd: 文件操作命令
  * val: 依赖`cmd`的参数
* 返回值
  * 0：成功
  * < 0: 失败

## 设置下次读取文件的位置

```C
off_t aos_lseek(int fd, off_t offset, int whence);
```

* 参数
  * fd: 文件描述符
  * offset: 根据参数`whence`来移动读写位置的位移数
  * whence: SEEK_SET 参数`offset` 即为新的读写位置
  *         SEEK_CUR 以目前的读写位置往后增加`offset` 个位移量
  *         SEEK_END 将读写位置指向文件尾后再增加`offset`个位移量. 当whence 值为SEEK_CUR 或SEEK_END 时, 参数`offet`允许负值的出现.
* 返回值
  * 返回新的读写位置

## 同步文件

```C
int aos_sync(int fd);
```

* 参数
  * fd: 文件描述符
* 返回值
  * 0：成功
  * < 0: 失败

## 获取文件状态

```C
int aos_stat(const char *path, struct stat *st);
```

* 参数
  * path: 文件名
  * st: 结构指针，指向一个存放文件状态信息的结构体
* 返回值
  * 0：成功
  * < 0: 失败

## 删除指定目录下的文件

```C
int aos_unlink(const char *path);
```

* 参数
  * path: 要删除文件的路径
* 返回值
  * 0：成功
  * < 0: 失败

## 重命名文件

```C
int aos_rename(const char *oldpath, const char *newpath);
```

* 参数
  * oldpath: 旧文件名
  * newpath: 新文件名
* 返回值
  * 0：成功
  * < 0: 失败

## 打开目录

```C
aos_dir_t *aos_opendir(const char *path);
```

* 参数
  * path: 目录名
* 返回值
  * 目录流指针: 成功
  * NULL: 失败

## 关闭目录

```C
int aos_closedir(aos_dir_t *dir);
```

* 参数
  * dir: 目录流指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 读取下个目录

```C
aos_dirent_t *aos_readdir(aos_dir_t *dir);
```

* 参数
  * dir: 目录流指针
* 返回值
  * 目录流指针: 成功
  * NULL: 已读到目录尾部

## 创建目录

```C
int aos_mkdir(const char *path);
```

* 参数
  * path: 目录名
* 返回值
  * 0: 成功
  * < 0: 失败

## 删除目录

```C
int aos_rmdir(const char *path);
```

* 参数
  * path: 目录名
* 返回值
  * 0: 成功
  * < 0: 失败
  