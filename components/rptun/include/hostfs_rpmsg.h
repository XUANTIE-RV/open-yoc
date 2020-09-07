/****************************************************************************
 * hostfs/hostfs_rpmsg.h
 * Hostfs rpmsg driver header file
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

#ifndef __FS_HOSTFS_HOSTFS_RPMSG_H
#define __FS_HOSTFS_HOSTFS_RPMSG_H

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define HOSTFS_NAME_MAX             32

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct hostfs_dirent_s {
    uint8_t   d_type;
    char      d_name[HOSTFS_NAME_MAX + 1];
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int     host_open(const char *pathname, int flags, int mode);
int     host_close(int fd);
ssize_t host_read(int fd, char *buf, size_t count);
ssize_t host_write(int fd, const char *buf, size_t count);
off_t   host_lseek(int fd, off_t offset, int whence);
int     host_ioctl(int fd, int request, unsigned long arg);
void    host_sync(int fd);
void   *host_opendir(const char *name);
int     host_readdir(void* dirp, struct hostfs_dirent_s *entry);
int     host_closedir(void* dirp);
int     host_unlink(const char *pathname);
int     host_mkdir(const char *pathname, uint32_t mode);
int     host_rmdir(const char *pathname);
int     host_rename(const char *oldpath, const char *newpath);
int     host_stat(const char *path, struct stat *buf);
int     hostfs_rpmsg_init(const char *cpuname);

#endif /* __FS_HOSTFS_HOSTFS_RPMSG_H */
