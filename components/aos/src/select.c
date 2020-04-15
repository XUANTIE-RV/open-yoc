/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc_config.h>

#include "lwip/opt.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/priv/api_msg.h"


#if 0
#include "lwip/api.h"
#include "lwip/igmp.h"
#include "arpa/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/priv/tcpip_priv.h"
#endif

#define NUM_SOCKETS MEMP_NUM_NETCONN
#define NUM_EVENTS  MEMP_NUM_NETCONN

/** This is overridable for the rare case where more than 255 threads
 * select on the same socket...
 */
#ifndef SELWAIT_T
#define SELWAIT_T u8_t
#endif

/** Contains all internal pointers and states used for a socket */
struct lwip_sock {
  /** sockets currently are built on netconns, each socket has one netconn */
  struct netconn *conn;
  /** data that was left from the previous read */
  void *lastdata;
  /** offset in the data that was left from the previous read */
  u16_t lastoffset;
  /** number of times data was received, set by event_callback(),
      tested by the receive and select functions */
  s16_t rcvevent;
  /** number of times data was ACKed (free send buffer), set by event_callback(),
      tested by select */
  u16_t sendevent;
  /** error happened for this socket, set by event_callback(), tested by select */
  u16_t errevent;
  /** last error that occurred on this socket (in fact, all our errnos fit into an u8_t) */
  u8_t err;
  /** counter of how many threads are waiting for this socket using select */
  SELWAIT_T select_waiting;
};

#if LWIP_NETCONN_SEM_PER_THREAD
#define SELECT_SEM_T        sys_sem_t*
#define SELECT_SEM_PTR(sem) (sem)
#else /* LWIP_NETCONN_SEM_PER_THREAD */
#define SELECT_SEM_T        sys_sem_t
#define SELECT_SEM_PTR(sem) (&(sem))
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

/** Description for a task waiting in select */
struct lwip_select_cb {
  /** Pointer to the next waiting task */
  struct lwip_select_cb *next;
  /** Pointer to the previous waiting task */
  struct lwip_select_cb *prev;
  /** readset passed to select */
  fd_set *readset;
  /** writeset passed to select */
  fd_set *writeset;
  /** unimplemented: exceptset passed to select */
  fd_set *exceptset;
  /** don't signal the same semaphore twice: set to 1 when signalled */
  int sem_signalled;
  /** semaphore to wake up a task waiting for select */
  sys_sem_t * psem;
};

#define LWIP_EVENT_OFFSET (NUM_SOCKETS + LWIP_SOCKET_OFFSET)
struct lwip_event {
  uint64_t counts;
  int used;
  int reads;
  int writes;
  /** semaphore to wake up a task waiting for select */
  sys_sem_t * psem;
};

/** The global array of available sockets */
struct lwip_sock sockets[NUM_SOCKETS];
/** The global array of available events */
struct lwip_event events[NUM_EVENTS];
/** The global list of tasks waiting for select */
struct lwip_select_cb *select_cb_list;
/** This counter is increased from lwip_select when the list is changed
    and checked in event_callback to see if it has changed. */
volatile int select_cb_ctr;

#if LWIP_SOCKET_SET_ERRNO
#ifndef set_errno
#define set_errno(err) do { if (err) { errno = (err); } } while(0)
#endif
#else /* LWIP_SOCKET_SET_ERRNO */
#define set_errno(err)
#endif /* LWIP_SOCKET_SET_ERRNO */

#define sock_set_errno(sk, e) do { \
  const int sockerr = (e); \
  sk->err = (u8_t)sockerr; \
  set_errno(sockerr); \
} while (0)

static struct lwip_event *
tryget_event(int s)
{
  s -= LWIP_EVENT_OFFSET;
  if ((s < 0) || (s >= NUM_EVENTS)) {
    return NULL;
  }
  if (!events[s].used) {
    return NULL;
  }
  return &events[s];
}

/**
 * Same as get_socket but doesn't set errno
 *
 * @param s externally used socket index
 * @return struct lwip_sock for the socket or NULL if not found
 */
static struct lwip_sock *
tryget_socket(int s)
{
  s -= LWIP_SOCKET_OFFSET;
  if ((s < 0) || (s >= NUM_SOCKETS)) {
    return NULL;
  }
  if (!sockets[s].conn) {
    return NULL;
  }
  return &sockets[s];
}

/**
 * Go through the readset and writeset lists and see which socket of the sockets
 * set in the sets has events. On return, readset, writeset and exceptset have
 * the sockets enabled that had events.
 *
 * @param maxfdp1 the highest socket index in the sets
 * @param readset_in    set of sockets to check for read events
 * @param writeset_in   set of sockets to check for write events
 * @param exceptset_in  set of sockets to check for error events
 * @param readset_out   set of sockets that had read events
 * @param writeset_out  set of sockets that had write events
 * @param exceptset_out set os sockets that had error events
 * @return number of sockets that had events (read/write/exception) (>= 0)
 */
static int
lwip_selscan(int maxfdp1, fd_set *readset_in, fd_set *writeset_in, fd_set *exceptset_in,
             fd_set *readset_out, fd_set *writeset_out, fd_set *exceptset_out)
{
  int i, nready = 0;
  fd_set lreadset, lwriteset, lexceptset;
  struct lwip_sock *sock;
  struct lwip_event *event;
  SYS_ARCH_DECL_PROTECT(lev);

  FD_ZERO(&lreadset);
  FD_ZERO(&lwriteset);
  FD_ZERO(&lexceptset);

  /* Go through each socket in each list to count number of sockets which
     currently match */
  for (i = LWIP_SOCKET_OFFSET; i < maxfdp1; i++) {
    /* if this FD is not in the set, continue */
    if (!(readset_in && FD_ISSET(i, readset_in)) &&
        !(writeset_in && FD_ISSET(i, writeset_in)) &&
        !(exceptset_in && FD_ISSET(i, exceptset_in))) {
      continue;
    }
    /* First get the socket's status (protected)... */
    SYS_ARCH_PROTECT(lev);
    sock = tryget_socket(i);
    event = tryget_event(i);
    if (sock != NULL || event != NULL) {
      void* lastdata = sock ? sock->lastdata : NULL;
      s16_t rcvevent = sock ? sock->rcvevent : event->reads;
      u16_t sendevent = sock ? sock->sendevent : event->writes;
      u16_t errevent = sock ? sock->errevent : 0;
      SYS_ARCH_UNPROTECT(lev);

      /* ... then examine it: */
      /* See if netconn of this socket is ready for read */
      if (readset_in && FD_ISSET(i, readset_in) && ((lastdata != NULL) || (rcvevent > 0))) {
        FD_SET(i, &lreadset);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for reading\n", i));
        nready++;
      }
      /* See if netconn of this socket is ready for write */
      if (writeset_in && FD_ISSET(i, writeset_in) && (sendevent != 0)) {
        FD_SET(i, &lwriteset);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for writing\n", i));
        nready++;
      }
      /* See if netconn of this socket had an error */
      if (exceptset_in && FD_ISSET(i, exceptset_in) && (errevent != 0)) {
        FD_SET(i, &lexceptset);
        LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_selscan: fd=%d ready for exception\n", i));
        nready++;
      }
    } else {
      SYS_ARCH_UNPROTECT(lev);
      /* continue on to next FD in list */
    }
  }
  /* copy local sets to the ones provided as arguments */
  *readset_out = lreadset;
  *writeset_out = lwriteset;
  *exceptset_out = lexceptset;

  LWIP_ASSERT("nready >= 0", nready >= 0);
  return nready;
}

int
select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout, void *semaphore)
{
  u32_t waitres = 0;
  sys_sem_t *psem = (sys_sem_t *)semaphore;
  int nready;
  fd_set lreadset, lwriteset, lexceptset;
  u32_t msectimeout;
  struct lwip_select_cb select_cb;
  int i;
  int maxfdp2;
#if LWIP_NETCONN_SEM_PER_THREAD
  int waited = 0;
#else
  sys_sem_t sem;
#endif
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select(%d, %p, %p, %p, tvsec=%"S32_F" tvusec=%"S32_F")\n",
                  maxfdp1, (void *)readset, (void *) writeset, (void *) exceptset,
                  timeout ? (s32_t)timeout->tv_sec : (s32_t)-1,
                  timeout ? (s32_t)timeout->tv_usec : (s32_t)-1));

  /* Go through each socket in each list to count number of sockets which
     currently match */
  nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);

  /* If we don't have any current events, then suspend if we are supposed to */
  if (!nready) {
    if (timeout && timeout->tv_sec == 0 && timeout->tv_usec == 0) {
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: no timeout, returning 0\n"));
      /* This is OK as the local fdsets are empty and nready is zero,
         or we would have returned earlier. */
      goto return_copy_fdsets;
    }

    /* None ready: add our semaphore to list:
       We don't actually need any dynamic memory. Our entry on the
       list is only valid while we are in this function, so it's ok
       to use local variables. */

    select_cb.next = NULL;
    select_cb.prev = NULL;
    select_cb.readset = readset;
    select_cb.writeset = writeset;
    select_cb.exceptset = exceptset;
    select_cb.sem_signalled = 0;
#if LWIP_NETCONN_SEM_PER_THREAD
    if(psem)
        select_cb.psem = psem;
    else {
      select_cb.psem = LWIP_NETCONN_THREAD_SEM_GET();
    }
#else /* LWIP_NETCONN_SEM_PER_THREAD */
    if(psem)
        select_cb.psem = psem;
    else {
      if (sys_sem_new(&sem, 0) != ERR_OK) {
        /* failed to create semaphore */
        set_errno(ENOMEM);
        return -1;
      }
      select_cb.psem = &sem;
    }
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

    /* Protect the select_cb_list */
    SYS_ARCH_PROTECT(lev);

    /* Put this select_cb on top of list */
    select_cb.next = select_cb_list;
    if (select_cb_list != NULL) {
      select_cb_list->prev = &select_cb;
    }
    select_cb_list = &select_cb;
    /* Increasing this counter tells event_callback that the list has changed. */
    select_cb_ctr++;

    /* Now we can safely unprotect */
    SYS_ARCH_UNPROTECT(lev);

    /* Increase select_waiting for each socket we are interested in */
    maxfdp2 = maxfdp1;
    for (i = LWIP_SOCKET_OFFSET; i < maxfdp1; i++) {
      if ((readset && FD_ISSET(i, readset)) ||
          (writeset && FD_ISSET(i, writeset)) ||
          (exceptset && FD_ISSET(i, exceptset))) {
        struct lwip_sock *sock;
        struct lwip_event *event;
        SYS_ARCH_PROTECT(lev);
        sock = tryget_socket(i);
        event = tryget_event(i);
        if (sock != NULL) {
          sock->select_waiting++;
          LWIP_ASSERT("sock->select_waiting > 0", sock->select_waiting > 0);
        } else if (event != NULL) {
          event->psem = select_cb.psem;
        } else {
          /* Not a valid socket */
          nready = -1;
          maxfdp2 = i;
          SYS_ARCH_UNPROTECT(lev);
          break;
        }
        SYS_ARCH_UNPROTECT(lev);
      }
    }

    if (nready >= 0) {
      /* Call lwip_selscan again: there could have been events between
         the last scan (without us on the list) and putting us on the list! */
      nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
      if (!nready) {
        /* Still none ready, just wait to be woken */
        if (timeout == 0) {
          /* Wait forever */
          msectimeout = 0;
        } else {
          msectimeout =  ((timeout->tv_sec * 1000) + ((timeout->tv_usec + 500)/1000));
          if (msectimeout == 0) {
            /* Wait 1ms at least (0 means wait forever) */
            msectimeout = 1;
          }
        }

        waitres = sys_arch_sem_wait(select_cb.psem, msectimeout);
#if LWIP_NETCONN_SEM_PER_THREAD
        waited = 1;
#endif
      }
    }

    /* Decrease select_waiting for each socket we are interested in */
    for (i = LWIP_SOCKET_OFFSET; i < maxfdp2; i++) {
      if ((readset && FD_ISSET(i, readset)) ||
          (writeset && FD_ISSET(i, writeset)) ||
          (exceptset && FD_ISSET(i, exceptset))) {
        struct lwip_sock *sock;
        SYS_ARCH_PROTECT(lev);
        sock = tryget_socket(i);
        if (sock != NULL) {
          /* for now, handle select_waiting==0... */
          LWIP_ASSERT("sock->select_waiting > 0", sock->select_waiting > 0);
          if (sock->select_waiting > 0) {
            sock->select_waiting--;
          }
        } else {
          /* Not a valid socket */
          nready = -1;
        }
        SYS_ARCH_UNPROTECT(lev);
      }
    }
    /* Take us off the list */
    SYS_ARCH_PROTECT(lev);
    if (select_cb.next != NULL) {
      select_cb.next->prev = select_cb.prev;
    }
    if (select_cb_list == &select_cb) {
      LWIP_ASSERT("select_cb.prev == NULL", select_cb.prev == NULL);
      select_cb_list = select_cb.next;
    } else {
      LWIP_ASSERT("select_cb.prev != NULL", select_cb.prev != NULL);
      select_cb.prev->next = select_cb.next;
    }
    /* Increasing this counter tells event_callback that the list has changed. */
    select_cb_ctr++;
    SYS_ARCH_UNPROTECT(lev);

#if LWIP_NETCONN_SEM_PER_THREAD
    if (select_cb.sem_signalled && (!waited || (waitres == SYS_ARCH_TIMEOUT))) {
      /* don't leave the thread-local semaphore signalled */
      sys_arch_sem_wait(select_cb.psem, 1);
    }
#else /* LWIP_NETCONN_SEM_PER_THREAD */
    if(!psem)
      sys_sem_free(select_cb.psem);
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

    if (nready < 0) {
      /* This happens when a socket got closed while waiting */
      set_errno(EBADF);
      return -1;
    }

    if (waitres == SYS_ARCH_TIMEOUT) {
      /* Timeout */
      LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: timeout expired\n"));
      /* This is OK as the local fdsets are empty and nready is zero,
         or we would have returned earlier. */
      goto return_copy_fdsets;
    }

    /* See what's set */
    nready = lwip_selscan(maxfdp1, readset, writeset, exceptset, &lreadset, &lwriteset, &lexceptset);
  }

  LWIP_DEBUGF(SOCKETS_DEBUG, ("lwip_select: nready=%d\n", nready));
return_copy_fdsets:
  set_errno(0);
  if (readset) {
    *readset = lreadset;
  }
  if (writeset) {
    *writeset = lwriteset;
  }
  if (exceptset) {
    *exceptset = lexceptset;
  }
  return nready;
}

int
select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout)
{
  return select2(maxfdp1, readset, writeset, exceptset, timeout, NULL);
}

