/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_TYPES_H
#define AOS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POLLIN  0x1
#define POLLOUT 0x2
#define POLLERR 0x4
struct pollfd {
    int fd;
    short events;
    short revents;
};

#define AOS_EXPORT(...)

#ifdef __cplusplus
}
#endif

#if !defined(__BEGIN_DECLS__) || !defined(__END_DECLS__)

#if defined(__cplusplus)
#  define __BEGIN_DECLS__ extern "C" {
#  define __END_DECLS__   }
#else
#  define __BEGIN_DECLS__
#  define __END_DECLS__
#endif

#endif


#endif /* AOS_TYPES_H */

