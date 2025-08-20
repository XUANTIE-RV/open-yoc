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

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <stdint.h>

typedef uint32_t    clockid_t;
typedef uint32_t    key_t;         /* Used for interprocess communication. */
typedef uint32_t    pid_t;         /* Used for process IDs and process group IDs. */
typedef signed long ssize_t;       /* Used for a count of bytes or an error indication. */
typedef long long   off_t; 
typedef long suseconds_t;

#ifndef PATH_MAX
#define PATH_MAX    1024
#endif

#if __BSD_VISIBLE
#include <sys/select.h>
#endif

#endif /* _SYS_TYPES_H */
