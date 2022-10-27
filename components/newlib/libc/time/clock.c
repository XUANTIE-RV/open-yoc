/*
 * Copyright (C) 2019 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <time.h>

extern int clock_gettime(clockid_t clockid, struct timespec *tp);

/*
POSIX.1-2001, POSIX.1-2008, C89, C99.  XSI requires that
CLOCKS_PER_SEC equals 1000000 independent of the actual
resolution.
*/
clock_t clock(void)
{
    struct timespec tv;

    /* Get the current time from the system */

    if (clock_gettime(CLOCK_MONOTONIC, &tv) == 0) {
        return (clock_t)tv.tv_sec;
    }

    return (clock_t) -1;
}
