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

#ifndef _SYS_SELECT_H__
#define _SYS_SELECT_H__

#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

#define	NBBY	8		/* number of bits in a byte */

typedef	long	fd_mask;
#define	NFDBITS	(sizeof (fd_mask) * NBBY)	/* bits per mask */
#ifndef	howmany
#define	howmany(x,y)	(((x)+((y)-1))/(y))
#endif

/* We use a macro for fd_set so that including Sockets.h afterwards
   can work.  */
typedef	struct _types_fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} _types_fd_set;

#define fd_set _types_fd_set

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
#define FD_ZERO(p)      memset((void*)(p), 0, sizeof(*(p)))

#endif
