/************************************************************************************
 * rptun/serial/serial.h
 *
 *   Copyright (C) 2007-2009, 2011-2013, 2016-2018 Gregory Nutt. All rights reserved.
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
 ************************************************************************************/

#ifndef _SERIAL_H
#define _SERIAL_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#ifdef CONFIG_SERIAL_TERMIOS
#  include <termios.h>
#endif

#include <devices/device.h>

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

/* Maximum number of threads than can be waiting for POLL events */

#ifndef CONFIG_SERIAL_NPOLLWAITERS
#  define CONFIG_SERIAL_NPOLLWAITERS 2
#endif

/* RX flow control */

#ifndef CONFIG_SERIAL_IFLOWCONTROL
#  undef CONFIG_SERIAL_IFLOWCONTROL_WATERMARKS
#endif

#ifndef CONFIG_SERIAL_IFLOWCONTROL_WATERMARKS
#  undef CONFIG_SERIAL_IFLOWCONTROL_LOWER_WATERMARK
#  undef CONFIG_SERIAL_IFLOWCONTROL_UPPER_WATERMARK
#endif

#ifdef CONFIG_SERIAL_IFLOWCONTROL_WATERMARKS
#  ifndef CONFIG_SERIAL_IFLOWCONTROL_LOWER_WATERMARK
#    define CONFIG_SERIAL_IFLOWCONTROL_LOWER_WATERMARK 10
#  endif

#  ifndef CONFIG_SERIAL_IFLOWCONTROL_UPPER_WATERMARK
#    define CONFIG_SERIAL_IFLOWCONTROL_UPPER_WATERMARK 90
#  endif

#  if CONFIG_SERIAL_IFLOWCONTROL_LOWER_WATERMARK > \
      CONFIG_SERIAL_IFLOWCONTROL_UPPER_WATERMARK
#    warning Lower watermark pct exceeds upper watermark pct
#  endif
#endif

/* vtable access helpers */

#ifndef CONFIG_SERIAL_USE_VFS
#define serial_earlyinit(dev)      dev->ops->earlyinit(dev)
#endif
#define serial_setup(dev)          dev->ops->setup(dev)
#define serial_shutdown(dev)       dev->ops->shutdown(dev)
#define serial_attach(dev)         dev->ops->attach(dev)
#define serial_detach(dev)         dev->ops->detach(dev)
#define serial_enabletxint(dev)    dev->ops->txint(dev, true)
#define serial_disabletxint(dev)   dev->ops->txint(dev, false)
#define serial_enablerxint(dev)    dev->ops->rxint(dev, true)
#define serial_disablerxint(dev)   dev->ops->rxint(dev, false)
#define serial_rxavailable(dev)    dev->ops->rxavailable(dev)
#define serial_txready(dev)        dev->ops->txready(dev)
#define serial_txempty(dev)        dev->ops->txempty(dev)
#define serial_send(dev,ch)        dev->ops->send(dev,ch)
#define serial_receive(dev,s)      dev->ops->receive(dev,s)

#ifdef CONFIG_SERIAL_DMA
#define serial_dmasend(dev)      \
  ((dev)->ops->dmasend ? (dev)->ops->dmasend(dev) : -ENOSYS)

#define serial_dmareceive(dev)   \
  ((dev)->ops->dmareceive ? (dev)->ops->dmareceive(dev) : -ENOSYS)

#define serial_dmarxfree(dev)    \
  ((dev)->ops->dmarxfree ? (dev)->ops->dmarxfree(dev) : -ENOSYS)

#define serial_dmatxavail(dev)   \
  ((dev)->ops->dmatxavail ? (dev)->ops->dmatxavail(dev) : -ENOSYS)

#endif

#ifdef CONFIG_SERIAL_IFLOWCONTROL
#  define serial_rxflowcontrol(dev,n,u) \
    (dev->ops->rxflowcontrol && dev->ops->rxflowcontrol(dev,n,u))
#endif

/************************************************************************************
 * Public Types
 ************************************************************************************/

/* This structure defines one serial I/O buffer.  The serial infrastructure will
 * initialize the 'sem' field but all other fields must be initialized by the
 * caller of serial_register().
 */

struct serial_buffer_s
{
    aos_sem_t        sem;    /* Used to control exclusive access to the buffer */
    volatile int16_t head;   /* Index to the head [IN] index in the buffer */
    volatile int16_t tail;   /* Index to the tail [OUT] index in the buffer */
    int16_t          size;   /* The allocated size of the buffer */
    char             *buffer; /* Pointer to the allocated buffer memory */
};

#ifdef CONFIG_SERIAL_DMA
struct serial_dmaxfer_s
{
    char            *buffer;  /* First DMA buffer */
    char            *nbuffer; /* Next DMA buffer */
    size_t          length;  /* Length of first DMA buffer */
    size_t          nlength; /* Length of next DMA buffer */
    size_t          nbytes;  /* Bytes actually transferred by DMA from both buffers */
};
#endif /* CONFIG_SERIAL_DMA */

/* This structure defines all of the operations providd by the architecture specific
 * logic.  All fields must be provided with non-NULL function pointers by the
 * caller of serial_register().
 */

struct serial_dev_s;
struct serial_ops_s
{
#ifndef CONFIG_SERIAL_USE_VFS
    int (*earlyinit)(struct serial_dev_s *dev);
#endif

    /* Configure the UART baud, bits, parity, fifos, etc. This method is called
    * the first time that the serial port is opened.  For the serial console,
    * this will occur very early in initialization; for other serial ports this
    * will occur when the port is first opened.  This setup does not include
    * attaching or enabling interrupts.  That portion of the UART setup is
    * performed when the attach() method is called.
    */

    int (*setup)(struct serial_dev_s *dev);

    /* Disable the UART.  This method is called when the serial port is closed.
    * This method reverses the operation the setup method.  NOTE that the serial
    * console is never shutdown.
    */

    void (*shutdown)(struct serial_dev_s *dev);

    /* Configure the UART to operation in interrupt driven mode.  This method is
    * called when the serial port is opened.  Normally, this is just after the
    * the setup() method is called, however, the serial console may operate in
    * a non-interrupt driven mode during the boot phase.
    *
    * RX and TX interrupts are not enabled when by the attach method (unless the
    * hardware supports multiple levels of interrupt enabling).  The RX and TX
    * interrupts are not enabled until the txint() and rxint() methods are called.
    */

    int (*attach)(struct serial_dev_s *dev);

    /* Detach UART interrupts.  This method is called when the serial port is
    * closed normally just before the shutdown method is called.  The exception is
    * the serial console which is never shutdown.
    */

    void (*detach)(struct serial_dev_s *dev);

    /* All ioctl calls will be routed through this method */

    int (*ioctl)(file_t *filep, int cmd, unsigned long arg);

    /* Called (usually) from the interrupt level to receive one character from
    * the UART.  Error bits associated with the receipt are provided in the
    * the return 'status'.
    */

    int (*receive)(struct serial_dev_s *dev, unsigned int *status);

    /* Call to enable or disable RX interrupts */

    void (*rxint)(struct serial_dev_s *dev, bool enable);

    /* Return true if the receive data is available */

    bool (*rxavailable)(struct serial_dev_s *dev);

#ifdef CONFIG_SERIAL_IFLOWCONTROL
    /* Return true if UART activated RX flow control to block more incoming
    * data.
    */

    bool (*rxflowcontrol)(struct serial_dev_s *dev,
                          unsigned int nbuffered, bool upper);
#endif

#ifdef CONFIG_SERIAL_DMA
    /* Start transfer bytes from the TX circular buffer using DMA */

    void (*dmasend)(struct serial_dev_s *dev);

    /* Start transfer bytes from the TX circular buffer using DMA */

    void (*dmareceive)(struct serial_dev_s *dev);

    /* Notify DMA that there is free space in the RX buffer */

    void (*dmarxfree)(struct serial_dev_s *dev);

    /* Notify DMA that there is data to be transferred in the TX buffer */

    void (*dmatxavail)(struct serial_dev_s *dev);
#endif

    /* This method will send one byte on the UART */

    void (*send)(struct serial_dev_s *dev, int ch);

    /* Call to enable or disable TX interrupts */

    void (*txint)(struct serial_dev_s *dev, bool enable);

    /* Return true if the tranmsit hardware is ready to send another byte.  This
    * is used to determine if send() method can be called.
    */

    bool (*txready)(struct serial_dev_s *dev);

    /* Return true if all characters have been sent.  If for example, the UART
    * hardware implements FIFOs, then this would mean the transmit FIFO is
    * empty.  This method is called when the driver needs to make sure that
    * all characters are "drained" from the TX hardware.
    */

    bool (*txempty)(struct serial_dev_s *dev);
};

/* This is the device structure used by the driver.  The caller of
 * serial_register() must allocate and initialize this structure.  The
 * calling logic need only set all fields to zero except:
 *
 *   'isconsole', 'xmit.buffer', 'rcv.buffer', the elements
 *   of 'ops', and 'private'
 *
 * The common logic will initialize all semaphores.
 */

struct serial_dev_s
{
#ifndef CONFIG_SERIAL_USE_VFS
    aos_dev_t            device;
#endif

    /* State data */

    uint8_t              open_count;   /* Number of times the device has been opened */
    volatile bool        xmitwaiting;  /* true: User waiting for space in xmit.buffer */
    volatile bool        recvwaiting;  /* true: User waiting for data in recv.buffer */
#ifdef CONFIG_SERIAL_REMOVABLE
    volatile bool        disconnected; /* true: Removable device is not connected */
#endif
    bool                 isconsole;    /* true: This is the serial console */

#ifdef CONFIG_SERIAL_TERMIOS
    /* Terminal control flags */

    tcflag_t             tc_iflag;     /* Input modes */
    tcflag_t             tc_oflag;     /* Output modes */
    tcflag_t             tc_lflag;     /* Local modes */
#if defined(CONFIG_TTY_SIGINT) || defined(CONFIG_TTY_SIGSTP)
    pid_t                pid;          /* Thread PID to receive signals (-1 if none) */
#endif
#endif

    /* Semaphores */

    aos_sem_t            closesem;     /* Locks out new open while close is in progress */
    aos_sem_t            xmitsem;      /* Wakeup user waiting for space in xmit.buffer */
    aos_sem_t            recvsem;      /* Wakeup user waiting for data in recv.buffer */
#ifndef CONFIG_DISABLE_POLL
    aos_sem_t            pollsem;      /* Manages exclusive access to fds[] */
#endif

    /* I/O buffers */

    struct serial_buffer_s xmit;         /* Describes transmit buffer */
    struct serial_buffer_s recv;         /* Describes receive buffer */

#ifdef CONFIG_SERIAL_DMA

    /* DMA transfers */

    struct serial_dmaxfer_s dmatx;       /* Describes transmit DMA transfer */
    struct serial_dmaxfer_s dmarx;       /* Describes receive DMA transfer */
#endif

    /* Driver interface */

    const struct serial_ops_s *ops;  /* Arch-specific operations */
    void            *priv;         /* Used by the arch-specific logic */

    /* The following is a list if poll structures of threads waiting for
    * driver events. The 'struct pollfd' reference for each open is also
    * retained in the f_priv field of the 'struct file'.
    */

#ifdef CONFIG_SERIAL_USE_VFS
#ifndef CONFIG_DISABLE_POLL
    struct pollfd *fds[CONFIG_SERIAL_NPOLLWAITERS];
#endif
#else
    void (*event)(aos_dev_t *dev, int event_id, void *priv);
    void *event_priv;
#endif
};

typedef struct serial_dev_s serial_dev_t;

#ifndef CONFIG_SERIAL_USE_VFS
struct serial_dev_config_s {
    const char                  *path;
    int                         uart_idx;
    int                         buf_size;
    bool                        isconsole;
    const struct serial_ops_s   *ops;
    void                        *priv;
};
#endif

/************************************************************************************
 * Public Data
 ************************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: serial_register
 *
 * Description:
 *   Register serial console and serial ports.
 *
 ************************************************************************************/

#ifdef CONFIG_SERIAL_USE_VFS
int serial_register(const char *path, serial_dev_t *dev);
#else
int serial_register(struct serial_dev_config_s *config);
#endif

/************************************************************************************
 * Name: serial_xmitchars
 *
 * Description:
 *   This function is called from the UART interrupt handler when an interrupt
 *   is received indicating that there is more space in the transmit FIFO.  This
 *   function will send characters from the tail of the xmit buffer while the driver
 *   write() logic adds data to the head of the xmit buffer.
 *
 ************************************************************************************/

void serial_xmitchars(serial_dev_t *dev);

/************************************************************************************
 * Name: serial_receivechars
 *
 * Description:
 *   This function is called from the UART interrupt handler when an interrupt
 *   is received indicating that are bytes available to be received.  This
 *   function will add chars to head of receive buffer.  Driver read() logic will take
 *   characters from the tail of the buffer.
 *
 ************************************************************************************/

void serial_recvchars(serial_dev_t *dev);

/************************************************************************************
 * Name: serial_datareceived
 *
 * Description:
 *   This function is called from serial_recvchars when new serial data is place in
 *   the driver's circular buffer.  This function will wake-up any stalled read()
 *   operations that are waiting for incoming data.
 *
 ************************************************************************************/

void serial_datareceived(serial_dev_t *dev);

/************************************************************************************
 * Name: serial_datasent
 *
 * Description:
 *   This function is called from serial_xmitchars after serial data has been sent,
 *   freeing up some space in the driver's circular buffer. This function will
 *   wake-up any stalled write() operations that was waiting for space to buffer
 *   outgoing data.
 *
 ************************************************************************************/

void serial_datasent(serial_dev_t *dev);

/************************************************************************************
 * Name: serial_connected
 *
 * Description:
 *   Serial devices (like USB serial) can be removed.  In that case, the "upper
 *   half" serial driver must be informed that there is no longer a valid serial
 *   channel associated with the driver.
 *
 *   In this case, the driver will terminate all pending transfers wint ENOTCONN and
 *   will refuse all further transactions while the "lower half" is disconnected.
 *   The driver will continue to be registered, but will be in an unusable state.
 *
 *   Conversely, the "upper half" serial driver needs to know when the serial
 *   device is reconnected so that it can resume normal operations.
 *
 * Assumptions/Limitations:
 *   This function may be called from an interrupt handler.
 *
 ************************************************************************************/

#ifdef CONFIG_SERIAL_REMOVABLE
void serial_connected(serial_dev_t *dev, bool connected);
#endif

/************************************************************************************
 * Name: serial_xmitchars_dma
 *
 * Description:
 *   Set up to transfer bytes from the TX circular buffer using DMA
 *
 ************************************************************************************/

#ifdef CONFIG_SERIAL_DMA
void serial_xmitchars_dma(serial_dev_t *dev);
#endif

/************************************************************************************
 * Name: serial_xmitchars_done
 *
 * Description:
 *   Perform operations necessary at the complete of DMA including adjusting the
 *   TX circular buffer indices and waking up of any threads that may have been
 *   waiting for space to become available in the TX circular buffer.
 *
 ************************************************************************************/

#ifdef CONFIG_SERIAL_DMA
void serial_xmitchars_done(serial_dev_t *dev);
#endif

/************************************************************************************
 * Name: serial_recvchars_dma
 *
 * Description:
 *   Set up to receive bytes into the RX circular buffer using DMA
 *
 ************************************************************************************/

#ifdef CONFIG_SERIAL_DMA
void serial_recvchars_dma(serial_dev_t *dev);
#endif

/************************************************************************************
 * Name: serial_recvchars_done
 *
 * Description:
 *   Perform operations necessary at the complete of DMA including adjusting the
 *   RX circular buffer indices and waking up of any threads that may have been
 *   waiting for new data to become available in the RX circular buffer.
 *
 ************************************************************************************/

#ifdef CONFIG_SERIAL_DMA
void serial_recvchars_done(serial_dev_t *dev);
#endif

/************************************************************************************
 * Name: serial_reset_sem
 *
 * Description:
 *   This function is called when need reset uart semphore, this may used in kill one
 *   process, but this process was reading/writing with the semphore.
 *
 ************************************************************************************/

void serial_reset_sem(serial_dev_t *dev);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* _SERIAL_H */
