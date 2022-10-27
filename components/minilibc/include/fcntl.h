/********************************************************************************
 *
 *   Copyright (C) 2007-2009, 2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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
 ********************************************************************************/

#ifndef __INCLUDE_FCNTL_H
#define __INCLUDE_FCNTL_H

/********************************************************************************
 * Included Files
 ********************************************************************************/


#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 * Pre-processor Definitions
 ********************************************************************************/

/* open flag settings for open() (and related APIs) */
#define O_ACCMODE   00000003          /* Mask for access mode */
#define O_RDONLY    00000000        /* Open for read access (only) */
#define O_RDOK      O_RDONLY        /* Read access is permitted (non-standard) */
#define O_WRONLY    00000001       /* Open for write access (only) */
#define O_WROK      O_WRONLY        /* Write access is permitted (non-standard) */
#define O_RDWR      00000002 /* Open for both read & write access */
#define O_CREAT     00000100        /* Create file/sem/mq object */
#define O_EXCL      00000200        /* Name must not exist when opened  */
#define O_NOCTTY    00000400               /* Required by POSIX */
#define O_TRUNC     00001000        /* Delete contents */
#define O_APPEND    00002000        /* Keep contents, append to end */

#define O_NONBLOCK  00004000        /* Don't wait for data */
#define O_NDELAY    O_NONBLOCK      /* Synonym for O_NONBLOCK */
#define O_SYNC      00010000        /* Synchronize output on write */
#define O_DSYNC     O_SYNC          /* Equivalent to OSYNC in NuttX */
#define O_FSYNC     O_SYNC          /* Equivalent to OSYNC in NuttX */
#define O_ASYNC     00020000          /* Equivalent to OSYNC in NuttX */
#define O_BINARY    00000000        /* Open the file in binary (untranslated) mode. */

/* Unsupported, but required open flags */

#define O_RSYNC     0               /* Synchronize input on read */


#define O_TEXT      0               /* Open the file in text (translated) mode. */

/* This is the highest bit number used in the open flags bitset.  Bits above
 * this bit number may be used within NuttX for other, internal purposes.
 */

#define _O_MAXBIT   8

/* Synonyms historically used as F_SETFL flags (BSD). */

#define FNDELAY     O_NONBLOCK        /* Don't wait for data */
#define FNONBLOCK   O_NONBLOCK        /* Don't wait for data */
#define FAPPEND     O_APPEND          /* Keep contents, append to end */
#define FSYNC       O_SYNC            /* Synchronize output on write */
#define FASYNC      0                 /* No counterpart in NuttX */

/* FFCNTL is all the bits that may be set via fcntl. */

#define FFCNTL      (FNONBLOCK | FNDELAY | FAPPEND | FSYNC | FASYNC)

/* fcntl() commands */

#define F_DUPFD         0       /* dup */
#define F_GETFD         1       /* get close_on_exec */
#define F_SETFD         2       /* set/clear close_on_exec */
#define F_GETFL         3       /* get file->f_flags */
#define F_SETFL         4       /* set file->f_flags */
#ifndef F_GETLK
#define F_GETLK         5
#define F_SETLK         6
#define F_SETLKW        7
#endif
#ifndef F_SETOWN
#define F_SETOWN        8       /* for sockets. */
#define F_GETOWN        9       /* for sockets. */
#endif
#ifndef F_SETSIG
#define F_SETSIG        10      /* for sockets. */
#define F_GETSIG        11      /* for sockets. */
#endif

/* For posix fcntl() and lockf() */

#define F_RDLCK     0  /* Take out a read lease */
#define F_WRLCK     1  /* Take out a write lease */
#define F_UNLCK     2  /* Remove a lease */

/* close-on-exec flag for F_GETRL and F_SETFL */

#define FD_CLOEXEC  1

/* These are the notifications that can be received from F_NOTIFY (linux) */

#define DN_ACCESS   0  /* A file was accessed */
#define DN_MODIFY   1  /* A file was modified */
#define DN_CREATE   2  /* A file was created */
#define DN_DELETE   3  /* A file was unlinked */
#define DN_RENAME   4  /* A file was renamed */
#define DN_ATTRIB   5  /* Attributes of a file were changed */

/* int creat(const char *path, mode_t mode);
 *
 * is equivalent to open with O_WRONLY|O_CREAT|O_TRUNC.
 */

//#define creat(path, mode) open(path, O_WRONLY|O_CREAT|O_TRUNC, mode)

/********************************************************************************
 * Public Type Definitions
 ********************************************************************************/

/* struct flock is the third argument for F_GETLK, F_SETLK and F_SETLKW */

struct flock
{
  int16_t l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */
  int16_t l_whence;  /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
  off_t   l_start;   /* Starting offset for lock */
  off_t   l_len;     /* Number of bytes to lock */
  pid_t   l_pid;     /* PID of process blocking our lock (F_GETLK only) */
};

/********************************************************************************
 * Public Function Prototypes
 ********************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/********************************************************************************
 * Public Data
 ********************************************************************************/

/* POSIX-like File System Interfaces */

int open(const char *path, int oflag, ...);
int fcntl(int fd, int cmd, ...);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_FCNTL_H */
