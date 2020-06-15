/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "aos/network.h"

#if 0
static int net_read(network_t *n, unsigned char *buffer, int len, int timeout_ms)
{
    struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};

    if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 100)) {
        interval.tv_sec = 0;
        interval.tv_usec = 10000;
    }

    if (setsockopt(n->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval,
                   sizeof(struct timeval))) {
        return -1;
    }

    int rc = recvfrom(n->fd, buffer, len, 0,
                      (struct sockaddr *)&(n->address),
                      (socklen_t *)&(n->address.sin_len));

    return rc;
}

static int net_write(network_t *n, unsigned char *buffer, int len, int timeout_ms)
{
    struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};

    if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 100)) {
        interval.tv_sec = 0;
        interval.tv_usec = 10000;
    }

    if (setsockopt(n->fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&interval,
                   sizeof(struct timeval))) {
        return -1;
    }

    int rc = sendto(n->fd, buffer, len, 0,
                    (struct sockaddr *)&(n->address),
                    sizeof(n->address));

    return rc;
}
#else
static int net_read(network_t *n, unsigned char *buf, int count, int timeout_ms)
{
    fd_set fds;
    int lfirst = 1;
    int rc, len = 0, fd;
    struct timeval tv;
    unsigned long long delta;
    struct timeval t1 = {0}, t2 = {0};

    if (!(n && buf && count)) {
        return -1;
    }

    fd = n->fd;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    gettimeofday(&t1, NULL);
    while (len < count) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        /* FIXME: patch for select. */
        if (lfirst) {
            lfirst = 0;
        } else {
            gettimeofday(&t2, NULL);
            delta = (t2.tv_sec  - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
            if (delta >= timeout_ms)
                break;
        }

        rc = select(fd + 1, &fds, NULL, NULL, &tv);
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                aos_msleep(20);
                continue;
            }
            return -1;
        } else if (rc == 0) {
            /* time out */
            break;
        }

        if (!FD_ISSET(fd, &fds)) {
            aos_msleep(20);
            continue;
        }

        rc = recv(fd, buf + len, count - len, 0);
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                aos_msleep(20);
                continue;
            }
            return -1;
        } else if (rc == 0) {
            /* the sockfd may be closed */
            break;
        }

        len += rc;
    }

    return len;
}

/**
 * @brief  write n bytes from a sockfd with timeout
 * @param  [in] fd
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on err
 */
static int net_write(network_t *n, unsigned char *buf, int count, int timeout_ms)
{
    fd_set fds;
    int lfirst = 1;
    int rc, len = 0, fd;
    struct timeval tv;
    unsigned long long delta;
    struct timeval t1 = {0}, t2 = {0};

    if (!(n && buf && count)) {
        return -1;
    }

    fd = n->fd;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    gettimeofday(&t1, NULL);
    while (len < count) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        /* FIXME: patch for select. */
        if (lfirst) {
            lfirst = 0;
        } else {
            gettimeofday(&t2, NULL);
            delta = (t2.tv_sec  - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
            if (delta >= timeout_ms)
                break;
        }

        rc = select(fd + 1, NULL, &fds, NULL, &tv);
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                aos_msleep(20);
                continue;
            }
            return -1;
        } else if (rc == 0) {
            /* time out */
            break;
        }

        if (!FD_ISSET(fd, &fds)) {
            aos_msleep(20);
            continue;
        }

        rc = sendto(fd, buf + len, count - len, 0,
                    (struct sockaddr *)&(n->address),
                    sizeof(n->address));
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                aos_msleep(20);
                continue;
            }
            return -1;
        } else if (rc == 0) {
            /* the sockfd may be closed */
            break;
        }

        len += rc;
    }

    return len;
}
#endif

static int net_connect(network_t *n, char *addr, int port, int net_type)
{
    int rc = -1;
    sa_family_t family = AF_INET;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_UNSPEC, net_type, 0, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0) {
        struct addrinfo *res = result;

        /* prefer ip4 addresses */
        while (res) {
            if (res->ai_family == AF_INET) {
                result = res;
                break;
            }

            res = res->ai_next;
        }

        if (result->ai_family == AF_INET) {
            n->address.sin_port = htons(port);
            n->address.sin_family = family = AF_INET;
            n->address.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
        } else {
            rc = -1;
        }

        freeaddrinfo(result);
    }

    if (rc == 0) {
        n->fd = socket(family, net_type, 0);

        if (n->fd != -1) {
            rc = connect(n->fd, (struct sockaddr *)&n->address, sizeof(n->address));
        }

        if (rc < 0) {
            close(n->fd);
            n->fd = 0;
        }
    }

    return rc;
}

static void net_disconnect(network_t *n)
{
    close(n->fd);
    n->fd = 0;
}

void network_init(network_t *n)
{
    n->fd = 0;
    n->net_read       = net_read;
    n->net_write      = net_write;
    n->net_connect    = net_connect;
    n->net_disconncet = net_disconnect;
}
