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

#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <aos/kernel.h>
#include <umm.h>

#if defined (__CC_ARM)
#pragma weak  __aeabi_assert
void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}

#pragma weak __stack_chk_fail
void __stack_chk_fail(void)
{
}

unsigned long __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

#pragma weak  gettimeofday
int gettimeofday(struct timeval *tv, void *tzp)
{
    uint64_t t;

    t = aos_now_ms();

    tv->tv_sec  = t / 1000;
    tv->tv_usec = (t % 1000) * 1000;

    return 0;
}

#pragma weak malloc
void *malloc(size_t size)
{
    void *mem;
    mem = umm_alloc(size);

    return mem;
}

#pragma weak free
void free(void *mem)
{
    umm_free(mem);
}

#pragma weak realloc
void *realloc(void *old, size_t newlen)
{
    void *mem;

    mem = umm_realloc(old, newlen);

    return mem;
}

#pragma weak calloc
void *calloc(size_t len, size_t elsize)
{
    void *mem;
    mem = umm_alloc(elsize * len);

    if (mem) {
        memset(mem, 0, elsize * len);
    }

    return mem;
}
#pragma weak strdup
char * strdup(const char *s)
{
    size_t  len = strlen(s) +1;
    void *dup_str = umm_alloc(len);
    if (dup_str == NULL)
        return NULL;

    return (char *)memcpy(dup_str, s, len);
}

#pragma weak fputc
int fputc(int ch, FILE *f)
{
    extern int uart_write(const void *buf, size_t size);
    uart_write((uint8_t *)(&ch), 1);
    return ch;
}
#pragma weak  bzero
/* referred from ota_socket.o */
void bzero()
{

}

/* referred from ssl_cli.o */
#pragma weak time
time_t time(time_t *t)
{
    return 0;
}

/* referred from aos_network.o */
#pragma weak accept
int accept(int sock, long *addr, long *addrlen)
{
    return 0;
}

#pragma weak listen
int listen(int sock, int backlog)
{
    return 0;
}

/* referred from timing.o */
#pragma weak alarm
unsigned int alarm(unsigned int seconds)
{
    return 0;
}

#endif

