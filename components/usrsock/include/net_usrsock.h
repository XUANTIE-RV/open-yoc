/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     net_usrsock.h
 * @brief    net user sock header
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

#ifndef __INCLUDE_NUTTX_NET_USRSOCK_H
#define __INCLUDE_NUTTX_NET_USRSOCK_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Event message flags */

#define USRSOCK_EVENT_ABORT          (1 << 1)
#define USRSOCK_EVENT_SENDTO_READY   (1 << 2)
#define USRSOCK_EVENT_RECVFROM_AVAIL (1 << 3)
#define USRSOCK_EVENT_REMOTE_CLOSED  (1 << 4)

/* Response message flags */

#define USRSOCK_MESSAGE_FLAG_REQ_IN_PROGRESS (1 << 0)
#define USRSOCK_MESSAGE_FLAG_EVENT           (1 << 1)

#define USRSOCK_MESSAGE_IS_EVENT(flags) \
                          (!!((flags) & USRSOCK_MESSAGE_FLAG_EVENT))
#define USRSOCK_MESSAGE_IS_REQ_RESPONSE(flags) \
                          (!USRSOCK_MESSAGE_IS_EVENT(flags))

#define USRSOCK_MESSAGE_REQ_IN_PROGRESS(flags) \
                          (!!((flags) & USRSOCK_MESSAGE_FLAG_REQ_IN_PROGRESS))
#define USRSOCK_MESSAGE_REQ_COMPLETED(flags) \
                          (!USRSOCK_MESSAGE_REQ_IN_PROGRESS(flags))

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Request types */

enum usrsock_request_types_e
{
  USRSOCK_REQUEST_SOCKET = 0,
  USRSOCK_REQUEST_CLOSE,
  USRSOCK_REQUEST_CONNECT,
  USRSOCK_REQUEST_SENDTO,
  USRSOCK_REQUEST_RECVFROM,
  USRSOCK_REQUEST_SETSOCKOPT,
  USRSOCK_REQUEST_GETSOCKOPT,
  USRSOCK_REQUEST_GETSOCKNAME,
  USRSOCK_REQUEST_GETPEERNAME,
  USRSOCK_REQUEST_BIND,
  USRSOCK_REQUEST_LISTEN,
  USRSOCK_REQUEST_ACCEPT,
  USRSOCK_REQUEST_IOCTL,
  USRSOCK_REQUEST__MAX
};

/* Response/event message types */

enum usrsock_message_types_e
{
  USRSOCK_MESSAGE_RESPONSE_ACK = 0,
  USRSOCK_MESSAGE_RESPONSE_DATA_ACK,
  USRSOCK_MESSAGE_SOCKET_EVENT,
};

/* Request structures (kernel => /dev/usrsock => daemon) */

struct usrsock_request_common_s
{
  int8_t reqid;
  uint8_t xid;
} __attribute__((__packed__));

struct usrsock_request_socket_s
{
  struct usrsock_request_common_s head;

  int16_t domain;
  int16_t type;
  int16_t protocol;
} __attribute__((__packed__));

struct usrsock_request_close_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
} __attribute__((__packed__));

struct usrsock_request_bind_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t addrlen;
} __attribute__((__packed__));

struct usrsock_request_connect_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t addrlen;
} __attribute__((__packed__));

struct usrsock_request_listen_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t backlog;
} __attribute__((__packed__));

struct usrsock_request_accept_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t max_addrlen;
} __attribute__((__packed__));

struct usrsock_request_sendto_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t addrlen;
  uint16_t buflen;
} __attribute__((__packed__));

struct usrsock_request_recvfrom_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t max_buflen;
  uint16_t max_addrlen;
} __attribute__((__packed__));

struct usrsock_request_setsockopt_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  int16_t level;
  int16_t option;
  uint16_t valuelen;
} __attribute__((__packed__));

struct usrsock_request_getsockopt_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  int16_t level;
  int16_t option;
  uint16_t max_valuelen;
} __attribute__((__packed__));

struct usrsock_request_getsockname_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t max_addrlen;
} __attribute__((__packed__));

struct usrsock_request_getpeername_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  uint16_t max_addrlen;
} __attribute__((__packed__));

struct usrsock_request_ioctl_s
{
  struct usrsock_request_common_s head;

  int16_t usockid;
  int32_t cmd;
  uint16_t arglen;
} __attribute__((__packed__));

/* Response/event message structures (kernel <= /dev/usrsock <= daemon) */

struct usrsock_message_common_s
{
  int8_t msgid;
  int8_t flags;
} __attribute__((__packed__));

/* Request acknowledgment/completion message */

struct usrsock_message_req_ack_s
{
  struct usrsock_message_common_s head;

  uint8_t xid;
  int32_t result;
} __attribute__((__packed__));

/* Request acknowledgment/completion message */

struct usrsock_message_datareq_ack_s
{
  struct usrsock_message_req_ack_s reqack;

  /* head.result => positive buflen, negative error-code. */

  uint16_t valuelen;          /* length of value returned after buffer */
  uint16_t valuelen_nontrunc; /* actual non-truncated length of value at
                               * daemon-sïde. */
} __attribute__((__packed__));

/* Socket event message */

struct usrsock_message_socket_event_s
{
  struct usrsock_message_common_s head;

  int16_t usockid;
  uint16_t events;
} __attribute__((__packed__));

#endif /* __INCLUDE_NUTTX_NET_USRSOCK_H */
