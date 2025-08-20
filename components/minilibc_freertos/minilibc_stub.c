 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <drv/uart.h>
#include <drv/tick.h>

extern csi_uart_t g_console_handle;

int fputc(int ch, FILE *stream)
{
    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}

int fgetc(FILE *stream)
{
    (void)stream;

    return csi_uart_getc(&g_console_handle);
}

int putc(int c, FILE *stream)
{
    return fputc(c, stream);
}

int puts(const char *s)
{
    while(*s !='\0') {
        fputc(*s, (void *)-1);
        s++;
    }
    fputc('\n', (void *)-1);
    return 0;
}

static void _putchar(char character)
{
    if (character == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, character);

}

int putchar(int c)
{
    _putchar(c);
    return 0;
}

_ssize_t write(int fd, const void *buf, size_t nbytes)
{
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        for (int i = 0; i < nbytes; i++)
            _putchar((*((char*)buf + i)));
        return nbytes;
    } else {
        return -1;
    }
}

int ioctl(int fildes, int request, ... /* arg */)
{
    return -1;
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    uint32_t t;

    if (tv) {
        t = csi_tick_get_ms();
        tv->tv_sec = t / 1000;
        tv->tv_usec = (t % 1000) * 1000;        
    }

    if (tz) {
        /* Not supported. */
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }
    return 0;
}

long timezone = 8; /* default CTS */

struct tm* localtime_r(const time_t* t, struct tm* r)
{
    time_t time_tmp;
    time_tmp = *t + timezone * 3600;
    return gmtime_r(&time_tmp, r);
}

struct tm* localtime(const time_t* t)
{
    struct tm* timeinfo;
    static struct tm tm_tmp;

    timeinfo = localtime_r(t, &tm_tmp);

    return timeinfo;
}

void *malloc(size_t size)
{
    return pvPortMalloc(size);
}

void *realloc(void *old, size_t newlen)
{
    return pvPortRealloc(old, newlen);
}

void *calloc(size_t size, size_t len)
{
    void *mem = pvPortMalloc(size * len);
    if (mem) {
        memset(mem, 0, size * len);
    }

    return mem;
}

void *memalign(size_t alignment, size_t size)
{
    return NULL;
}

void free(void *addr)
{
    if (!addr)
        return;
    vPortFree(addr);
}

void _exit(int status)
{
    while (1)
        ;
}

void exit(int status)
{
    __builtin_unreachable(); // fix noreturn warning
}

__attribute__((weak)) void _fini()
{
}

void _system(const char *s)
{
    return;
}

void abort(void)
{
    __builtin_unreachable(); // fix noreturn warning
}

int isatty(int fd)
{
    if (fd == fileno(stdin) || fd == fileno(stdout) || fd == fileno(stderr)) {
        return -1;
    }
    return 0;
}

