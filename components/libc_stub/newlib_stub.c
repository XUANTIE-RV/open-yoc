/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <aos/kernel.h>
#if defined(CONFIG_KERNEL_RHINO)
#include <k_api.h>
#endif
#if defined(CONFIG_TCPIP) || defined(CONFIG_SAL)
#include <sys/socket.h>
#ifdef TELNETD_ENABLED
#include "lwip/apps/telnetserver.h"
#endif
#endif
#ifdef AOS_COMP_VFS
#include <vfs.h>
#include <vfs_conf.h>
extern int aos_vfs_fcntl(int fd, int cmd, int val);
#else
#define VFS_FD_OFFSET 48
#define VFS_MAX_FILE_NUM 50
#endif /* AOS_COMP_VFS */
#define FD_VFS_START VFS_FD_OFFSET
#define FD_VFS_END   (FD_VFS_START + VFS_MAX_FILE_NUM - 1)

#if defined(CONFIG_TCPIP) || defined(CONFIG_SAL)
#include "lwipopts.h"
#define FD_AOS_SOCKET_OFFSET (LWIP_SOCKET_OFFSET)
#define FD_AOS_NUM_SOCKETS (MEMP_NUM_NETCONN)
#define FD_AOS_NUM_EVENTS  FD_AOS_NUM_SOCKETS
#define FD_AOS_EVENT_OFFSET (FD_AOS_SOCKET_OFFSET + FD_AOS_NUM_SOCKETS + 4)

#define FD_SOCKET_START FD_AOS_SOCKET_OFFSET
#define FD_SOCKET_END   (FD_AOS_SOCKET_OFFSET + FD_AOS_NUM_SOCKETS - 1)
#define FD_EVENT_START  FD_AOS_EVENT_OFFSET
#define FD_EVENT_END    (FD_AOS_EVENT_OFFSET + FD_AOS_NUM_EVENTS - 1)
#endif

#define LIBC_CHECK_AOS_RET(ret) do {if ((ret) < 0) {ptr->_errno = -(ret); return -1; } } while (0)

extern uint64_t aos_calendar_time_get(void);

int _execve_r(struct _reent *ptr, const char *name, char *const *argv,
              char *const *env)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    int ret = -1;

    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
#ifdef AOS_COMP_VFS
        ret = aos_fcntl(fd, cmd, arg);
        LIBC_CHECK_AOS_RET(ret);
#endif
        return ret;
#if defined(CONFIG_TCPIP) || defined(CONFIG_SAL)
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
#if defined(CONFIG_SAL)
        extern int sal_fcntl(int s, int cmd, ...);
        ret = sal_fcntl(fd, cmd, arg);
#else
        ret = lwip_fcntl(fd, cmd, arg);
#endif
        return ret;
#endif
    } else {
        ptr->_errno = EBADF;
        return -1;
    }
}

int _fork_r(struct _reent *ptr)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _getpid_r(struct _reent *ptr)
{
    ptr->_errno = ENOSYS;
    return 0;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    if (fd >= 0 && fd < 3) {
        return 1;
    }

    ptr->_errno = ENOTTY;
    return 0;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    ptr->_errno = ENOSYS;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    int ret = -1;
#ifdef AOS_COMP_VFS
    ret = aos_lseek(fd, pos, whence);
#endif
    if (ret < 0) {
        ptr->_errno = -ret;
        ret = -1;
    }
    return ret;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    int ret = -1;
#ifdef AOS_COMP_VFS
    ret = aos_mkdir(name);
#endif
    if (ret < 0) {
        ptr->_errno = -ret;
        ret = -1;
    }
    return ret;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    int ret = -1;
#ifdef AOS_COMP_VFS
    ret = aos_open(file, flags);
#endif
    if (ret < 0) {
        ptr->_errno = -ret;
        ret = -1;
    }
    return ret;
}

int _close_r(struct _reent *ptr, int fd)
{
    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
#ifdef AOS_COMP_VFS
        return aos_close(fd);
#else
        return -1;
#endif
#if defined(CONFIG_TCPIP) || defined(CONFIG_SAL)
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
        return lwip_close(fd);
#endif
    } else {
        ptr->_errno = EBADF;
        return -1;
    }
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
#ifdef AOS_COMP_VFS
        return aos_read(fd, buf, nbytes);
#else
        return -1;
#endif
#if defined(CONFIG_TCPIP) || defined(CONFIG_SAL)
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
#if defined(CONFIG_SAL)
        return recv(fd, buf, nbytes, 0);
#else
        return lwip_read(fd, buf, nbytes);
#endif
#endif
    } else {
        ptr->_errno = EBADF;
        return -1;
    }
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    int ret = -1;

    if (buf == NULL) {
        return 0;
    }

    if ((fd >= FD_VFS_START) && (fd <= FD_VFS_END)) {
#ifdef AOS_COMP_VFS
        ret = aos_write(fd, buf, nbytes);
#endif
        if (ret < 0) {
            ret = -1;
        }
        return ret;
#if defined(CONFIG_TCPIP) || defined(CONFIG_SAL)
    } else if ((fd >= FD_SOCKET_START) && (fd <= FD_EVENT_END)) {
#if defined(CONFIG_SAL)
        return send(fd, buf, nbytes, 0);
#else
        return lwip_write(fd, buf, nbytes);
#endif
#endif
    } else if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        extern int uart_write(const void *buf, size_t size);
        uart_write(buf, nbytes);
        return nbytes;
    } else {
        return -1;
    }
}

int ioctl(int fildes, int request, ... /* arg */)
{
    va_list args;
    int ret = -1;

    va_start(args, request);

    if ((fildes >= FD_VFS_START) && (fildes <= FD_VFS_END)) {
#ifdef AOS_COMP_VFS
        unsigned long arg = 0;
        arg = va_arg(args, unsigned long);
        ret = aos_ioctl(fildes, request, arg);
        va_end(args);
#endif
        return ret;
#if defined(CONFIG_TCPIP) || defined(CONFIG_SAL)
    } else if ((fildes >= FD_AOS_SOCKET_OFFSET) &&
               (fildes <= (FD_AOS_EVENT_OFFSET + FD_AOS_NUM_EVENTS - 1))) {
#if defined(CONFIG_TCPIP)
        void *argp = NULL;
        argp = va_arg(args, void *);
        ret = lwip_ioctl(fildes, request, argp);
        va_end(args);
#else
        ret = -1;
#endif
        return ret;
#endif
    } else {
        va_end(args);
        return -1;
    }
}

int _rename_r(struct _reent *ptr, const char *oldname, const char *newname)
{
    int ret = -1;

#ifdef AOS_COMP_VFS
    ret = aos_rename(oldname, newname);
#endif
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    ptr->_errno = ENOSYS;
    return NULL;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    int ret = -1;

    if ((file == NULL) || (pstat == NULL)) {
        ptr->_errno = EINVAL;
        return -1;
    }
#ifdef AOS_COMP_VFS
    aos_stat_t stat;
    ret = aos_stat(file, &stat);
    LIBC_CHECK_AOS_RET(ret);
    pstat->st_mode  = stat.st_mode;
    pstat->st_size  = stat.st_size;
    pstat->st_atime = stat.st_actime;
    pstat->st_mtime = stat.st_modtime;
#endif

    return ret;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *buf)
{
    int ret = -1;

    if ((fd < 3) || (buf == NULL)) {
        ptr->_errno = EINVAL;
        return -1;
    }
#ifdef AOS_COMP_VFS
    aos_stat_t stat_temp;
    ret = aos_fstat(fd, &stat_temp);
    LIBC_CHECK_AOS_RET(ret);
    buf->st_mode = stat_temp.st_mode;
    buf->st_size = stat_temp.st_size;
    buf->st_atime = stat_temp.st_actime;
    buf->st_mtime = stat_temp.st_modtime;
#endif

    return ret;
}

_CLOCK_T_ _times_r(struct _reent *ptr, struct tms *ptms)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _link_r(struct _reent *ptr, const char *oldpath, const char *newpath)
{
    int ret = -1;

#ifdef AOS_COMP_VFS
    ret = aos_link(oldpath, newpath);
#endif
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    int ret = -1;

#ifdef AOS_COMP_VFS
    ret = aos_unlink(file);
#endif
    LIBC_CHECK_AOS_RET(ret);
    return ret;
}

int _wait_r(struct _reent *ptr, int *status)
{
    ptr->_errno = ENOSYS;
    return -1;
}

int _gettimeofday_r(struct _reent *ptr, struct timeval *tv, void *__tzp)
{
    uint64_t t;
    struct timezone *tz = __tzp;

    if (tv) {
        t = aos_calendar_time_get();
        tv->tv_sec  = t / 1000;
        tv->tv_usec = (t % 1000) * 1000;
    }

    if (tz) {
        /* Not supported. */
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }

    return 0;
}

long timezone = 8; /* default CTS */

struct tm* localtime_r(const time_t* t, struct tm* r)
{
    time_t time_tmp;
    time_tmp = *t + timezone * 3600;
    return gmtime_r(&time_tmp, r);
}

struct tm* localtime(const time_t* t)
{
    struct tm* timeinfo;
    static struct tm tm_tmp;

    timeinfo = localtime_r(t, &tm_tmp);

    return timeinfo;
}

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_malloc(size | AOS_UNSIGNED_INT_MSB);
    aos_alloc_trace(mem, (size_t)__builtin_return_address(0));
#else
    mem = aos_malloc(size);
#endif

    return mem;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_realloc(old, newlen | AOS_UNSIGNED_INT_MSB);
    aos_alloc_trace(mem, (size_t)__builtin_return_address(0));
#else
    mem = aos_realloc(old, newlen);
#endif

    return mem;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_malloc((size * len) | AOS_UNSIGNED_INT_MSB);
    aos_alloc_trace(mem, (size_t)__builtin_return_address(0));
#else
    mem = aos_malloc(size * len);
#endif

    if (mem) {
        bzero(mem, size * len);
    }

    return mem;
}

#if (CONFIG_MEMALIGN_SUPPORT)
struct align_struct {
    int   used;
    void  *ptr;
};

static struct {
#define ALIGN_TOTAL_CNT_INIT (8)
    int                   total_cnt;
    int                   used_cnt;
    struct align_struct   *ptrs;
    aos_mutex_t           lock;
} g_align_cb;

#define align_is_init() (g_align_cb.total_cnt)
#define align_lock()    (aos_mutex_lock(&g_align_cb.lock, AOS_WAIT_FOREVER))
#define align_unlock()  (aos_mutex_unlock(&g_align_cb.lock))

void *_memalign_r(struct _reent *ptr, size_t alignment, size_t size)
{
    void *rc = NULL;
    size_t old_size, new_size;

    if (!align_is_init()) {
        aos_mutex_new(&g_align_cb.lock);
        g_align_cb.ptrs      = aos_calloc(ALIGN_TOTAL_CNT_INIT, sizeof(struct align_struct) * ALIGN_TOTAL_CNT_INIT);
        g_align_cb.total_cnt = ALIGN_TOTAL_CNT_INIT;
    }

    align_lock();
    if (g_align_cb.used_cnt >= g_align_cb.total_cnt) {
        old_size = sizeof(struct align_struct) * g_align_cb.total_cnt;
        g_align_cb.total_cnt += ALIGN_TOTAL_CNT_INIT;
        new_size = sizeof(struct align_struct) * g_align_cb.total_cnt;
        g_align_cb.ptrs = aos_realloc((void*)g_align_cb.ptrs, new_size);
        memset((uint8_t*)g_align_cb.ptrs + old_size, 0, new_size - old_size);
    }

    rc = aos_malloc_align(alignment, size);
    if (rc) {
        for (int i = 0; i < g_align_cb.total_cnt; i++) {
            if (g_align_cb.ptrs[i].used == 0) {
                g_align_cb.used_cnt++;
                g_align_cb.ptrs[i].used = 1;
                g_align_cb.ptrs[i].ptr  = rc;
                break;
            }
        }
    }
    align_unlock();

    return rc;
}

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    int rc = -1;
    void *ptr;

    ptr = memalign(alignment, size);
    if (ptr) {
        rc      = 0;
        *memptr = ptr;
    }

    return rc;
}
#else
void *_memalign_r(struct _reent *ptr, size_t alignment, size_t size)
{
    return NULL;
}
#endif

void _free_r(struct _reent *ptr, void *addr)
{
    if (!addr)
        return;
#if (CONFIG_MEMALIGN_SUPPORT)
    if (align_is_init()) {
        align_lock();
        if (g_align_cb.used_cnt) {
            for (int i = 0; i < g_align_cb.total_cnt; i++) {
                if (addr == g_align_cb.ptrs[i].ptr) {
                    aos_free_align(addr);
                    g_align_cb.used_cnt--;
                    g_align_cb.ptrs[i].used = 0;
                    g_align_cb.ptrs[i].ptr  = NULL;
                    align_unlock();
                    return;
                }
            }
        }
        align_unlock();
    }
#endif
    aos_free(addr);
}

void _exit(int status)
{
    while (1)
        ;
}

void exit(int status)
{
    aos_task_exit(status);
    __builtin_unreachable(); // fix noreturn warning
}

__attribute__((weak)) void _fini()
{
}

void _system(const char *s)
{
    return;
}

void abort(void)
{
#if defined(CONFIG_KERNEL_RHINO)
    k_err_proc(RHINO_SYS_FATAL_ERR);
#endif
    __builtin_unreachable(); // fix noreturn warning
}

int isatty(int fd)
{
    if (fd == fileno(stdin) || fd == fileno(stdout) || fd == fileno(stderr)) {
        return -1;
    }
    return 0;
}
