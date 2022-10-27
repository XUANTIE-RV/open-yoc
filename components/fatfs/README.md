# 概述

FatFs是一个通用的文件系统(FAT/exFAT)模块，用于在小型嵌入式系统中实现FAT文件系统。

# 示例

包含以下头文件：

```c
#include <vfs.h>
#include <devices/devicelist.h>
```

初始化：

```c
vfs_init();
int ret = vfs_fatfs_register();
if (ret != 0) {
    LOGE(TAG, "fatfs register failed(%d)", ret);
}
```

```c
int buff[512] = {0};
void read_write()
{
    int fd1 = aos_open("/fatfs0/test5.txt",  O_CREAT | O_RDWR | O_APPEND);

    aos_write(fd1, "hello world1\n", 13);

    aos_sync(fd1);

    int fd2 = aos_open("/fatfs0/test5.txt", O_RDWR);

    aos_lseek(fd2, 12, 0);

    int ret = aos_read(fd2, buff, 512);

    printf("read ret %d\n", ret);

    if (ret > 0) {
        printf("read: %s\n", buff);
    }  

    aos_close(fd1);
    aos_close(fd2);
}

void read_dir()
{
    aos_dir_t *dir = aos_opendir("/fatfs0");

    aos_dirent_t *dp = NULL;
    do {
        dp = aos_readdir(dir);
        if (dp)
            printf("readdir: %s\n", dp->d_name);
    } while (dp != NULL);

    aos_closedir(dir);
}

void main()
{
    vfs_init();
    int ret = vfs_fatfs_register();
    if (ret != 0) {
        LOGE(TAG, "fatfs register failed(%d)", ret);
    }
    read_write();
    read_dir();
}

```