# 虚拟文件系统（vfs）

## 概述

VFS用于为各类文件（包括设备和文件系统的文件）提供了一个统一的用户操作接口,用户可以将具体的文件系统注册到 VFS 中，如 FatFS、SPIFFS 等。


## 组件安装

```bash
yoc init
yoc install vfs
```


## 配置

无。


## 接口列表

| 函数                  | 说明                                                    |
| :-------------------- | :------------------------------------------------------ |
| vfs_init              | 初始化虚拟文件系统                                      |
| vfs_device_init       | 初始化虚拟设备驱动                                      |
| aos_open              | 通过路径打开文件                                        |
| aos_close             | 关闭文件                                                |
| aos_read              | 读取文件内容                                            |
| aos_write             | 向文件写入内容                                          |
| aos_ioctl             | 发送特定命令控制接口                                    |
| aos_fcntl             | 在打开的文件描述符上执行下面描述的操作，操作由 cmd 确定 |
| aos_lseek             | 设置下次读取文件的位置                                  |
| aos_sync              | 同步文件                                                |
| aos_stat              | 获取文件状态                                            |
| aos_unlink            | 删除指定目录下的文件                                    |
| aos_rename            | 重命名文件                                              |
| aos_opendir           | 打开目录                                                |
| aos_closedir          | 关闭目录                                                |
| aos_readdir           | 读取下个目录                                            |
| aos_mkdir             | 创建目录                                                |
| aos_rmdir             | 删除目录                                                |
| aos_register_driver   | 注册虚拟设备驱动                                        |
| aos_unregister_driver | 注销虚拟设备驱动                                        |
| aos_register_fs       | 注册虚拟文件系统                                        |
| aos_unregister_fs     | 注销虚拟文件系统                                        |


## 接口详细说明


### vfs_init

`int vfs_init(void);`

- 功能描述:
   - 初始化虚拟文件系统。

- 参数:
   - 无。

- 返回值:
   - 0：成功。
   - < 0：失败。


### vfs_device_init

`int vfs_device_init(void);`

- 功能描述:
   - 初始化虚拟设备驱动。

- 参数:
   - 无。

- 返回值:
   - 0：成功。
   - < 0：失败。


### aos_open

`int aos_open(const char *path, int flags);`

- 功能描述:
   - 通过路径打开文件。
  
- 参数:
   - `path`: 文件或者设备的路径。
   - `flags`: 打开操作模式。

- 返回值:
   - 文件描述符：成功。
   - < 0：失败。

#### flags

|参数       |描述   |
| --------- | -------------- |
|O_RDONLY   | 只读方式打开文件 |
|O_WRONLY   | 只写方式打开文件 |
|O_RDWR     | 以读写方式打开文件 |
|O_CREAT    | 如果要打开的文件不存在，则建立该文件 |
|O_APPEND   | 当读写文件时会从文件尾开始移动，也就是所写入的数据会以附加的方式添加到文件的尾部 |
|O_TRUNC    | 如果文件已经存在，则清空文件中的内容 |
|O_EXCL     | 如果存在指定文件，返回出错 |


### aos_close

`int aos_close(int fd);`

- 功能描述:
   - 关闭文件。

- 参数:
   - `fd`: 文件描述符。

- 返回值:
   - 0：成功。
   - < 0：失败。


### aos_read

`ssize_t aos_read(int fd, void *buf, size_t nbytes);`

- 功能描述:
   - 读取文件内容。

- 参数:
   - `fd`: 文件描述符。
   - `nbytes`: 需要读取的字节数。
   - `buf`: 读取到缓冲区的指针。

- 返回值:
   - 实际读取到的字节数。
   - 0：读取数据到达文件结尾。
   - < 0：失败。


### aos_write

`ssize_t aos_write(int fd, const void *buf, size_t nbytes);`

- 功能描述:
   - 向文件写入内容。

- 参数:
   - `fd`: 文件描述符。
   - `nbytes`: 需要写入的字节数。
   - `buf`: 数据缓冲区的指针。

- 返回值:
   - 实际写入的字节数。
   - < 0：失败。


### aos_ioctl

`int aos_ioctl(int fd, int cmd, unsigned long arg);`

- 功能描述:
   - 发送特定命令控制接口。

- 参数:
   - `fd`: 文件描述符。
   - `cmd`: 特定控制命令。
   - `arg`: 命令的参数。

- 返回值:
   - 任何返回值。


### aos_fcntl

`int aos_fcntl(int fd, int cmd, int val);`

- 功能描述:
   - 在打开的文件描述符上执行下面描述的操作，操作由 cmd 确定。

- 参数:
   - `fd`: 文件描述符。
   - `cmd`: 文件操作命令。
   - `val`: 依赖cmd的参数。

- 返回值:
   - 0：成功。
   - < 0: 失败。


### aos_lseek

`off_t aos_lseek(int fd, off_t offset, int whence);`

- 功能描述:
   - 设置下次读取文件的位置。

- 参数:
   - `fd`: 文件描述符。
   - `offset`: 根据参数 whence 来移动读写位置的位移数。
   - `whence`: SEEK_SET 参数 offset  即为新的读写位置；SEEK_CUR 以目前的读写位置往后增加`offset` 个位移量；SEEK_END 将读写位置指向文件尾后再增加`offset`个位移量. 当whence 值为SEEK_CUR 或SEEK_END 时, 参数`offet`允许负值的出现。

- 返回值:
   - 返回新的读写位置。


### aos_sync

`int aos_sync(int fd);`

- 功能描述:
   - 同步文件。

- 参数:
   - `fd`: 文件描述符。

- 返回值:
   - 0：成功。
   - < 0: 失败。


### aos_stat

`int aos_stat(const char *path, aos_stat_t *st);`

- 功能描述:
   - 获取文件状态。

- 参数:
   - `path`: 文件名。
   - `st`: 结构指针，指向一个存放文件状态信息的结构体。

- 返回值:
   - 0：成功。
   - < 0: 失败。


### aos_unlink

`int aos_unlink(const char *path);`

- 功能描述:
   - 删除指定目录下的文件。

- 参数:
   - `path`: 要删除文件的路径。

- 返回值:
   - 0：成功。
   - < 0: 失败。


### aos_rename

`int aos_rename(const char *oldpath, const char *newpath);`

- 功能描述:
   - 重命名文件。

- 参数:
   - `oldpath`: 旧文件名。
   - `newpath`: 新文件名。

- 返回值:
   - 0：成功。
   - < 0: 失败。


### aos_opendir

`aos_dir_t *aos_opendir(const char *path);`

- 功能描述:
   - 打开目录。

- 参数:
   - `path`: 目录名。

- 返回值:
   - 目录流指针: 成功。
   - NULL: 失败。


### aos_closedir

`int aos_closedir(aos_dir_t *dir);`

- 功能描述:
   - 关闭目录。

- 参数:
   - `dir`: 目录流指针。

- 返回值:
   - 0: 成功
   - < 0: 失败


### aos_readdir

`aos_dirent_t *aos_readdir(aos_dir_t *dir);`

- 功能描述:
   - 读取下个目录。

- 参数:
   - `dir`: 目录流指针。

- 返回值:
   - 目录流指针: 成功。
   - NULL: 已读到目录尾部。


### aos_mkdir

`int aos_mkdir(const char *path);`

- 功能描述:
   - 创建目录。

- 参数:
   - `path`: 目录名。

- 返回值:
   - 0: 成功。
   - < 0: 失败。


### aos_rmdir

`int aos_rmdir(const char *path);`

- 功能描述:
   - 删除目录。

- 参数:
   - `path`: 目录名。

- 返回值:
   - 0: 成功。
   - < 0: 失败。
  

### aos_register_driver

`int aos_register_driver(const char *path, file_ops_t *fops, void *arg);`

- 功能描述:
   - 注册虚拟设备驱动。

- 参数:
   - `path`: 目录名。
   - `fops`：结构指针，指向一个设备驱动操作的结构体。
   - `arg`：命令的参数。

- 返回值:
   - 0: 成功。
   - < 0: 失败。


### aos_unregister_driver

`int aos_unregister_driver(const char *path);`

- 功能描述:
   - 注销虚拟设备驱动。

- 参数:
   - `path`: 目录名。

- 返回值:
   - 0: 成功。
   - < 0: 失败。


### aos_register_fs

`int aos_register_fs(const char *path, fs_ops_t *fops, void *arg);`

- 功能描述:
   - 注册虚拟文件系统。

- 参数:
   - `fops`：结构指针，指向一个文件系统操作的结构体。
   - `arg`：命令的参数。
   - `path`: 目录名。

- 返回值:
   - 0: 成功。
   - < 0: 失败。


### aos_unregister_fs

`int aos_unregister_fs(const char *path);`

- 功能描述:
   - 注销虚拟文件系统。

- 参数:
   - `path`: 目录名。

- 返回值:
   - 0: 成功。
   - < 0: 失败。


## 示例

### 文件读写

```C
    int file()
    {
        char *buffer;
        const char *filepath
        int fd;
        int ret = 0;
    /* Open the file for reading */
    
    fd = aos_open(filepath, O_RDONLY);
    
    if (fd < 0) {
        printf(g_fmtcmdfailed, cmd, "open", fd);
        return -1;
    }
    
    buffer = aos_malloc(IOBUFFERSIZE);
    
    if (buffer == NULL) {
        (void)aos_close(fd);
        printf(g_fmtcmdfailed, cmd, "malloc", -1);
        return -1;
    }
    
    /* And just dump it byte for byte into stdout */
    
    for (;;) {
        int nbytesread = aos_read(fd, buffer, IOBUFFERSIZE);
    
        /* Check for read errors */
    
        if (nbytesread < 0) {
            //int errval = errno;
    
            /* EINTR is not an error (but will stop stop the cat) */
    
            printf(g_fmtcmdfailed, cmd, "read", errno);
    
            ret = -1;
            break;
        }
    
        /* Check for data successfully read */
    
        else if (nbytesread > 0) {
    
            printf("%.*s", nbytesread, buffer);
        }
    
        /* Otherwise, it is the end of file */
    
        else {
            break;
        }
    }
    printf("\n");
    /* Close the input file and return the result */
    (void)aos_close(fd);
    aos_free(buffer);
    return ret;
}
```


### 目录操作

```C
    int direntry( )
    {
        aos_dir_t *dirp;
        int ret = 0;
        char *dirpath
    /* Open the directory */
    
    dirp = aos_opendir(dirpath);
    
    if (dirp == NULL) {
        /* Failed to open the directory */
    
        printf(g_fmtnosuch, cmd, "directory", dirpath);
        return -1;
    }
    
    /* Read each directory entry */
    
    for (; ;) {
        aos_dirent_t *entryp = aos_readdir(dirp);
    
        if (entryp == NULL) {
            /* Finished with this directory */
    
            break;
        }
    
        /* Call the handler with this directory entry */
    
        if (handler(dirpath, entryp, pvarg) <  0) {
            /* The handler reported a problem */
    
            ret = -1;
            break;
        }
    }
    
    aos_closedir(dirp);
    return ret;
}
```

### 注册虚拟设备驱动

```C   
    int vfs_device_init(void)
    {
        int ret;
        ret = aos_register_driver("/dev/event", &event_fops, NULL);
    
        if (ret != VFS_SUCCESS) {
        return ret;
        }
        return VFS_SUCCESS;
    }
```


## 诊断错误码
无


## 运行资源
无。


## 依赖资源
  - csi
  - aos


## 组件参考
无。

