/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <aos/kernel.h>
#include <errno.h>

/*
4.3BSD, POSIX.1-2001.  POSIX.1-2001 declares this function
obsolete; use nanosleep(2) instead.  POSIX.1-2008 removes the
specification of usleep().

On the original BSD implementation, and in glibc before version
2.2.2, the return type of this function is void.  The POSIX
version returns int, and this is also the prototype used since
glibc 2.2.2.

Only the EINVAL error return is documented by SUSv2 and
POSIX.1-2001.
*/
__attribute__((weak)) int usleep(useconds_t usec)
{
    if (usec && (usec <= 1000000)) {
        if (usec < 1000) {
            usec = 1000;
        }

        aos_msleep(usec / 1000);
        return 0;
    }

    return -EINVAL;
}

