#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/cdefs.h>
#include <sys/_sigset.h>
#include <sys/_timeval.h>
#include <sys/timespec.h>

/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#  ifndef       FD_SETSIZE
#       define  FD_SETSIZE      64
#  endif

typedef unsigned long   fd_mask;
#  define       NFDBITS (sizeof (fd_mask) * 8)  /* bits per mask */
#  ifndef       _howmany
#       define  _howmany(x,y)   (((x)+((y)-1))/(y))
#  endif

/* We use a macro for fd_set so that including Sockets.h afterwards
   can work.  */
typedef struct _types_fd_set {
        fd_mask fds_bits[_howmany(FD_SETSIZE, NFDBITS)];
} _types_fd_set;

#define fd_set _types_fd_set


#if 0
/* FD_SET used for lwip_select */
#ifndef FD_SET
#undef  FD_SETSIZE
/* Make FD_SETSIZE match NUM_SOCKETS in socket.c */
#define MEMP_NUM_NETCONN 128
#define FD_SETSIZE    MEMP_NUM_NETCONN
#define FDSETSAFESET(n, code) do { \
  if (((n) - LWIP_SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n) - LWIP_SOCKET_OFFSET) >= 0)) { \
  code; }} while(0)
#define FDSETSAFEGET(n, code) (((n) - LWIP_SOCKET_OFFSET < MEMP_NUM_NETCONN) && (((int)(n) - LWIP_SOCKET_OFFSET) >= 0) ?\
  (code) : 0)
#define FD_SET(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] |=  (1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_CLR(n, p)  FDSETSAFESET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] &= ~(1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_ISSET(n,p) FDSETSAFEGET(n, (p)->fd_bits[((n)-LWIP_SOCKET_OFFSET)/8] &   (1 << (((n)-LWIP_SOCKET_OFFSET) & 7)))
#define FD_ZERO(p)    memset((void*)(p), 0, sizeof(*(p)))

typedef struct fd_set
{
  unsigned char fd_bits [(FD_SETSIZE+7)/8];
} fd_set;

#elif LWIP_SOCKET_OFFSET
#error LWIP_SOCKET_OFFSET does not work with external FD_SET!
#elif FD_SETSIZE < MEMP_NUM_NETCONN
#error "external FD_SETSIZE too small for number of sockets"
#endif /* FD_SET */
#endif

extern int select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout, void *semaphore);
          
extern int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout);


#ifdef __cplusplus
}
#endif

#endif
