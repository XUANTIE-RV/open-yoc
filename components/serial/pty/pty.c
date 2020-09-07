/****************************************************************************
 * serial/pty/pty.c
 *
 *   Copyright (C) 2016-2018 Gregory Nutt. All rights reserved.
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
#include <termios.h>
#include <pipe.h>
#include <pty.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Maximum number of threads than can be waiting for POLL events */

#ifndef CONFIG_DEV_PTY_NPOLLWAITERS
#  define CONFIG_DEV_PTY_NPOLLWAITERS 2
#endif

#ifndef CONFIG_PSEUDOTERM_TXBUFSIZE
#  define CONFIG_PSEUDOTERM_TXBUFSIZE 256
#endif

#ifndef CONFIG_PSEUDOTERM_RXBUFSIZE
#  define CONFIG_PSEUDOTERM_RXBUFSIZE 256
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct pty_poll_s
{
    void *src;
    void *sink;
};

/* This device structure describes on memory of the PTY device pair */

struct pty_devpair_s;
struct pty_dev_s
{
    struct pty_devpair_s *pd_devpair;
    int pd_src;                /* Provides data to read() method (pipe output) */
    int pd_sink;               /* Accepts data from write() method (pipe input) */
    bool pd_master;               /* True: this is the master */

#ifdef CONFIG_SERIAL_TERMIOS
    /* Terminal control flags */

    tcflag_t pd_iflag;            /* Terminal nput modes */
    tcflag_t pd_oflag;            /* Terminal output modes */
#endif

#ifndef CONFIG_DISABLE_POLL
    struct pty_poll_s pd_poll[CONFIG_DEV_PTY_NPOLLWAITERS];
#endif
};

/* This structure describes the pipe pair */

struct pty_devpair_s
{
    struct pty_dev_s pp_master;   /* Maseter device */
    struct pty_dev_s pp_slave;    /* Slave device */

    bool pp_locked;               /* Slave is locked */
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
    uint8_t pp_minor;             /* Minor device number */
    uint16_t pp_nopen;            /* Open file count */
#endif
    aos_sem_t pp_slavesem;        /* Slave lock semaphore */
    aos_sem_t pp_exclsem;         /* Mutual exclusion */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void    pty_semtake(struct pty_devpair_s *devpair);
static void    pty_destroy(struct pty_devpair_s *devpair);

static int     pty_open(inode_t *node, file_t *filep);
static int     pty_close(file_t *filep);
static ssize_t pty_read(file_t *filep, void *buffer, size_t buflen);
static ssize_t pty_write(file_t *filep, const void *buffer, size_t buflen);
static int     pty_ioctl(file_t *filep, int cmd, unsigned long arg);
#ifndef CONFIG_DISABLE_POLL
static int     pty_poll(file_t *filep, bool setup, poll_notify_t notify, struct pollfd *fds, void *arg);
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const file_ops_t g_pty_fops =
{
    pty_open,      /* open */
    pty_close,     /* close */
    pty_read,      /* read */
    pty_write,     /* write */
    pty_ioctl      /* ioctl */
#ifndef CONFIG_DISABLE_POLL
    , pty_poll     /* poll */
#endif
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pty_semtake
 ****************************************************************************/

static void pty_semtake(struct pty_devpair_s *devpair)
{
    int ret;

    do {
        /* Take the semaphore (perhaps waiting) */

        ret = aos_sem_wait(&devpair->pp_exclsem, AOS_WAIT_FOREVER);

        /* The only case that an error should occur here is if the wait was
         * awakened by a signal.
         */

        aos_assert(ret == OK || ret == -EINTR);
    } while (ret == -EINTR);
}

/****************************************************************************
 * Name: pty_semgive
 ****************************************************************************/

#define pty_semgive(c) aos_sem_signal(&(c)->pp_exclsem)

/****************************************************************************
 * Name: pty_destroy
 ****************************************************************************/

static void pty_destroy(struct pty_devpair_s *devpair)
{
    char devname[16];

    /* Un-register the slave device */

    snprintf(devname, 16, "/dev/ttyp%d", devpair->pp_minor);
    (void)aos_unregister_driver(devname);

    /* Un-register the master device (/dev/ptyN may have already been
     * unlinked).
     */

    snprintf(devname, 16, "/dev/pty%d", (int)devpair->pp_minor);
    (void)aos_unregister_driver(devname);

    /* Close the contained file structures */

    (void)aos_close(devpair->pp_master.pd_src);
    (void)aos_close(devpair->pp_master.pd_sink);
    (void)aos_close(devpair->pp_slave.pd_src);
    (void)aos_close(devpair->pp_slave.pd_sink);

#ifdef CONFIG_PSEUDOTERM_SUSV1
    /* Free this minor number so that it can be reused */

    ptmx_minor_free(devpair->pp_minor);
#endif

    /* And free the device structure */

    aos_sem_free(&devpair->pp_exclsem);
    aos_free(devpair);
}

/****************************************************************************
 * Name: pty_open
 ****************************************************************************/

static int pty_open(inode_t *node, file_t *filep)
{
    inode_t *inode;
    struct pty_dev_s *dev;
    struct pty_devpair_s *devpair;

    aos_assert(filep != NULL && filep->node != NULL);
    inode   = filep->node;
    dev     = (struct pty_dev_s *)inode->i_arg;
    aos_assert(dev != NULL && dev->pd_devpair != NULL);
    devpair = dev->pd_devpair;

    /* Wait if this is an attempt to open the slave device and the slave
     * device is locked.
     */

    if (!dev->pd_master) {
        /* Slave... Check if the slave driver is locked.  We need to lock the
         * scheduler while we are running to prevent asyncrhonous modification
         * of pp_locked by pty_ioctl().
         */

        aos_kernel_sched_suspend();
        while (devpair->pp_locked) {
            /* Wait until unlocked.  We will also most certainly suspend here. */

            (void)aos_sem_wait(&devpair->pp_slavesem, AOS_WAIT_FOREVER);

            /* Get exclusive access to the device structure.  This might also
             * cause suspension.
             */

            pty_semtake(devpair);

            /* Check again in case something happened asynchronously while we
             * were suspended.
             */

            if (devpair->pp_locked) {
                /* This cannot suspend because we have the scheduler locked.
                 * So pp_locked cannot change asyncrhonously between this test
                 * and the redundant test at the top of the loop.
                 */

                pty_semgive(devpair);
            }
        }

        aos_kernel_sched_resume();
    } else {
        /* Master ... Get exclusive access to the device structure */

        pty_semtake(devpair);
    }

    /* Increment the count of open references on the driver */

    devpair->pp_nopen++;
    aos_assert(devpair->pp_nopen > 0);

    pty_semgive(devpair);
    return OK;
}

/****************************************************************************
 * Name: pty_open
 ****************************************************************************/

static int pty_close(file_t *filep)
{
    inode_t *inode;
    struct pty_dev_s *dev;
    struct pty_devpair_s *devpair;

    aos_assert(filep != NULL && filep->node != NULL);
    inode   = filep->node;
    dev     = (struct pty_dev_s *)inode->i_arg;
    aos_assert(dev != NULL && dev->pd_devpair != NULL);
    devpair = dev->pd_devpair;

    /* Get exclusive access */

    pty_semtake(devpair);

    /* Is this the last open reference?  If so, was the driver previously
     * unlinked?
     */

    aos_assert(devpair->pp_nopen > 0);
    if (devpair->pp_nopen <= 1) {
        /* Yes.. Free the device pair now (without freeing the semaphore) */

        pty_destroy(devpair);
        return OK;
    } else {
        /* Otherwise just decrement the open count */

        devpair->pp_nopen--;
    }

    pty_semgive(devpair);
    return OK;
}

/****************************************************************************
 * Name: pty_read
 ****************************************************************************/

static ssize_t pty_read(file_t *filep, void *buffer, size_t len)
{
    inode_t *inode;
    struct pty_dev_s *dev;
    char *buf = (char *)buffer;
    ssize_t ntotal;
#ifdef CONFIG_SERIAL_TERMIOS
    ssize_t nread;
    size_t i;
    char ch;
    int ret;
#endif

    aos_assert(filep != NULL && filep->node != NULL);
    inode = filep->node;
    dev   = (struct pty_dev_s *)inode->i_arg;
    aos_assert(dev != NULL);

#ifdef CONFIG_SERIAL_TERMIOS
    /* Do input processing if any is enabled
     *
     * Specifically not handled:
     *
     *   All of the local modes; echo, line editing, etc.
     *   Anything to do with break or parity errors.
     *   ISTRIP     - We should be 8-bit clean.
     *   IUCLC      - Not Posix
     *   IXON/OXOFF - No xon/xoff flow control.
     */

    if (dev->pd_iflag & (INLCR | IGNCR | ICRNL)) {
        /* We will transfer one byte at a time, making the appropriate
         * translations.
         */

        ntotal = 0;
        for (i = 0; i < len; i++) {
            /* This logic should return if the pipe becomes empty after some
             * bytes were read from the pipe.  If we have already read some
             * data, we use the FIONREAD ioctl to test if there are more bytes
             * in the pipe.
             *
             * REVISIT:  An alternative design might be to (1) configure the
             * source file as non-blocking, then (2) wait using poll() for the
             * first byte to be received.  (3) Subsequent bytes would
             * use file_read() without polling and would (4) terminate when no
             * data is returned.
             */

            if (ntotal > 0) {
                int nsrc;

                /* There are inherent race conditions in this test.  We lock
                 * the scheduler before the test and after the file_read()
                 * below to eliminate one race:  (a) We detect that there is
                 * data in the source file, (b) we are suspended and another
                 * thread reads the data, emptying the fifo, then (c) we
                 * resume and call file_read(), blocking indefinitely.
                 */

                aos_kernel_sched_suspend();

                /* Check how many bytes are waiting in the pipe */

                ret = aos_ioctl(dev->pd_src, FIONREAD,
                                (unsigned long)((uintptr_t)&nsrc));
                if (ret < 0) {
                    aos_kernel_sched_resume();
                    ntotal = ret;
                    break;
                }

                /* Break out of the loop and return ntotal if the pipe is
                 * empty.  This is another race:  There fifo was empty when we
                 * called file_ioctl() above, but it might not be empty right
                 * now.  Losing that race should not lead to any bad behaviors,
                 * however, we the caller will get those bytes on the next
                 * read.
                 */

                if (nsrc < 1) {
                    aos_kernel_sched_resume();
                    break;
                }

                /* Read one byte from the source the byte.  This should not
                 * block.
                 */

                nread = aos_read(dev->pd_src, &ch, 1);
                aos_kernel_sched_resume();
            } else {
                /* Read one byte from the source the byte.  This call will
                 * block if the source pipe is empty.
                 *
                 * REVISIT: Should not block if the oflags include O_NONBLOCK.
                 * How would we ripple the O_NONBLOCK characteristic to the
                 * contained soruce pipe?  file_vfcntl()?  Or FIONREAD? See the
                 * TODO comment at the top of this file.
                 */

                nread = aos_read(dev->pd_src, &ch, 1);
            }

            /* Check if file_read was successful */

            if (nread < 0) {
                ntotal = nread;
                break;
            }

            /* Perform input processing */
            /* \n -> \r or \r -> \n translation? */

            if (ch == '\n' && (dev->pd_iflag & INLCR) != 0) {
                ch = '\r';
            } else if (ch == '\r' && (dev->pd_iflag & ICRNL) != 0) {
                ch = '\n';
            }

            /* Discarding \r ?  Print character if (1) character is not \r or
             * if (2) we were not asked to ignore \r.
             */

            if (ch != '\r' || (dev->pd_iflag & IGNCR) == 0) {

                /* Transfer the (possibly translated) character and update the
                 * count of bytes transferred.
                 */

                *buf++ = ch;
                ntotal++;
            }
        }
    }
    else
#endif
    {
        /* NOTE: the source pipe will block if no data is available in
         * the pipe.   Otherwise, it will return data from the pipe.  If
         * there are fewer than 'len' bytes in the, it will return with
         * ntotal < len.
         *
         * REVISIT: Should not block if the oflags include O_NONBLOCK.
         * How would we ripple the O_NONBLOCK characteristic to the
         * contained source pipe? file_vfcntl()?  Or FIONREAD?  See the
         * TODO comment at the top of this file.
         */

        ntotal = aos_read(dev->pd_src, buf, len);
    }

    return ntotal;
}

/****************************************************************************
 * Name: pty_write
 ****************************************************************************/

static ssize_t pty_write(file_t *filep, const void *buffer, size_t len)
{
    inode_t *inode;
    struct pty_dev_s *dev;
    char *buf = (char *)buffer;
    ssize_t ntotal;
#ifdef CONFIG_SERIAL_TERMIOS
    ssize_t nwritten;
    size_t i;
    char ch;
#endif

    aos_assert(filep != NULL && filep->node != NULL);
    inode = filep->node;
    dev   = (struct pty_dev_s *)inode->i_arg;
    aos_assert(dev != NULL);

#ifdef CONFIG_SERIAL_TERMIOS
    /* Do output post-processing */

    if ((dev->pd_oflag & OPOST) != 0) {
        /* We will transfer one byte at a time, making the appropriae
         * translations.  Specifically not handled:
         *
         *   OXTABS - primarily a full-screen terminal optimisation
         *   ONOEOT - Unix interoperability hack
         *   OLCUC  - Not specified by POSIX
         *   ONOCR  - low-speed interactive optimisation
         */

        ntotal = 0;
        for (i = 0; i < len; i++) {
            ch = *buf++;

            /* Mapping CR to NL? */

            if (ch == '\r' && (dev->pd_oflag & OCRNL) != 0) {
                ch = '\n';
            }

            /* Are we interested in newline processing? */

            if ((ch == '\n') && (dev->pd_oflag & (ONLCR | ONLRET)) != 0) {
                char cr = '\r';

                /* Transfer the carriage return.  This will block if the
                 * sink pipe is full.
                 *
                 * REVISIT: Should not block if the oflags include O_NONBLOCK.
                 * How would we ripple the O_NONBLOCK characteristic to the
                 * contained sink pipe?  file_vfcntl()?  Or FIONSPACE?  See the
                 * TODO comment at the top of this file.
                 */

                nwritten = aos_write(dev->pd_sink, &cr, 1);
                if (nwritten < 0) {
                    ntotal = nwritten;
                    break;
                }

                /* Update the count of bytes transferred */

                ntotal++;
            }

            /* Transfer the (possibly translated) character..  This will block
             * if the sink pipe is full
             *
             * REVISIT: Should not block if the oflags include O_NONBLOCK.
             * How would we ripple the O_NONBLOCK characteristic to the
             * contained sink pipe?  file_vfcntl()?  Or FIONSPACe?  See the
             * TODO comment at the top of this file.
             */

            nwritten = aos_write(dev->pd_sink, &ch, 1);
            if (nwritten < 0) {
                ntotal = nwritten;
                break;
            }

            /* Update the count of bytes transferred */

            ntotal++;
        }
    }
    else
#endif
    {
        /* Write the 'len' bytes to the sink pipe.  This will block until all
         * 'len' bytes have been written to the pipe.
         *
         * REVISIT: Should not block if the oflags include O_NONBLOCK.
         * How would we ripple the O_NONBLOCK characteristic to the
         * contained sink pipe?  file_vfcntl()?  Or FIONSPACE?  See the
         * TODO comment at the top of this file.
         */

        ntotal = aos_write(dev->pd_sink, buf, len);
    }

    return ntotal;
}

/****************************************************************************
 * Name: pty_ioctl
 *
 * Description:
 *   The standard ioctl method.  This is where ALL of the PWM work is done.
 *
 ****************************************************************************/

static int pty_ioctl(file_t *filep, int cmd, unsigned long arg)
{
    inode_t *inode;
    struct pty_dev_s *dev;
    struct pty_devpair_s *devpair;
    int ret;

    aos_assert(filep != NULL && filep->node != NULL);
    inode   = filep->node;
    dev     = (struct pty_dev_s *)inode->i_arg;
    aos_assert(dev != NULL && dev->pd_devpair != NULL);
    devpair = dev->pd_devpair;

    /* Get exclusive access */

    pty_semtake(devpair);

    /* Handle IOCTL commands */

    switch (cmd) {
        /* PTY IOCTL commands would be handled here */

        case TIOCGPTN:    /* Get Pty Number (of pty-mux device): int* */
        {
#ifdef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
            ret = -ENOSYS;
#else
            int *ptyno = (int *)((uintptr_t)arg);
            if (ptyno == NULL) {
                ret = -EINVAL;
            } else {
                *ptyno = (int)devpair->pp_minor;
                ret = OK;
            }
#endif
        }
        break;

        case TIOCSPTLCK:  /* Lock/unlock Pty: int */
        {
            if (arg == 0) {

                /* Unlocking */

                aos_kernel_sched_suspend();
                devpair->pp_locked = false;

                /* Release any waiting threads */

                aos_sem_signal_all(&devpair->pp_slavesem);

                aos_kernel_sched_resume();
                ret = OK;
            } else {
                /* Locking */

                devpair->pp_locked = true;
                ret = OK;
            }
        }
        break;

        case TIOCGPTLCK:  /* Get Pty lock state: int* */
        {
            int *ptr = (int *)((uintptr_t)arg);
            if (ptr == NULL) {
                ret = -EINVAL;
            } else {
                *ptr = (int)devpair->pp_locked;
                ret = OK;
            }
        }
        break;

#ifdef CONFIG_SERIAL_TERMIOS
        case TCGETS:
        {
            struct termios *termiosp = (struct termios *)arg;

            if (!termiosp) {
                ret = -EINVAL;
                break;
            }

            /* And update with flags from this layer */

            termiosp->c_iflag = dev->pd_iflag;
            termiosp->c_oflag = dev->pd_oflag;
            termiosp->c_lflag = 0;
            ret = OK;
        }
        break;

        case TCSETS:
        {
            struct termios *termiosp = (struct termios *)arg;

            if (!termiosp) {
                ret = -EINVAL;
                break;
            }

            /* Update the flags we keep at this layer */

            dev->pd_iflag = termiosp->c_iflag;
            dev->pd_oflag = termiosp->c_oflag;
            ret = OK;
        }
        break;
#endif

        /* Get the number of bytes that are immediately available for reading
         * from the source pipe.
         */

        case FIONREAD:
        {
            ret = aos_ioctl(dev->pd_src, cmd, arg);
        }
        break;

        /* Get the number of bytes waiting in the sink pipe (FIONWRITE) or the
         * number of unused bytes in the sink pipe (FIONSPACE).
         */

        case FIONWRITE:
        case FIONSPACE:
        {
            ret = aos_ioctl(dev->pd_sink, cmd, arg);
        }
        break;

        /* Any unrecognized IOCTL commands will be passed to the contained
         * pipe driver.
         *
         * REVISIT:  We know for a fact that the pipe driver only supports
         * FIONREAD, FIONWRITE, FIONSPACE and PIPEIOC_POLICY.  The first two
         * are handled above and PIPEIOC_POLICY should not be managed by
         * applications -- it can break the PTY!
         */

        default:
        {
#if 0
            ret = file_ioctl(&dev->pd_src, cmd, arg);
            if (ret >= 0 || ret == -ENOTTY) {
                ret = file_ioctl(&dev->pd_sink, cmd, arg);
            }
#else
            ret = ENOTTY;
#endif
        }
        break;
    }

    pty_semgive(devpair);
    return ret;
}

/****************************************************************************
 * Name: pty_poll
 ****************************************************************************/

#ifndef CONFIG_DISABLE_POLL
static int pty_poll(file_t *filep, bool setup, poll_notify_t notify, struct pollfd *fds, void *arg)
{
    inode_t *inode;
    struct pty_dev_s *dev;
    struct pty_devpair_s *devpair;
    struct pty_poll_s *pollp = NULL;
    int ret = -ENOSYS;
    int i;

    aos_assert(filep != NULL && filep->f_inode != NULL);
    inode   = filep->f_inode;
    dev     = inode->i_private;
    devpair = dev->pd_devpair;

    pty_semtake(devpair);

    if (setup) {
        for (i = 0; i < CONFIG_DEV_PTY_NPOLLWAITERS; i++) {
            if (dev->pd_poll[i].src == NULL && dev->pd_poll[i].sink == NULL) {
                pollp = &dev->pd_poll[i];
                break;
            }
        }

        if (i >= CONFIG_DEV_PTY_NPOLLWAITERS) {
            ret = -EBUSY;
            goto errout;
        }
    }
    else {
        pollp = (struct pty_poll_s *)fds->priv;
    }

    /* POLLIN: Data other than high-priority data may be read without blocking. */

    if ((fds->events & POLLIN) != 0) {
        fds->priv = pollp->src;
        ret = file_poll(&dev->pd_src, fds, setup);
        if (ret < 0) {
            goto errout;
        }

        pollp->src = fds->priv;
    }

    /* POLLOUT: Normal data may be written without blocking. */

    if ((fds->events & POLLOUT) != 0) {
        fds->priv = pollp->sink;
        ret = file_poll(&dev->pd_sink, fds, setup);
        if (ret < 0) {
            if (pollp->src) {
                fds->priv = pollp->src;
                file_poll(&dev->pd_src, fds, false);
                pollp->src = NULL;
            }

            goto errout;
        }
        pollp->sink = fds->priv;
    }

    if (setup) {
        fds->priv = pollp;
    }

errout:
    pty_semgive(devpair);
    return ret;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pty_register
 *
 * Description:
 *   Create and register PTY master and slave devices.  The slave side of
 *   the interface is always locked initially.  The master must call
 *   unlockpt() before the slave device can be opened.
 *
 * Input Parameters:
 *   minor - The number that qualifies the naming of the created devices.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is returned on
 *   any failure.
 *
 ****************************************************************************/

int pty_register(int minor)
{
    struct pty_devpair_s *devpair;
    int pipe_a[2];
    int pipe_b[2];
    char devname[16];
    int ret;

    /* Allocate a device instance */

    devpair = aos_zalloc(sizeof(struct pty_devpair_s));
    if (devpair == NULL) {
        return -ENOMEM;
    }

    /* Initialize semaphores */

    aos_sem_new(&devpair->pp_slavesem, 0);
    aos_sem_new(&devpair->pp_exclsem,  1);

    devpair->pp_minor             = minor;
    devpair->pp_locked            = true;
    devpair->pp_master.pd_devpair = devpair;
    devpair->pp_master.pd_master  = true;
    devpair->pp_slave.pd_devpair  = devpair;

    /* Create two pipes:
     *
     *   pipe_a:  Master source, slave sink (TX, slave-to-master)
     *   pipe_b:  Master sink, slave source (RX, master-to-slave)
     */

    ret = pipe2(pipe_a, CONFIG_PSEUDOTERM_TXBUFSIZE);
    if (ret < 0) {
        goto errout_with_devpair;
    }

    ret = pipe2(pipe_b, CONFIG_PSEUDOTERM_RXBUFSIZE);
    if (ret < 0) {
        goto errout_with_pipea;
    }

    /* Detach the pipe file descriptors (closing them in the process)
     *
     *  fd[0] is for reading;
     *  fd[1] is for writing.
     */

    devpair->pp_master.pd_src = pipe_a[0];
    pipe_a[0] = -1;

    devpair->pp_slave.pd_sink = pipe_a[1];
    pipe_a[1] = -1;

    devpair->pp_slave.pd_src = pipe_b[0];
    pipe_b[0] = -1;

    devpair->pp_master.pd_sink = pipe_b[1];
    pipe_b[1] = -1;

    /* Register the slave device
     *
     * BSD style (deprecated): /dev/ttypN
     * SUSv1 style:  /dev/pts/N
     *
     * Where N is the minor number
     */

    snprintf(devname, 16, "/dev/ttyp%d", minor);

    ret = aos_register_driver(devname, &g_pty_fops, &devpair->pp_slave);
    if (ret < 0) {
        goto errout_with_pipeb;
    }

    /* Register the master device
     *
     * BSD style (deprecated):  /dev/ptyN
     * SUSv1 style: Master: /dev/ptmx (multiplexor, see ptmx.c)
     *
     * Where N is the minor number
     */

    snprintf(devname, 16, "/dev/pty%d", minor);

    ret = aos_register_driver(devname, &g_pty_fops, &devpair->pp_master);
    if (ret < 0) {
        goto errout_with_slave;
    }

    return OK;

errout_with_slave:
    snprintf(devname, 16, "/dev/ttyp%d", minor);
    (void)aos_unregister_driver(devname);

errout_with_pipeb:
    (void)aos_close(devpair->pp_master.pd_src);
    (void)aos_close(devpair->pp_slave.pd_sink);

errout_with_pipea:
    (void)aos_close(devpair->pp_slave.pd_src);
    (void)aos_close(devpair->pp_master.pd_sink);

errout_with_devpair:
    aos_sem_free(&devpair->pp_exclsem);
    aos_sem_free(&devpair->pp_slavesem);
    aos_free(devpair);
    return ret;
}

int pty_unlock(int fd)
{
    return aos_ioctl(fd, TIOCSPTLCK, 0);
}
