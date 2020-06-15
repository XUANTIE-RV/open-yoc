/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock.h
 * @brief    user sock header
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

#ifndef __NET_USRSOCK_USRSOCK_H
#define __NET_USRSOCK_USRSOCK_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <sys/types.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include <time.h>

#include "lwip/sockets.h"
#include "devif.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define USRSOCK_TAG  "usrsock"

#define USRSOCK_ASSERT(msg, assertion) do { if (!(assertion)) { \
                                            LOGE(USRSOCK_TAG, msg);} \
                                          } while (0)

#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/* Internal socket type/domain for marking usrsock sockets */

#define SOCK_USRSOCK_TYPE   0x7f
#define PF_USRSOCK_DOMAIN   0x7f

/* Internal event flags */

#define USRSOCK_EVENT_CONNECT_READY (1 << 0)
#define USRSOCK_EVENT_REQ_COMPLETE  (1 << 15)
#define USRSOCK_EVENT_INTERNAL_MASK (USRSOCK_EVENT_CONNECT_READY | \
                                     USRSOCK_EVENT_REQ_COMPLETE)

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

struct usrsockdev_s;

enum usrsock_conn_state_e
{
  USRSOCK_CONN_STATE_UNINITIALIZED = 0,
  USRSOCK_CONN_STATE_ABORTED,
  USRSOCK_CONN_STATE_READY,
  USRSOCK_CONN_STATE_CONNECTING,
};

struct usrsock_conn_s
{
  dlist_t    node;                   /* Supports a doubly linked list */
  uint8_t    crefs;                  /* Reference counts on this instance */
  int        sock_index;

  enum usrsock_conn_state_e state;   /* State of kernel<->daemon link for conn */
  bool          connected;           /* Socket has been connected */
  int8_t        type;                /* Socket type (SOCK_STREAM, etc) */
  int16_t       usockid;             /* Connection number used for kernel<->daemon */
  uint16_t      flags;               /* Socket state flags */
  struct usrsockdev_s *dev;          /* Device node used for this conn */

  struct
  {
    aos_sem_t sem;         /* Request semaphore (only one outstanding request) */
    uint8_t   xid;         /* Expected message exchange id */
    bool      inprogress;  /* Request was received but daemon is still processing */
    uint16_t  valuelen;    /* Length of value from daemon */
    uint16_t  valuelen_nontrunc; /* Actual length of value at daemon */
    int       result;      /* Result for request */

    struct
    {
      struct iovec *iov;     /* Data request input buffers */
      int    iovcnt;         /* Number of input buffers */
      size_t total;          /* Total length of buffers */
      size_t pos;            /* Writer position on input buffer */
    } datain;
  } resp;

  /* Defines the list of usrsock callbacks */

  struct devif_callback_s *list;
  long       s_rcvtimeo;  /* Receive timeout value (in deciseconds) */
  long       s_sndtimeo;
};

struct usrsock_reqstate_s
{
  struct usrsock_conn_s *conn;       /* Reference to connection structure */
  struct devif_callback_s *cb;       /* Reference to callback instance */
  aos_sem_t               recvsem;   /* Semaphore signals recv completion */
  int                     result;    /* OK on success, otherwise a negated errno. */
  bool                    completed;
  bool                    unlock;    /* True: unlock is required */
};

struct usrsock_data_reqstate_s
{
  struct usrsock_reqstate_s reqstate;
  uint16_t                  valuelen;
  uint16_t                  valuelen_nontrunc;
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifdef __cplusplus
#  define EXTERN extern "C"
extern "C"
{
#else
#  define EXTERN extern
#endif

EXTERN const struct sock_intf_s g_usrsock_sockif;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: usrsock_initialize()
 *
 * Description:
 *   Initialize the User Socket connection structures.  Called once and only
 *   from the networking layer.
 *
 ****************************************************************************/

void usrsock_initialize(void);

/****************************************************************************
 * Name: usrsock_alloc()
 *
 * Description:
 *   Allocate a new, uninitialized usrsock connection structure.  This is
 *   normally something done by the implementation of the socket() API
 *
 ****************************************************************************/

struct usrsock_conn_s *usrsock_alloc(void);

/****************************************************************************
 * Name: usrsock_free()
 *
 * Description:
 *   Free a usrsock connection structure that is no longer in use. This should
 *   be done by the implementation of close().
 *
 ****************************************************************************/

void usrsock_free(struct usrsock_conn_s *conn);

/****************************************************************************
 * Name: usrsock_nextconn()
 *
 * Description:
 *   Traverse the list of allocated usrsock connections
 *
 * Assumptions:
 *   This function is called from usrsock device logic.
 *
 ****************************************************************************/

struct usrsock_conn_s *usrsock_nextconn(struct usrsock_conn_s *conn);

/****************************************************************************
 * Name: usrsock_connidx()
 ****************************************************************************/

int usrsock_connidx(struct usrsock_conn_s *conn);

/****************************************************************************
 * Name: usrsock_active()
 *
 * Description:
 *   Find a connection structure that is the appropriate
 *   connection for usrsock
 *
 * Assumptions:
 *
 ****************************************************************************/

struct usrsock_conn_s *usrsock_active(int16_t usockid);

/****************************************************************************
 * Name: usrsock_setup_request_callback()
 ****************************************************************************/

int usrsock_setup_request_callback(struct usrsock_conn_s *conn,
                                   struct usrsock_reqstate_s *pstate,
                                   devif_callback_event_t event,
                                   uint16_t flags);

/****************************************************************************
 * Name: usrsock_setup_data_request_callback()
 ****************************************************************************/

int usrsock_setup_data_request_callback(struct usrsock_conn_s *conn,
                                        struct usrsock_data_reqstate_s *pstate,
                                        devif_callback_event_t event,
                                        uint16_t flags);

/****************************************************************************
 * Name: usrsock_teardown_request_callback()
 ****************************************************************************/

void usrsock_teardown_request_callback(struct usrsock_reqstate_s *pstate);

/****************************************************************************
 * Name: usrsock_teardown_data_request_callback()
 ****************************************************************************/

#define usrsock_teardown_data_request_callback(datastate) \
  usrsock_teardown_request_callback(&(datastate)->reqstate)

/****************************************************************************
 * Name: usrsock_setup_datain
 ****************************************************************************/

void usrsock_setup_datain(struct usrsock_conn_s *conn,
                          struct iovec *iov, unsigned int iovcnt);

/****************************************************************************
 * Name: usrsock_teardown_datain
 ****************************************************************************/

#define usrsock_teardown_datain(conn) usrsock_setup_datain(conn, NULL, 0)

/****************************************************************************
 * Name: usrsock_event
 *
 * Description:
 *   Handler for received connection events
 *
 ****************************************************************************/

int usrsock_event(struct usrsock_conn_s *conn, uint16_t events);

/****************************************************************************
 * Name: usrsockdev_do_request
 ****************************************************************************/

int usrsockdev_do_request(struct usrsock_conn_s *conn,
                          struct iovec *iov, unsigned int iovcnt);

/****************************************************************************
 * Name: usrsockdev_register
 *
 * Description:
 *   Register /dev/usrsock
 *
 ****************************************************************************/

void usrsockdev_register(void);

/****************************************************************************
 * Name: usrsocket_socket
 *
 * Description:
 *   socket() creates an endpoint for communication and returns a socket
 *   structure.
 *
 * Input Parameters:
 *   domain   (see sys/socket.h)
 *   type     (see sys/socket.h)
 *   protocol (see sys/socket.h)
 *
 * Returned Value:
 *   0 on success; negative error-code on error
 *
 *   EACCES
 *     Permission to create a socket of the specified type and/or protocol
 *     is denied.
 *   EAFNOSUPPORT
 *     The implementation does not support the specified address family.
 *   EINVAL
 *     Unknown protocol, or protocol family not available.
 *   EMFILE
 *     Process file table overflow.
 *   ENFILE
 *     The system limit on the total number of open files has been reached.
 *   ENOBUFS or ENOMEM
 *     Insufficient memory is available. The socket cannot be created until
 *     sufficient resources are freed.
 *   EPROTONOSUPPORT
 *     The protocol type or the specified protocol is not supported within
 *     this domain.
 *
 * Assumptions:
 *
 ****************************************************************************/

int usrsocket_socket(int domain, int type, int protocol);

/****************************************************************************
 * Name: usrsocket_close
 *
 * Description:
 *   Performs the close operation on a usrsock connection instance
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *
 * Returned Value:
 *   0 on success; -1 on error with errno set appropriately.
 *
 * Assumptions:
 *
 ****************************************************************************/

int usrsocket_close(int s);

/****************************************************************************
 * Name: usrsocket_bind
 *
 * Description:
 *   usrsocket_bind() gives the socket 'conn' the local address 'addr'. 'addr'
 *   is 'addrlen' bytes long. Traditionally, this is called "assigning a name
 *   to a socket." When a socket is created with socket, it exists in a name
 *   space (address family) but has no name assigned.
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *   addr     Socket local address
 *   addrlen  Length of 'addr'
 *
 * Returned Value:
 *   0 on success; -1 on error with errno set appropriately
 *
 *   EACCES
 *     The address is protected, and the user is not the superuser.
 *   EADDRINUSE
 *     The given address is already in use.
 *   EINVAL
 *     The socket is already bound to an address.
 *   ENOTSOCK
 *     psock is a descriptor for a file, not a socket.
 *
 * Assumptions:
 *
 ****************************************************************************/

int usrsocket_bind(int s, const struct sockaddr *addr, socklen_t addrlen);

/****************************************************************************
 * Name: usrsocket_connect
 *
 * Description:
 *   Perform a usrsock connection
 *
 * Input Parameters:
 *   s       usrsock sockets index
 *   addr    The address of the remote server to connect to
 *   addrlen Length of address buffer
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

int usrsocket_connect(int s, const struct sockaddr *addr, socklen_t addrlen);

/****************************************************************************
 * Name: usrsocket_listen
 *
 * Description:
 *   To accept connections, a socket is first created with psock_socket(), a
 *   willingness to accept incoming connections and a queue limit for
 *   incoming connections are specified with psock_listen(), and then the
 *   connections are accepted with psock_accept().  For the case of AFINET
 *   and AFINET6 sockets, psock_listen() calls this function.  The
 *   psock_listen() call applies only to sockets of type SOCK_STREAM or
 *   SOCK_SEQPACKET.
 *
 * Parameters:
 *   s        usrsock sockets index
 *   backlog  The maximum length the queue of pending connections may grow.
 *            If a connection request arrives with the queue full, the client
 *            may receive an error with an indication of ECONNREFUSED or,
 *            if the underlying protocol supports retransmission, the request
 *            may be ignored so that retries succeed.
 *
 * Returned Value:
 *   On success, zero is returned. On error, a negated errno value is
 *   returned.  See list() for the set of appropriate error values.
 *
 ****************************************************************************/

int usrsocket_listen(int s, int backlog);

/****************************************************************************
 * Name: usrsocket_accept
 *
 * Description:
 *   The usrsock_sockif_accept function is used with connection-based socket
 *   types (SOCK_STREAM, SOCK_SEQPACKET and SOCK_RDM). It extracts the first
 *   connection request on the queue of pending connections, creates a new
 *   connected socket with mostly the same properties as 'sockfd', and
 *   allocates a new socket descriptor for the socket, which is returned. The
 *   newly created socket is no longer in the listening state. The original
 *   socket 'sockfd' is unaffected by this call.  Per file descriptor flags
 *   are not inherited across an inet_accept.
 *
 *   The 'sockfd' argument is a socket descriptor that has been created with
 *   socket(), bound to a local address with bind(), and is listening for
 *   connections after a call to listen().
 *
 *   On return, the 'addr' structure is filled in with the address of the
 *   connecting entity. The 'addrlen' argument initially contains the size
 *   of the structure pointed to by 'addr'; on return it will contain the
 *   actual length of the address returned.
 *
 *   If no pending connections are present on the queue, and the socket is
 *   not marked as non-blocking, inet_accept blocks the caller until a
 *   connection is present. If the socket is marked non-blocking and no
 *   pending connections are present on the queue, inet_accept returns
 *   EAGAIN.
 *
 * Parameters:
 *   s        usrsock sockets index
 *   addr     Receives the address of the connecting client
 *   addrlen  Input: allocated size of 'addr', Return: returned size of 'addr'
 *   newsock  Location to return the accepted socket information.
 *
 * Returned Value:
 *   Returns 0 (OK) on success.  On failure, it returns a negated errno
 *   value.  See accept() for a desrciption of the approriate error value.
 *
 * Assumptions:
 *   The network is locked.
 *
 ****************************************************************************/

int usrsocket_accept(int s, struct sockaddr *addr, socklen_t *addrlen);

/****************************************************************************
 * Name: usrsocket_sendto
 *
 * Description:
 *   If sendto() is used on a connection-mode (SOCK_STREAM, SOCK_SEQPACKET)
 *   socket, the parameters to and 'tolen' are ignored (and the error EISCONN
 *   may be returned when they are not NULL and 0), and the error ENOTCONN is
 *   returned when the socket was not actually connected.
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *   buf      Data to send
 *   len      Length of data to send
 *   flags    Send flags (ignored)
 *   to       Address of recipient
 *   tolen    The length of the address structure
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

ssize_t usrsocket_sendto(int s, const void *buf, size_t len,
                         int flags, const struct sockaddr *to,
                         socklen_t tolen);

/****************************************************************************
 * Name: usrsocket_recvfrom
 *
 * Description:
 *   recvfrom() receives messages from a socket, and may be used to receive
 *   data on a socket whether or not it is connection-oriented.
 *
 *   If from is not NULL, and the underlying protocol provides the source
 *   address, this source address is filled in. The argument fromlen
 *   initialized to the size of the buffer associated with from, and modified
 *   on return to indicate the actual size of the address stored there.
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *   buf      Buffer to receive data
 *   len      Length of buffer
 *   flags    Receive flags (ignored)
 *   from     Address of source (may be NULL)
 *   fromlen  The length of the address structure
 *
 ****************************************************************************/

ssize_t usrsocket_recvfrom(int s, void *buf, size_t len,
                           int flags, struct sockaddr *from,
                           socklen_t *fromlen);

/****************************************************************************
 * Name: usrsocket_getsockopt
 *
 * Description:
 *   getsockopt() retrieve thse value for the option specified by the
 *   'option' argument for the socket specified by the 'psock' argument. If
 *   the size of the option value is greater than 'value_len', the value
 *   stored in the object pointed to by the 'value' argument will be silently
 *   truncated. Otherwise, the length pointed to by the 'value_len' argument
 *   will be modified to indicate the actual length of the'value'.
 *
 *   The 'level' argument specifies the protocol level of the option. To
 *   retrieve options at the socket level, specify the level argument as
 *   SOL_SOCKET.
 *
 *   See <sys/socket.h> a complete list of values for the 'option' argument.
 *
 * Input Parameters:
 *   s         usrsock sockets index
 *   level     Protocol level to set the option
 *   option    identifies the option to get
 *   value     Points to the argument value
 *   value_len The length of the argument value
 *
 ****************************************************************************/

int usrsocket_getsockopt(int s, int level, int option,
                         void *value, socklen_t* value_len);

/****************************************************************************
 * Name: usrsocket_setsockopt
 *
 * Description:
 *   usrsocket_setsockopt() sets the option specified by the 'option' argument,
 *   at the protocol level specified by the 'level' argument, to the value
 *   pointed to by the 'value' argument for the socket on the 'psock' argument.
 *
 *   The 'level' argument specifies the protocol level of the option. To set
 *   options at the socket level, specify the level argument as SOL_SOCKET.
 *
 *   See <sys/socket.h> a complete list of values for the 'option' argument.
 *
 * Input Parameters:
 *   s         usrsock sockets index
 *   level     Protocol level to set the option
 *   option    identifies the option to set
 *   value     Points to the argument value
 *   value_len The length of the argument value
 *
 ****************************************************************************/

int usrsocket_setsockopt(int s, int level, int option,
                         const void *value, socklen_t value_len);

/****************************************************************************
 * Name: usrsocket_getsockname
 *
 * Description:
 *   The getsockname() function retrieves the locally-bound name of the
 *   specified socket, stores this address in the sockaddr structure pointed
 *   to by the 'addr' argument, and stores the length of this address in the
 *   object pointed to by the 'addrlen' argument.
 *
 *   If the actual length of the address is greater than the length of the
 *   supplied sockaddr structure, the stored address will be truncated.
 *
 *   If the socket has not been bound to a local name, the value stored in
 *   the object pointed to by address is unspecified.
 *
 * Input Parameters:
 *   s         usrsock sockets index
 *   addr     sockaddr structure to receive data [out]
 *   addrlen  Length of sockaddr structure [in/out]
 *
 ****************************************************************************/

int usrsocket_getsockname(int s, struct sockaddr *addr, socklen_t *addrlen);

/****************************************************************************
 * Name: usrsocket_getpeername
 *
 * Description:
 *   The getpeername() function retrieves the remote-connected name of the
 *   specified socket, stores this address in the sockaddr structure pointed
 *   to by the 'addr' argument, and stores the length of this address in the
 *   object pointed to by the 'addrlen' argument.
 *
 *   If the actual length of the address is greater than the length of the
 *   supplied sockaddr structure, the stored address will be truncated.
 *
 *   If the socket has not been bound to a local name, the value stored in
 *   the object pointed to by address is unspecified.
 *
 * Input Parameters:
 *   s        usrsock sockets index
 *   addr     sockaddr structure to receive data [out]
 *   addrlen  Length of sockaddr structure [in/out]
 *
 ****************************************************************************/

int usrsocket_getpeername(int s, struct sockaddr *addr, socklen_t *addrlen);

void net_lock(void);
void net_unlock(void);
int net_lockedwait(aos_sem_t *sem);
int net_timedwait(aos_sem_t *sem, int timeout);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __NET_USRSOCK_USRSOCK_H */
