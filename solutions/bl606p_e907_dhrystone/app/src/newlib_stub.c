/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <reent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <drv/uart.h>

extern csi_uart_t g_console_handle;
int _isatty_r(struct _reent *ptr, int fd)
{
    if (fd >= 0 && fd < 3) {
        return 1;
    }

    ptr->_errno = ENOTSUP;
    return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    if (buf == NULL) {
        return 0;
    }
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        char *ch = (char *)buf;
        for (int i = 0; i < nbytes; i ++) {

            if (ch[i] == '\n') {
                csi_uart_putc(&g_console_handle, '\r');
            }
        
            csi_uart_putc(&g_console_handle, ch[i]);
        }
        return nbytes;
    }
    return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    ptr->_errno = ENOTSUP;
    return NULL;
}
