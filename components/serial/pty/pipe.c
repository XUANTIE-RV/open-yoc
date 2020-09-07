/****************************************************************************
 * serial/pty/pipe.c
 *
 *   Copyright (C) 2008-2009, 2015, 2018 Gregory Nutt. All rights reserved.
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
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <soc.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <aos/kernel.h>
#include <vfs.h>
#include <vfs_register.h>
#include <vfs_inode.h>

#include "pipe_common.h"

#if CONFIG_DEV_PIPE_SIZE > 0

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MAX_PIPES 32

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int pipe_close(file_t *filep);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const file_ops_t pipe_fops =
{
    pipecommon_open,   /* open */
    pipe_close,        /* close */
    pipecommon_read,   /* read */
    pipecommon_write,  /* write */
    pipecommon_ioctl,  /* ioctl */
#ifndef CONFIG_DISABLE_POLL
    pipecommon_poll,   /* poll */
#endif
};

static aos_sem_t  g_pipesem;
static uint32_t   g_pipeset     = 0;
static uint32_t   g_pipecreated = 0;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pipe_allocate
 ****************************************************************************/

static inline int pipe_allocate(void)
{
    int pipeno;
    int ret = -ENFILE;

    for (pipeno = 0; pipeno < MAX_PIPES; pipeno++) {
        if ((g_pipeset & (1 << pipeno)) == 0) {
            g_pipeset |= (1 << pipeno);
            ret = pipeno;
            break;
        }
    }

    return ret;
}

/****************************************************************************
 * Name: pipe_free
 ****************************************************************************/

static inline void pipe_free(int pipeno)
{
    int ret;

    ret = aos_sem_wait(&g_pipesem, AOS_WAIT_FOREVER);
    if (ret == OK) {
        g_pipeset &= ~(1 << pipeno);
        (void)aos_sem_signal(&g_pipesem);
    }
}

/****************************************************************************
 * Name: pipe_close
 ****************************************************************************/

static int pipe_close(file_t *filep)
{
    inode_t             *inode = filep->node;
    struct pipe_dev_s   *dev   = (struct pipe_dev_s *)inode->i_arg;
    int                 ret;

    aos_assert(dev);

    /* Perform common close operations */

    ret = pipecommon_close(filep);
    if (ret == 0 && dev->d_refs == 0) {
        /* Release the pipe when there are no further open references to it. */

        pipe_free(dev->d_pipeno);
    }

    return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pipe_init
 *
 * Description:
 *   pipe init
 *
 * Input Parameters:
 *   NULL
 *
 * Returned Value:
 *   0 is returned on success; otherwise, -1 is returned with errno set
 *   appropriately.
 *
 ****************************************************************************/

void pipe_init(void)
{
    aos_sem_new(&g_pipesem, 1);
}

/****************************************************************************
 * Name: pipe2
 *
 * Description:
 *   pipe() creates a pair of file descriptors, pointing to a pipe inode,
 *   and  places them in the array pointed to by 'fd'. fd[0] is for reading,
 *   fd[1] is for writing.
 *
 *   NOTE: mkfifo2 is a special, non-standard, NuttX-only interface.  Since
 *   the NuttX FIFOs are based in in-memory, circular buffers, the ability
 *   to control the size of those buffers is critical for system tuning.
 *
 * Input Parameters:
 *   fd[2] - The user provided array in which to catch the pipe file
 *   descriptors
 *   bufsize - The size of the in-memory, circular buffer in bytes.
 *
 * Returned Value:
 *   0 is returned on success; otherwise, -1 is returned with errno set
 *   appropriately.
 *
 ****************************************************************************/

int pipe2(int fd[2], size_t bufsize)
{
    struct pipe_dev_s *dev = NULL;
    char devname[16];
    int pipeno;
    int errcode;
    int ret;

    /* Get exclusive access to the pipe allocation data */

    ret = aos_sem_wait(&g_pipesem, AOS_WAIT_FOREVER);
    if (ret < 0) {
        errcode = -ret;
        goto errout;
    }

    /* Allocate a minor number for the pipe device */

    pipeno = pipe_allocate();
    if (pipeno < 0) {
        (void)aos_sem_signal(&g_pipesem);
        errcode = -pipeno;
        goto errout;
    }

    /* Create a pathname to the pipe device */

    snprintf(devname, sizeof(devname), "/dev/pipe%d", pipeno);

    /* Check if the pipe device has already been created */

    if ((g_pipecreated & (1 << pipeno)) == 0) {
        /* No.. Allocate and initialize a new device structure instance */

        dev = pipecommon_allocdev(bufsize);
        if (!dev) {
            (void)aos_sem_signal(&g_pipesem);
            errcode = ENOMEM;
            goto errout_with_pipe;
        }

        dev->d_pipeno = pipeno;

        /* Register the pipe device */

        ret = aos_register_driver(devname, &pipe_fops, (void *)dev);
        if (ret != 0) {
            (void)aos_sem_signal(&g_pipesem);
            errcode = -ret;
            goto errout_with_dev;
        }

        /* Remember that we created this device */

        g_pipecreated |= (1 << pipeno);
    }

    (void)aos_sem_signal(&g_pipesem);

    /* Get a write file descriptor */

    fd[1] = aos_open(devname, O_WRONLY);
    if (fd[1] < 0) {
        errcode = -fd[1];
        goto errout_with_driver;
    }

    /* Get a read file descriptor */

    fd[0] = aos_open(devname, O_RDONLY);
    if (fd[0] < 0) {
        errcode = -fd[0];
        goto errout_with_wrfd;
    }

    return OK;

errout_with_wrfd:
    aos_close(fd[1]);

errout_with_driver:
    aos_unregister_driver(devname);

errout_with_dev:
    if (dev) {
        pipecommon_freedev(dev);
    }

errout_with_pipe:
    pipe_free(pipeno);

errout:
    return -errcode;
}

#endif /* CONFIG_DEV_PIPE_SIZE > 0 */
