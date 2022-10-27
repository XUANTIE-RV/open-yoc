/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sys/types.h>
#include <sys/stat.h>

caddr_t _sbrk (size_t incr)
{
    return (caddr_t)NULL;
}

int _write (int fd, const void *ptr, size_t len)
{
    return 0;
}

int _close(int fd)
{
    return 0;
}

off_t _lseek (int fd,  off_t offset, int whence)
{
    return 0;
}

int _read (int fd, void *ptr, size_t len)
{
    return 0;
}

