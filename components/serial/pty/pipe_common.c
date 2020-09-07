/****************************************************************************
 * serial/pty/pipe_common.c
 *
 *   Copyright (C) 2008-2009, 2011, 2015-2016, 2018 Gregory Nutt. All
 *     rights reserved.
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
#include <tioctl.h>

#include "pipe_common.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TAG "pipe"

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void pipecommon_semtake(aos_sem_t *sem);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pipecommon_semtake
 ****************************************************************************/

static void pipecommon_semtake(aos_sem_t *sem)
{
    int ret;

    do {
        /* Take the semaphore (perhaps waiting) */

        ret = aos_sem_wait(sem, AOS_WAIT_FOREVER);

        /* The only case that an error should occur here is if the wait was
         * awakened by a signal.
         */

        aos_assert(ret == OK || ret == -EINTR);
    }
    while (ret == -EINTR);
}

/****************************************************************************
 * Name: pipecommon_pollnotify
 ****************************************************************************/

#ifndef CONFIG_DISABLE_POLL
static void pipecommon_pollnotify(struct pipe_dev_s *dev,
                                  uint8_t eventset)
{
    int i;

    if (eventset & POLLERR) {
        eventset &= ~(POLLOUT | POLLIN);
    }

    for (i = 0; i < CONFIG_DEV_PIPE_NPOLLWAITERS; i++) {
        struct pollfd *fds = dev->d_fds[i];

        if (fds) {
            fds->revents |= eventset & (fds->events | POLLERR | POLLHUP);

            if ((fds->revents & (POLLOUT | POLLHUP)) == (POLLOUT | POLLHUP)) {
                /* POLLOUT and POLLHUP are mutually exclusive. */

                fds->revents &= ~POLLOUT;
            }

            if (fds->revents != 0) {
                LOGI(TAG, "Report events: %02x\n", fds->revents);
                aos_sem_signal(fds->sem);
            }
        }
    }
}
#else
#  define pipecommon_pollnotify(dev,event)
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pipecommon_allocdev
 ****************************************************************************/

struct pipe_dev_s *pipecommon_allocdev(size_t bufsize)
{
    struct pipe_dev_s *dev;

    aos_assert(bufsize <= CONFIG_DEV_PIPE_MAXSIZE);

    /* Allocate a private structure to manage the pipe */

    dev = (struct pipe_dev_s *)aos_malloc(sizeof(struct pipe_dev_s));
    if (dev) {
        /* Initialize the private structure */

        memset(dev, 0, sizeof(struct pipe_dev_s));
        aos_sem_new(&dev->d_bfsem, 1);
        aos_sem_new(&dev->d_rdsem, 0);
        aos_sem_new(&dev->d_wrsem, 0);

        dev->d_bufsize = bufsize;
    }

    return dev;
}

/****************************************************************************
 * Name: pipecommon_freedev
 ****************************************************************************/

void pipecommon_freedev(struct pipe_dev_s *dev)
{
    aos_sem_free(&dev->d_bfsem);
    aos_sem_free(&dev->d_rdsem);
    aos_sem_free(&dev->d_wrsem);
    aos_free(dev);
}

/****************************************************************************
 * Name: pipecommon_open
 ****************************************************************************/

int pipecommon_open(inode_t *node, file_t *filep)
{
    inode_t             *inode = filep->node;
    struct pipe_dev_s   *dev   = (struct pipe_dev_s *)inode->i_arg;
    int                 ret;

    aos_assert(dev != NULL);

    /* Make sure that we have exclusive access to the device structure.  The
     * nxsem_wait() call should fail only if we are awakened by a signal.
     */

    ret = aos_sem_wait(&dev->d_bfsem, AOS_WAIT_FOREVER);
    if (ret < 0) {
      LOGE(TAG, "ERROR: nxsem_wait failed: %d\n", ret);
      return ret;
    }

    /* If this the first reference on the device, then allocate the buffer.
     * In the case of policy 1, the buffer already be present when the pipe
     * is first opened.
     */

    if (dev->d_refs == 0 && dev->d_buffer == NULL) {
        dev->d_buffer = (uint8_t *)aos_malloc(dev->d_bufsize);
        if (!dev->d_buffer) {
            (void)aos_sem_signal(&dev->d_bfsem);
            return -ENOMEM;
        }
    }

    /* Increment the reference count on the pipe instance */

    dev->d_refs++;

    /* If opened for writing, increment the count of writers on the pipe instance */

    if ((filep->i_flags & O_WROK) != 0) {
        dev->d_nwriters++;

        /* If this this is the first writer, then the read semaphore indicates the
        * number of readers waiting for the first writer.  Wake them all up.
        */

        if (dev->d_nwriters == 1) {
            aos_sem_signal_all(&dev->d_rdsem);
        }
    } else {
        dev->d_nreaders++;
    }

    /* If opened for read-only, then wait for either (1) at least one writer
     * on the pipe (policy == 0), or (2) until there is buffered data to be
     * read (policy == 1).
     */

    aos_kernel_sched_suspend();
    (void)aos_sem_signal(&dev->d_bfsem);

    if ((filep->i_flags & O_RDWR) == O_RDONLY &&  /* Read-only */
        dev->d_nwriters < 1 &&                     /* No writers on the pipe */
        dev->d_wrndx == dev->d_rdndx) {            /* Buffer is empty */

        /* NOTE: d_rdsem is normally used when the read logic waits for more
         * data to be written.  But until the first writer has opened the
         * pipe, the meaning is different: it is used prevent O_RDONLY open
         * calls from returning until there is at least one writer on the pipe.
         * This is required both by spec and also because it prevents
         * subsequent read() calls from returning end-of-file because there is
         * no writer on the pipe.
         */

        ret = aos_sem_wait(&dev->d_rdsem, AOS_WAIT_FOREVER);
        if (ret < 0) {
            /* The nxsem_wait() call should fail only if we are awakened by
            * a signal.
            */

            LOGE(TAG, "ERROR: nxsem_wait failed: %d\n", ret);

            /* Immediately close the pipe that we just opened */

            (void)pipecommon_close(filep);
        }
    }

    aos_kernel_sched_resume();
    return ret;
}

/****************************************************************************
 * Name: pipecommon_close
 ****************************************************************************/

int pipecommon_close(file_t *filep)
{
    inode_t             *inode = filep->node;
    struct pipe_dev_s   *dev   = (struct pipe_dev_s *)inode->i_arg;

    aos_assert(dev && dev->d_refs > 0);

    /* Make sure that we have exclusive access to the device structure.
     * NOTE: close() is supposed to return EINTR if interrupted, however
     * I've never seen anyone check that.
     */

    pipecommon_semtake(&dev->d_bfsem);

    /* Decrement the number of references on the pipe.  Check if there are
     * still outstanding references to the pipe.
     */

    /* Check if the decremented reference count would go to zero */

    if (--dev->d_refs > 0) {
        /* No more references.. If opened for writing, decrement the count of
         * writers on the pipe instance.
         */

        if ((filep->i_flags & O_WROK) != 0) {
            /* If there are no longer any writers on the pipe, then notify all of the
             * waiting readers that they must return end-of-file.
             */

            if (--dev->d_nwriters <= 0) {
                aos_sem_signal_all(&dev->d_rdsem);

                /* Inform poll readers that other end closed. */

                pipecommon_pollnotify(dev, POLLHUP);
            }
        }

        /* If opened for reading, decrement the count of readers on the pipe
         * instance.
         */

        if ((filep->i_flags & O_RDOK) != 0) {
            if (--dev->d_nreaders <= 0) {
                if (PIPE_IS_POLICY_0(dev->d_flags)) {
                    /* Inform poll writers that other end closed. */

                    pipecommon_pollnotify(dev, POLLERR);
                }
            }
        }
    }

    /* What is the buffer management policy?  Do we free the buffer when the
     * last client closes the pipe policy 0, or when the buffer becomes empty.
     * In the latter case, the buffer data will remain valid and can be
     * obtained when the pipe is re-opened.
     */

    else if (PIPE_IS_POLICY_0(dev->d_flags) || dev->d_wrndx == dev->d_rdndx) {
        /* Policy 0 or the buffer is empty ... deallocate the buffer now. */

        aos_free(dev->d_buffer);
        dev->d_buffer = NULL;

        /* And reset all counts and indices */

        dev->d_wrndx    = 0;
        dev->d_rdndx    = 0;
        dev->d_refs     = 0;
        dev->d_nwriters = 0;
        dev->d_nreaders = 0;

#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
        /* If, in addition, we have been unlinked, then also need to free the
         * device structure as well to prevent a memory leak.
         */

        if (PIPE_IS_UNLINKED(dev->d_flags)) {
            pipecommon_freedev(dev);
            return OK;
        }
#endif
    }

    aos_sem_signal(&dev->d_bfsem);
    return OK;
}

/****************************************************************************
 * Name: pipecommon_read
 ****************************************************************************/

ssize_t pipecommon_read(file_t *filep, void *buffer, size_t len)
{
    inode_t             *inode = filep->node;
    struct pipe_dev_s   *dev   = (struct pipe_dev_s *)inode->i_arg;
    char                *buf   = (char *)buffer;
    ssize_t             nread  = 0;
    int                 ret;

    aos_assert(dev);

    if (len == 0) {
        return 0;
    }

    /* Make sure that we have exclusive access to the device structure */

    ret = aos_sem_wait(&dev->d_bfsem, AOS_WAIT_FOREVER);
    if (ret < 0) {
        return ret;
    }

    /* If the pipe is empty, then wait for something to be written to it */

    while (dev->d_wrndx == dev->d_rdndx) {
        /* If O_NONBLOCK was set, then return EGAIN */

        if (filep->i_flags & O_NONBLOCK) {
            aos_sem_signal(&dev->d_bfsem);
            return -EAGAIN;
        }

        /* If there are no writers on the pipe, then return end of file */

        if (dev->d_nwriters <= 0) {
            aos_sem_signal(&dev->d_bfsem);
            return 0;
        }

        /* Otherwise, wait for something to be written to the pipe */

        aos_kernel_sched_suspend();
        aos_sem_signal(&dev->d_bfsem);
        ret = aos_sem_wait(&dev->d_rdsem, AOS_WAIT_FOREVER);
        aos_kernel_sched_resume();

        if (ret < 0 || (ret = aos_sem_wait(&dev->d_bfsem, AOS_WAIT_FOREVER)) < 0) {
            return ret;
        }
    }

    /* Then return whatever is available in the pipe (which is at least one byte) */

    nread = 0;
    while ((size_t)nread < len && dev->d_wrndx != dev->d_rdndx) {
        *buf++ = dev->d_buffer[dev->d_rdndx];
        if (++dev->d_rdndx >= dev->d_bufsize) {
            dev->d_rdndx = 0;
        }

        nread++;
    }

    /* Notify all waiting writers that bytes have been removed from the buffer */

    aos_sem_signal_all(&dev->d_wrsem);

    /* Notify all poll/select waiters that they can write to the FIFO */

    pipecommon_pollnotify(dev, POLLOUT);

    aos_sem_signal(&dev->d_bfsem);
    return nread;
}

/****************************************************************************
 * Name: pipecommon_write
 ****************************************************************************/

ssize_t pipecommon_write(file_t *filep, const void *buffer, size_t len)
{
    inode_t             *inode      = filep->node;
    struct pipe_dev_s   *dev        = (struct pipe_dev_s *)inode->i_arg;
    const char          *buf        = (const char *)buffer;
    ssize_t             nwritten    = 0;
    ssize_t             last;
    int                 nxtwrndx;
    int                 ret;

    aos_assert(dev);

    /* Handle zero-length writes */

    if (len == 0) {
        return 0;
    }

    /* REVISIT:  "If all file descriptors referring to the read end of a pipe
     * have been closed, then a write will cause a SIGPIPE signal to be
     * generated for the calling process.  If the calling process is ignoring
     * this signal, then write(2) fails with the error EPIPE."
     */

    if (dev->d_nreaders <= 0) {
        return -EPIPE;
    }

    /* At present, this method cannot be called from interrupt handlers.  That
     * is because it calls nxsem_wait (via pipecommon_semtake below) and
     * nxsem_wait cannot be called from interrupt level.  This actually
     * happens fairly commonly IF [a-z]err() is called from interrupt handlers
     * and stdout is being redirected via a pipe.  In that case, the debug
     * output will try to go out the pipe (interrupt handlers should use the
     * _err() APIs).
     *
     * On the other hand, it would be very valuable to be able to feed the pipe
     * from an interrupt handler!  TODO:  Consider disabling interrupts instead
     * of taking semaphores so that pipes can be written from interrupt handlers
     */

    /* Make sure that we have exclusive access to the device structure */

    ret = aos_sem_wait(&dev->d_bfsem, AOS_WAIT_FOREVER);
    if (ret < 0) {
        return ret;
    }

    /* Loop until all of the bytes have been written */

    last = 0;
    for (; ; ) {
        /* Calculate the write index AFTER the next byte is written */

        nxtwrndx = dev->d_wrndx + 1;
        if (nxtwrndx >= dev->d_bufsize) {
            nxtwrndx = 0;
        }

        /* Would the next write overflow the circular buffer? */

        if (nxtwrndx != dev->d_rdndx) {
            /* No... copy the byte */

            dev->d_buffer[dev->d_wrndx] = *buf++;
            dev->d_wrndx = nxtwrndx;

            /* Is the write complete? */

            nwritten++;
            if ((size_t)nwritten >= len) {
                /* Yes.. Notify all of the waiting readers that more data is available */

                aos_sem_signal_all(&dev->d_rdsem);

                /* Notify all poll/select waiters that they can read from the FIFO */

                pipecommon_pollnotify(dev, POLLIN);

                /* Return the number of bytes written */

                aos_sem_signal(&dev->d_bfsem);
                return len;
            }
        } else {
            /* There is not enough room for the next byte.  Was anything written in this pass? */

            if (last < nwritten) {
                /* Yes.. Notify all of the waiting readers that more data is available */

                aos_sem_signal_all(&dev->d_rdsem);

                /* Notify all poll/select waiters that they can read from the FIFO */

                pipecommon_pollnotify(dev, POLLIN);
            }

            last = nwritten;

            /* If O_NONBLOCK was set, then return partial bytes written or EGAIN */

            if (filep->i_flags & O_NONBLOCK) {
                if (nwritten == 0) {
                    nwritten = -EAGAIN;
                }

                aos_sem_signal(&dev->d_bfsem);
                return nwritten;
            }

            /* There is more to be written.. wait for data to be removed from the pipe */

            aos_kernel_sched_suspend();
            aos_sem_signal(&dev->d_bfsem);
            pipecommon_semtake(&dev->d_wrsem);
            aos_kernel_sched_resume();
            pipecommon_semtake(&dev->d_bfsem);
        }
    }
}

/****************************************************************************
 * Name: pipecommon_poll
 ****************************************************************************/

#ifndef CONFIG_DISABLE_POLL
int pipecommon_poll(file_t *filep, bool setup, poll_notify_t notify, struct pollfd *fds, void *arg)
{
    inode_t           *inode    = filep->node;
    struct pipe_dev_s *dev      = (struct pipe_dev_s *)inode->i_arg;
    uint8_t            eventset;
    pipe_ndx_t         nbytes;
    int                ret      = OK;
    int                i;

    aos_assert(dev && fds);

    /* Are we setting up the poll?  Or tearing it down? */

    pipecommon_semtake(&dev->d_bfsem);
    if (setup) {
        /* This is a request to set up the poll.  Find an available
         * slot for the poll structure reference
         */

        for (i = 0; i < CONFIG_DEV_PIPE_NPOLLWAITERS; i++) {
            /* Find an available slot */

            if (!dev->d_fds[i]) {
                /* Bind the poll structure and this slot */

                dev->d_fds[i] = fds;
                fds->priv     = &dev->d_fds[i];
                break;
            }
        }

        if (i >= CONFIG_DEV_PIPE_NPOLLWAITERS) {
            fds->priv   = NULL;
            ret         = -EBUSY;
            goto errout;
        }

        /* Should immediately notify on any of the requested events?
         * First, determine how many bytes are in the buffer
         */

        if (dev->d_wrndx >= dev->d_rdndx) {
            nbytes = dev->d_wrndx - dev->d_rdndx;
        } else {
            nbytes = dev->d_bufsize + dev->d_wrndx - dev->d_rdndx;
        }

        /* Notify the POLLOUT event if the pipe is not full, but only if
         * there is readers.
         */

        eventset = 0;
        if ((filep->i_flags & O_WROK) && (nbytes < (dev->d_bufsize - 1))) {
            eventset |= POLLOUT;
        }

        /* Notify the POLLIN event if the pipe is not empty */

        if ((filep->i_flags & O_RDOK) && (nbytes > 0)) {
            eventset |= POLLIN;
        }

        /* Notify the POLLHUP event if the pipe is empty and no writers */

        if (nbytes == 0 && dev->d_nwriters <= 0) {
            eventset |= POLLHUP;
        }

        /* Change POLLOUT to POLLERR, if no readers and policy 0. */

        if ((eventset | POLLOUT) &&
             PIPE_IS_POLICY_0(dev->d_flags) &&
             dev->d_nreaders <= 0) {
            eventset |= POLLERR;
        }

        if (eventset) {
            pipecommon_pollnotify(dev, eventset);
        }
    } else {
        /* This is a request to tear down the poll. */

        struct pollfd **slot = (struct pollfd **)fds->priv;

        /* Remove all memory of the poll setup */

        *slot                = NULL;
        fds->priv            = NULL;
    }

errout:
    aos_sem_signal(&dev->d_bfsem);
    return ret;
}
#endif

/****************************************************************************
 * Name: pipecommon_ioctl
 ****************************************************************************/

int pipecommon_ioctl(file_t *filep, int cmd, unsigned long arg)
{
    inode_t             *inode      = filep->node;
    struct pipe_dev_s   *dev        = (struct pipe_dev_s *)inode->i_arg;
    int                 ret         = -EINVAL;

    pipecommon_semtake(&dev->d_bfsem);

    switch (cmd) {
        case PIPEIOC_POLICY:
        {
            if (arg != 0) {
                PIPE_POLICY_1(dev->d_flags);
            } else {
                PIPE_POLICY_0(dev->d_flags);
            }

            ret = OK;
        }
        break;

        case FIONWRITE:  /* Number of bytes waiting in send queue */
        case FIONREAD:   /* Number of bytes available for reading */
        {
            int count;

            /* Determine the number of bytes written to the buffer.  This is,
             * of course, also the number of bytes that may be read from the
             * buffer.
             *
             *   d_rdndx - index to remove next byte from the buffer
             *   d_wrndx - Index to next location to add a byte to the buffer.
             */

            if (dev->d_wrndx < dev->d_rdndx) {
                count = (dev->d_bufsize - dev->d_rdndx) + dev->d_wrndx;
            } else {
                count = dev->d_wrndx - dev->d_rdndx;
            }

            *(int *)((uintptr_t)arg) = count;
            ret = 0;
        }
        break;

        /* Free space in buffer */

        case FIONSPACE:
        {
            int count;

            /* Determine the number of bytes free in the buffer.
             *
             *   d_rdndx - index to remove next byte from the buffer
             *   d_wrndx - Index to next location to add a byte to the buffer.
             */

            if (dev->d_wrndx < dev->d_rdndx) {
                count = (dev->d_rdndx - dev->d_wrndx) - 1;
            } else {
                count = ((dev->d_bufsize - dev->d_wrndx) + dev->d_rdndx) - 1;
            }

            *(int *)((uintptr_t)arg) = count;
            ret = 0;
        }
        break;

      default:
        break;
    }

    aos_sem_signal(&dev->d_bfsem);
    return ret;
}
