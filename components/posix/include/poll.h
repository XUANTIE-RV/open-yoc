/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef _POLL_H
#define _POLL_H

#if defined(CONFIG_AOS_LWIP) || defined(CONFIG_SAL)
#include <lwip/opt.h>
#else
#undef LWIP_POSIX_SOCKETS_IO_NAMES
#define LWIP_POSIX_SOCKETS_IO_NAMES 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !LWIP_POSIX_SOCKETS_IO_NAMES

#if !defined(POLLIN) && !defined(POLLOUT)
#define POLLIN     0x1
#define POLLOUT    0x2
#define POLLERR    0x4
#define POLLNVAL   0x8
/* Below values are unimplemented */
#define POLLRDNORM 0x10
#define POLLRDBAND 0x20
#define POLLPRI    0x40
#define POLLWRNORM 0x80
#define POLLWRBAND 0x100
#define POLLHUP    0x200
typedef unsigned int nfds_t;
struct pollfd {
    int fd;        /* file descriptor */
    short events;  /* requested events */
    short revents; /* returned events */
};
#endif

int poll(struct pollfd fds[], nfds_t nfds, int timeout);

#endif /* !LWIP_POSIX_SOCKETS_IO_NAMES */

#ifdef __cplusplus
}
#endif

#endif /*_POLL_H*/
