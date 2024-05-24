/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */
#include <poll.h>
#if !LWIP_POSIX_SOCKETS_IO_NAMES
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <aos/kernel.h>
#include <vfs.h>

struct poll_arg {
    aos_sem_t sem;
};

static void vfs_poll_notify(void *fd, void *arg)
{
    struct poll_arg *parg = arg;
    aos_sem_signal(&parg->sem);
}

static int wait_io(int maxfd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct poll_arg *parg, int timeout)
{
    timeout = timeout >= 0 ? timeout : AOS_WAIT_FOREVER;
    aos_sem_wait(&parg->sem, timeout);
    return 0;
}

static int init_parg(struct poll_arg *parg)
{
    aos_sem_new(&parg->sem,  0);
    return 0;
}

static void deinit_parg(struct poll_arg *parg)
{
    aos_sem_free(&parg->sem);
}

static int pre_poll(struct pollfd *fds, int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, void *parg)
{
    int i;
    int maxfd = 0;

    for (i = 0; i < nfds; i++) {
        struct pollfd *pfd = &fds[i];

        pfd->revents = 0;
    }

    for (i = 0; i < nfds; i++) {
        struct pollfd *pfd = &fds[i];
        if (pfd->fd < aos_vfs_fd_offset_get()) {
            if (pfd->fd > FD_SETSIZE - 1) {
                errno = EINVAL;
                return -1;
            }
            if (pfd->events & POLLIN) {
                FD_SET(pfd->fd, rfds);
            }
            if (pfd->events & POLLOUT) {
                FD_SET(pfd->fd, wfds);
            }
            if (pfd->events & POLLERR) {
                FD_SET(pfd->fd, efds);
            }
            maxfd = pfd->fd > maxfd ? pfd->fd : maxfd;
        } else {
            if (aos_do_pollfd(pfd->fd, true, vfs_poll_notify, pfd, parg) == -ENOENT) {
                errno = ENOENT;
                return -1;
            }
        }
    }

    return maxfd;
}

static int post_poll(struct pollfd *fds, int nfds)
{
    int j;
    int ret = 0;

    for (j = 0; j < nfds; j++) {
        struct pollfd *pfd = &fds[j];

        if (pfd->fd < aos_vfs_fd_offset_get()) {
            continue;
        }

        if (aos_do_pollfd(pfd->fd, false, NULL, NULL, NULL) == -ENOENT) {
            continue;
        }

        if (pfd->revents) {
            ret ++;
        }
    }

    return ret;
}

int aos_poll(struct pollfd *fds, int nfds, int timeout)
{
    fd_set rfds;
    fd_set wfds;
    fd_set efds;

    int ret = 0;
    int nset = 0;
    struct poll_arg parg;

    if (init_parg(&parg) < 0) {
        return -1;
    }

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    ret = pre_poll(fds, nfds, &rfds, &wfds, &efds, &parg);

    if (ret < 0) {
        goto check_poll;
    }

    ret = wait_io(ret, &rfds, &wfds, &efds, &parg, timeout);

    if (ret > 0) {
        int i;

        for (i = 0; i < nfds; i++) {
            struct pollfd *pfd = &fds[i];

            if (FD_ISSET(pfd->fd, &rfds)) {
                pfd->revents |= POLLIN;
            }
            if (FD_ISSET(pfd->fd, &wfds)) {
                pfd->revents |= POLLOUT;
            }
            if (FD_ISSET(pfd->fd, &efds)) {
                pfd->revents |= POLLERR;
            }
        }
        nset += ret;
    }

check_poll:
    nset += post_poll(fds, nfds);

    deinit_parg(&parg);

    return ret < 0 ? -1 : nset;
}

int poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
#if defined(CONFIG_AOS_LWIP) || defined(CONFIG_SAL)
#define FLAG_FD_SOCKET 1
#define FLAG_FD_DEVICE (1 << 1)
    int flags = 0;
    for (int j = 0; j < nfds; j++) {
        struct pollfd *pfd = &fds[j];
        if (pfd->fd < aos_vfs_fd_offset_get()) {
            flags |= FLAG_FD_SOCKET;
        } else {
            flags |= FLAG_FD_DEVICE;
        }
    }
    if (flags == (FLAG_FD_SOCKET | FLAG_FD_DEVICE)) {
        // FIXME: socket and files can't exist the same time
        printf("socket and files can't exist the same time\r\n");
        return -1;
    }
    if (flags == FLAG_FD_SOCKET) {
#if defined(CONFIG_AOS_LWIP)
        extern int lwip_poll(struct pollfd *fds, nfds_t nfds, int timeout);
        return lwip_poll(fds, nfds, timeout);
#elif defined(CONFIG_SAL)
        // TODO:
        return 0;
#endif
    }
#endif
    return aos_poll(fds, nfds, timeout);
}

#endif /* !LWIP_POSIX_SOCKETS_IO_NAMES */