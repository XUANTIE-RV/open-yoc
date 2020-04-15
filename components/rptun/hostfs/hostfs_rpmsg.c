/****************************************************************************
 * hostfs/hostfs_rpmsg.c
 * Hostfs rpmsg driver
 *
 *   Copyright (C) 2017 Pinecone Inc. All rights reserved.
 *   Author: Guiding Li<liguiding@pinecone.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <vfs.h>
#include <openamp.h>

#include <hostfs_rpmsg.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(x)             (sizeof(x) / sizeof((x)[0]))
#endif

#define HOSTFS_RPMSG_EPT_NAME       "rpmsg-hostfs"

#define HOSTFS_RPMSG_OPEN           1
#define HOSTFS_RPMSG_CLOSE          2
#define HOSTFS_RPMSG_READ           3
#define HOSTFS_RPMSG_WRITE          4
#define HOSTFS_RPMSG_LSEEK          5
#define HOSTFS_RPMSG_IOCTL          6
#define HOSTFS_RPMSG_SYNC           7
#define HOSTFS_RPMSG_DUP            8
#define HOSTFS_RPMSG_FSTAT          9
#define HOSTFS_RPMSG_FTRUNCATE      10
#define HOSTFS_RPMSG_OPENDIR        11
#define HOSTFS_RPMSG_READDIR        12
#define HOSTFS_RPMSG_REWINDDIR      13
#define HOSTFS_RPMSG_CLOSEDIR       14
#define HOSTFS_RPMSG_STATFS         15
#define HOSTFS_RPMSG_UNLINK         16
#define HOSTFS_RPMSG_MKDIR          17
#define HOSTFS_RPMSG_RMDIR          18
#define HOSTFS_RPMSG_RENAME         19
#define HOSTFS_RPMSG_STAT           20

#define HOSTFS_IXOTH     (1 << 0)   /* Bits 0-2: Permissions for others: RWX */
#define HOSTFS_IWOTH     (1 << 1)
#define HOSTFS_IROTH     (1 << 2)
#define HOSTFS_IRWXO     (7 << 0)

#define HOSTFS_IXGRP     (1 << 3)   /* Bits 3-5: Group permissions: RWX */
#define HOSTFS_IWGRP     (1 << 4)
#define HOSTFS_IRGRP     (1 << 5)
#define HOSTFS_IRWXG     (7 << 3)

#define HOSTFS_IXUSR     (1 << 6)   /* Bits 6-8: Owner permissions: RWX */
#define HOSTFS_IWUSR     (1 << 7)
#define HOSTFS_IRUSR     (1 << 8)
#define HOSTFS_IRWXU     (7 << 6)

#define HOSTFS_ISVTX     0          /* "Sticky" bit (not used) */
#define HOSTFS_ISGID     0          /* Set group ID bit (not used)*/
#define HOSTFS_ISUID     0          /* Set UID bit (not used) */

#define HOSTFS_IFIFO     0          /* Bits 11-14: File type bits (not all used) */
#define HOSTFS_IFCHR     (1 << 11)
#define HOSTFS_IFDIR     (2 << 11)
#define HOSTFS_IFBLK     (3 << 11)
#define HOSTFS_IFREG     (4 << 11)
#define HOSTFS_IFMQ      (5 << 11)
#define HOSTFS_IFSEM     (6 << 11)
#define HOSTFS_IFSHM     (7 << 11)
#define HOSTFS_IFSOCK    (8 << 11)
#define HOSTFS_IFMTD     (9 << 11)
#define HOSTFS_IFTGT     (15 << 11) /* May be the target of a symbolic link */

#define HOSTFS_IFLNK     (1 << 15)  /* Bit 15: Symbolic link */
#define HOSTFS_IFMT      (31 << 11) /* Bits 11-15: Full file type */

/* File type macros that operate on an instance of mode_t */

#define HOSTFS_ISFIFO(m) (0)
#define HOSTFS_ISCHR(m)  (((m) & HOSTFS_IFTGT) == HOSTFS_IFCHR)
#define HOSTFS_ISDIR(m)  (((m) & HOSTFS_IFTGT) == HOSTFS_IFDIR)
#define HOSTFS_ISBLK(m)  (((m) & HOSTFS_IFTGT) == HOSTFS_IFBLK)
#define HOSTFS_ISREG(m)  (((m) & HOSTFS_IFTGT) == HOSTFS_IFREG)
#define HOSTFS_ISMQ(m)   (((m) & HOSTFS_IFTGT) == HOSTFS_IFMQ)
#define HOSTFS_ISSEM(m)  (((m) & HOSTFS_IFTGT) == HOSTFS_IFSEM)
#define HOSTFS_ISSHM(m)  (((m) & HOSTFS_IFTGT) == HOSTFS_IFSHM)
#define HOSTFS_ISSOCK(m) (((m) & HOSTFS_IFTGT) == HOSTFS_IFSOCK)
#define HOSTFS_ISMTD(m)  (((m) & HOSTFS_IFTGT) == HOSTFS_IFMTD)
#define HOSTFS_ISLNK(m)  (((m) & HOSTFS_IFLNK) != 0)

#define HOSTFS_RDONLY    (1 << 0)                   /* Open for read access (only) */
#define HOSTFS_RDOK      HOSTFS_RDONLY              /* Read access is permitted (non-standard) */
#define HOSTFS_WRONLY    (1 << 1)                   /* Open for write access (only) */
#define HOSTFS_WROK      HOSTFS_WRONLY              /* Write access is permitted (non-standard) */
#define HOSTFS_RDWR      (HOSTFS_RDOK|HOSTFS_WROK)  /* Open for both read & write access */
#define HOSTFS_CREAT     (1 << 2)                   /* Create file/sem/mq object */
#define HOSTFS_EXCL      (1 << 3)                   /* Name must not exist when opened  */
#define HOSTFS_APPEND    (1 << 4)                   /* Keep contents, append to end */
#define HOSTFS_TRUNC     (1 << 5)                   /* Delete contents */
#define HOSTFS_NONBLOCK  (1 << 6)                   /* Don't wait for data */
#define HOSTFS_NDELAY    HOSTFS_NONBLOCK            /* Synonym for O_NONBLOCK */
#define HOSTFS_SYNC      (1 << 7)                   /* Synchronize output on write */
#define HOSTFS_DSYNC     HOSTFS_SYNC                /* Equivalent to OSYNC in NuttX */
#define HOSTFS_BINARY    (1 << 8)                   /* Open the file in binary (untranslated) mode. */
#define HOSTFS_DIRECT    (1 << 9)                   /* Avoid caching, write directly to hardware */

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct hostfs_rpmsg_s {
    struct rpmsg_endpoint ept;
    const char            *cpuname;
};

struct hostfs_rpmsg_cookie_s {
    aos_sem_t sem;
    int       result;
    void      *data;
};

struct hostfs_stat_s {
    uint32_t  st_mode;
    off_t     st_size;
    int16_t   st_blksize;
    uint32_t  st_blocks;
    time_t    st_atime;
    time_t    st_mtime;
    time_t    st_ctime;
    uint8_t   st_count;
};

struct hostfs_rpmsg_header_s {
    uint32_t command;
    int32_t  result;
    uint64_t cookie;
} __attribute__((__packed__));

struct hostfs_rpmsg_open_s {
    struct hostfs_rpmsg_header_s header;
    int32_t                      flags;
    int32_t                      mode;
    char                         pathname[0];
} __attribute__((__packed__));

struct hostfs_rpmsg_close_s {
    struct hostfs_rpmsg_header_s header;
    int32_t                      fd;
} __attribute__((__packed__));

struct hostfs_rpmsg_read_s {
    struct hostfs_rpmsg_header_s header;
    int32_t                      fd;
    uint32_t                     count;
    char                         buf[0];
} __attribute__((__packed__));

#define hostfs_rpmsg_write_s hostfs_rpmsg_read_s

struct hostfs_rpmsg_lseek_s {
    struct hostfs_rpmsg_header_s header;
    int32_t                      fd;
    int32_t                      whence;
    int32_t                      offset;
} __attribute__((__packed__));

struct hostfs_rpmsg_ioctl_s {
    struct hostfs_rpmsg_header_s header;
    int32_t                      fd;
    int32_t                      request;
    int32_t                      arg;
} __attribute__((__packed__));

#define hostfs_rpmsg_sync_s hostfs_rpmsg_close_s

struct hostfs_rpmsg_opendir_s {
    struct hostfs_rpmsg_header_s header;
    char                         pathname[0];
} __attribute__((__packed__));

struct hostfs_rpmsg_readdir_s {
    struct hostfs_rpmsg_header_s header;
    int32_t                      fd;
    uint32_t                     type;
    char                         name[0];
} __attribute__((__packed__));

#define hostfs_rpmsg_closedir_s hostfs_rpmsg_close_s
#define hostfs_rpmsg_unlink_s hostfs_rpmsg_opendir_s

struct hostfs_rpmsg_mkdir_s {
    struct hostfs_rpmsg_header_s header;
    int32_t                      mode;
    uint32_t                     reserved;
    char                         pathname[0];
} __attribute__((__packed__));

#define hostfs_rpmsg_rmdir_s hostfs_rpmsg_opendir_s
#define hostfs_rpmsg_rename_s hostfs_rpmsg_opendir_s

struct hostfs_rpmsg_stat_s {
    struct hostfs_rpmsg_header_s header;
    union {
        struct hostfs_stat_s     buf;
        uint32_t                 reserved[16];
    };
    char                         pathname[0];
} __attribute__((__packed__));

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int hostfs_rpmsg_default_handler(struct rpmsg_endpoint *ept,
                                        void *data, size_t len,
                                        uint32_t src, void *priv);
static int hostfs_rpmsg_read_handler(struct rpmsg_endpoint *ept,
                                     void *data, size_t len,
                                     uint32_t src, void *priv);
static int hostfs_rpmsg_readdir_handler(struct rpmsg_endpoint *ept,
                                        void *data, size_t len,
                                        uint32_t src, void *priv);
static int hostfs_rpmsg_stat_handler(struct rpmsg_endpoint *ept,
                                     void *data, size_t len,
                                     uint32_t src, void *priv);
static void hostfs_rpmsg_device_created(struct rpmsg_device *rdev,
                                        void *priv_);
static void hostfs_rpmsg_device_destroy(struct rpmsg_device *rdev,
                                        void *priv_);
static int  hostfs_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                                size_t len, uint32_t src, void *priv);
static int  hostfs_rpmsg_send_recv(uint32_t command, bool copy,
                                   struct hostfs_rpmsg_header_s *msg,
                                   int len, void *data);
static int hostfs_rpmsg_switch_flags(int flags);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct hostfs_rpmsg_s g_hostfs_rpmsg;

static const rpmsg_ept_cb g_hostfs_rpmsg_handler[] = {
    [HOSTFS_RPMSG_OPEN]      = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_CLOSE]     = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_READ]      = hostfs_rpmsg_read_handler,
    [HOSTFS_RPMSG_WRITE]     = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_LSEEK]     = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_IOCTL]     = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_SYNC]      = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_OPENDIR]   = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_READDIR]   = hostfs_rpmsg_readdir_handler,
    [HOSTFS_RPMSG_CLOSEDIR]  = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_UNLINK]    = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_MKDIR]     = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_RMDIR]     = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_RENAME]    = hostfs_rpmsg_default_handler,
    [HOSTFS_RPMSG_STAT]      = hostfs_rpmsg_stat_handler,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int hostfs_rpmsg_default_handler(struct rpmsg_endpoint *ept,
                                        void *data, size_t len,
                                        uint32_t src, void *priv)
{
    struct hostfs_rpmsg_header_s *header = data;
    struct hostfs_rpmsg_cookie_s *cookie =
        (struct hostfs_rpmsg_cookie_s *)(uintptr_t)header->cookie;

    cookie->result = header->result;
    if (cookie->result >= 0 && cookie->data) {
        memcpy(cookie->data, data, len);
    }
    aos_sem_signal(&cookie->sem);

    return 0;
}

static int hostfs_rpmsg_read_handler(struct rpmsg_endpoint *ept,
                                     void *data, size_t len,
                                     uint32_t src, void *priv)
{
    struct hostfs_rpmsg_header_s *header = data;
    struct hostfs_rpmsg_cookie_s *cookie =
        (struct hostfs_rpmsg_cookie_s *)(uintptr_t)header->cookie;
    struct hostfs_rpmsg_read_s *rsp = data;

    cookie->result = header->result;
    if (cookie->result > 0) {
        memcpy(cookie->data, rsp->buf, B2C(cookie->result));
    }
    aos_sem_signal(&cookie->sem);

    return 0;
}

static int hostfs_rpmsg_readdir_handler(struct rpmsg_endpoint *ept,
                                        void *data, size_t len,
                                        uint32_t src, void *priv)
{
    struct hostfs_rpmsg_header_s *header = data;
    struct hostfs_rpmsg_cookie_s *cookie =
        (struct hostfs_rpmsg_cookie_s *)(uintptr_t)header->cookie;
    struct hostfs_rpmsg_readdir_s *rsp = data;
    struct hostfs_dirent_s *entry = cookie->data;

    cookie->result = header->result;
    if (cookie->result >= 0) {
        strncpy(entry->d_name, rsp->name, HOSTFS_NAME_MAX);
        entry->d_name[HOSTFS_NAME_MAX] = '\0';
        entry->d_type = rsp->type;
    }
    aos_sem_signal(&cookie->sem);

    return 0;
}

static int hostfs_rpmsg_stat_handler(struct rpmsg_endpoint *ept,
                                     void *data, size_t len,
                                     uint32_t src, void *priv)
{
    struct hostfs_rpmsg_header_s *header = data;
    struct hostfs_rpmsg_cookie_s *cookie =
        (struct hostfs_rpmsg_cookie_s *)(uintptr_t)header->cookie;
    struct hostfs_rpmsg_stat_s *rsp = data;
    struct stat *buf = cookie->data;

    cookie->result = header->result;
    if (cookie->result >= 0) {
        buf->st_mode    = S_IRWXU | S_IRWXG | S_IRWXO |
                          (HOSTFS_ISDIR(rsp->buf.st_mode) ? S_IFDIR : S_IFREG);
        buf->st_size    = B2C(rsp->buf.st_size);
        buf->st_blksize = B2C(rsp->buf.st_blksize);
        buf->st_mtime   = rsp->buf.st_mtime;
    }
    aos_sem_signal(&cookie->sem);

    return 0;
}

static void hostfs_rpmsg_device_created(struct rpmsg_device *rdev,
                                        void *priv_)
{
    struct hostfs_rpmsg_s *priv = priv_;

    if (strcmp(priv->cpuname, rpmsg_get_cpuname(rdev)) == 0) {
        priv->ept.priv = priv;
        rpmsg_create_ept(&priv->ept, rdev, HOSTFS_RPMSG_EPT_NAME,
                         RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
                         hostfs_rpmsg_ept_cb, NULL);
    }
}

static void hostfs_rpmsg_device_destroy(struct rpmsg_device *rdev,
                                        void *priv_)
{
    struct hostfs_rpmsg_s *priv = priv_;

    if (strcmp(priv->cpuname, rpmsg_get_cpuname(rdev)) == 0) {
        rpmsg_destroy_ept(&priv->ept);
    }
}

static int hostfs_rpmsg_ept_cb(struct rpmsg_endpoint *ept, void *data,
                               size_t len, uint32_t src, void *priv)
{
    struct hostfs_rpmsg_header_s *header = data;
    uint32_t command = header->command;

    if (command < ARRAY_SIZE(g_hostfs_rpmsg_handler)) {
        return g_hostfs_rpmsg_handler[command](ept, data, len, src, priv);
    }

    return -EINVAL;
}

static int hostfs_rpmsg_send_recv(uint32_t command, bool copy,
                                  struct hostfs_rpmsg_header_s *msg,
                                  int len, void *data)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_cookie_s cookie;
    int ret;

    memset(&cookie, 0, sizeof(cookie));
    aos_sem_new(&cookie.sem, 0);

    if (data) {
        cookie.data = data;
    } else if (copy) {
        cookie.data = msg;
    }

    msg->command = command;
    msg->result  = -ENXIO;
    msg->cookie  = (uintptr_t)&cookie;

    if (copy) {
        ret = rpmsg_send(&priv->ept, msg, len);
    } else {
        ret = rpmsg_send_nocopy(&priv->ept, msg, len);
    }
    if (ret < 0) {
        goto fail;
    }

    while (1) {
        ret = aos_sem_wait(&cookie.sem, AOS_WAIT_FOREVER);
        if (ret != -EINTR) {
            if (ret == 0) {
                ret = cookie.result;
            }
            break;
        }
    }

fail:
    aos_sem_free(&cookie.sem);
    return ret;
}

static int hostfs_rpmsg_switch_flags(int flags)
{
    int acc_mode, hostfs_flags = 0;

    acc_mode = flags & O_ACCMODE;
    if (acc_mode == O_RDONLY) {
        hostfs_flags |= HOSTFS_RDONLY;
    } else if (acc_mode == O_WRONLY) {
        hostfs_flags |= HOSTFS_WRONLY;
    } else if (acc_mode == O_RDWR) {
        hostfs_flags |= HOSTFS_RDWR;
    }

    if ((flags & O_CREAT) && (flags & O_EXCL)) {
        hostfs_flags |= HOSTFS_CREAT | HOSTFS_EXCL;
    } else if ((flags & O_CREAT) && (flags & O_TRUNC)) {
        hostfs_flags |= HOSTFS_CREAT | HOSTFS_TRUNC;
    } else if (flags & O_APPEND) {
        hostfs_flags |= HOSTFS_CREAT | HOSTFS_APPEND;
    } else if (flags & O_CREAT) {
        hostfs_flags |= HOSTFS_CREAT;
    }
    return hostfs_flags;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int host_open(const char *pathname, int flags, int mode)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_open_s *msg;
    uint32_t space;
    size_t len;

    len  = sizeof(*msg);
    len += B2C(strlen(pathname) + 1);

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        return -ENOMEM;
    }

    aos_assert(len <= space);

    msg->flags = hostfs_rpmsg_switch_flags(flags);
    msg->mode  = mode;
    strcpy(msg->pathname, pathname);

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_OPEN, false,
                                  (struct hostfs_rpmsg_header_s *)msg, len, NULL);
}

int host_close(int fd)
{
    struct hostfs_rpmsg_close_s msg = {
        .fd = fd,
    };

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_CLOSE, true,
                                  (struct hostfs_rpmsg_header_s *)&msg, sizeof(msg), NULL);
}

ssize_t host_read(int fd, char *buf, size_t count)
{
    size_t read = 0;
    int ret = 0;

    while (read < count) {
        struct hostfs_rpmsg_read_s msg = {
            .fd    = fd,
            .count = C2B(count - read),
        };

        ret = hostfs_rpmsg_send_recv(HOSTFS_RPMSG_READ, true,
                                     (struct hostfs_rpmsg_header_s *)&msg, sizeof(msg), buf);
        if (ret <= 0) {
            break;
        }

        read += B2C(ret);
        buf  += B2C(ret);
    }

    return read ? read : ret;
}

ssize_t host_write(int fd, const char *buf, size_t count)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    size_t written = 0;
    int ret = 0;

    while (written < count) {
        struct hostfs_rpmsg_write_s *msg;
        uint32_t space;

        msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
        if (!msg) {
            ret = -ENOMEM;
            break;
        }

        space -= sizeof(*msg);
        if (space > count - written) {
            space = count - written;
        }

        msg->fd    = fd;
        msg->count = C2B(space);
        memcpy(msg->buf, buf + written, space);

        ret = hostfs_rpmsg_send_recv(HOSTFS_RPMSG_WRITE, false,
                                     (struct hostfs_rpmsg_header_s *)msg, sizeof(*msg) + space, NULL);
        if (ret <= 0) {
            break;
        }

        written += B2C(ret);
    }

    return written ? written : ret;
}

off_t host_lseek(int fd, off_t offset, int whence)
{
    struct hostfs_rpmsg_lseek_s msg = {
        .fd     = fd,
        .offset = C2B(offset),
        .whence = whence,
    };
    int ret;

    ret = hostfs_rpmsg_send_recv(HOSTFS_RPMSG_LSEEK, true,
                                 (struct hostfs_rpmsg_header_s *)&msg, sizeof(msg), NULL);

    return ret < 0 ? ret : B2C(ret);
}

int host_ioctl(int fd, int request, unsigned long arg)
{
    struct hostfs_rpmsg_ioctl_s msg = {
        .fd      = fd,
        .request = request,
        .arg     = arg,
    };

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_IOCTL, true,
                                  (struct hostfs_rpmsg_header_s *)&msg, sizeof(msg), NULL);
}

void host_sync(int fd)
{
    struct hostfs_rpmsg_sync_s msg = {
        .fd = fd,
    };

    hostfs_rpmsg_send_recv(HOSTFS_RPMSG_SYNC, true,
                           (struct hostfs_rpmsg_header_s *)&msg, sizeof(msg), NULL);
}

void *host_opendir(const char *name)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_opendir_s *msg;
    uint32_t space;
    size_t len;
    int ret;

    len  = sizeof(*msg);
    len += B2C(strlen(name) + 1);

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        return NULL;
    }

    aos_assert(len <= space);

    strcpy(msg->pathname, name);

    ret = hostfs_rpmsg_send_recv(HOSTFS_RPMSG_OPENDIR, false,
                                 (struct hostfs_rpmsg_header_s *)msg, len, NULL);

    return ret < 0 ? NULL : (void *)ret;
}

int host_readdir(void *dirp, struct hostfs_dirent_s* entry)
{
    struct hostfs_rpmsg_readdir_s msg = {
        .fd = (uintptr_t)dirp,
    };

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_READDIR, true,
                                  (struct hostfs_rpmsg_header_s *)&msg, sizeof(msg), entry);
}

int host_closedir(void *dirp)
{
    struct hostfs_rpmsg_closedir_s msg = {
        .fd = (uintptr_t)dirp,
    };

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_CLOSEDIR, true,
                                  (struct hostfs_rpmsg_header_s *)&msg, sizeof(msg), NULL);
}

int host_unlink(const char *pathname)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_unlink_s *msg;
    uint32_t space;
    size_t len;

    len  = sizeof(*msg);
    len += B2C(strlen(pathname) + 1);

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        return -ENOMEM;
    }

    aos_assert(len <= space);

    strcpy(msg->pathname, pathname);

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_UNLINK, false,
                                  (struct hostfs_rpmsg_header_s *)msg, len, NULL);
}

int host_mkdir(const char *pathname, uint32_t mode)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_mkdir_s *msg;
    uint32_t space;
    size_t len;

    len  = sizeof(*msg);
    len += B2C(strlen(pathname) + 1);

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        return -ENOMEM;
    }

    msg->mode = mode;
    strcpy(msg->pathname, pathname);

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_MKDIR, false,
                                  (struct hostfs_rpmsg_header_s *)msg, len, NULL);
}

int host_rmdir(const char *pathname)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_rmdir_s *msg;
    uint32_t space;
    size_t len;

    len  = sizeof(*msg);
    len += B2C(strlen(pathname) + 1);

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        return -ENOMEM;
    }

    aos_assert(len <= space);

    strcpy(msg->pathname, pathname);

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_RMDIR, false,
                                  (struct hostfs_rpmsg_header_s *)msg, len, NULL);
}

int host_rename(const char *oldpath, const char *newpath)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_rename_s *msg;
    size_t len, oldlen;
    uint32_t space;

    len     = sizeof(*msg);
    oldlen  = B2C((strlen(oldpath) + 1 + 0x7) & ~0x7);
    len    += oldlen + B2C(strlen(newpath) + 1);

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        return -ENOMEM;
    }

    aos_assert(len <= space);

    strcpy(msg->pathname, oldpath);
    strcpy(msg->pathname + oldlen, newpath);

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_RENAME, false,
                                  (struct hostfs_rpmsg_header_s *)msg, len, NULL);
}

int host_stat(const char *path, struct stat *buf)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;
    struct hostfs_rpmsg_stat_s *msg;
    uint32_t space;
    size_t len;

    len  = sizeof(*msg);
    len += B2C(strlen(path) + 1);

    msg = rpmsg_get_tx_payload_buffer(&priv->ept, &space, true);
    if (!msg) {
        return -ENOMEM;
    }

    aos_assert(len <= space);

    strcpy(msg->pathname, path);

    return hostfs_rpmsg_send_recv(HOSTFS_RPMSG_STAT, false,
                                  (struct hostfs_rpmsg_header_s *)msg, len, buf);
}

int hostfs_rpmsg_init(const char *cpuname)
{
    struct hostfs_rpmsg_s *priv = &g_hostfs_rpmsg;

    priv->cpuname = cpuname;

    return rpmsg_register_callback(priv,
                                   hostfs_rpmsg_device_created,
                                   hostfs_rpmsg_device_destroy,
                                   NULL);
}
