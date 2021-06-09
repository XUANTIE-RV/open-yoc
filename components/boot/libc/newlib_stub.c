/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if CONFIG_NEWLIB_STUB
#include <reent.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/time.h>
#include <stdarg.h>

int _execve_r(struct _reent *ptr, const char *name, char *const *argv,
              char *const *env)
{
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    return -1;
}

int _getpid_r(struct _reent *ptr)
{
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    return -1;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    return -1;
}

int _link_r(struct _reent *ptr, const char *old, const char *new)
{
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    return -1;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    return -1;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    return -1;
}

int _close_r(struct _reent *ptr, int fd)
{
    return -1;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    return -1;
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    return NULL;
}
#endif