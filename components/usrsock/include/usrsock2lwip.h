/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock2lwip.h
 * @brief    user sock lwip mapping header
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

#ifndef __USRSOCK2LWIP_H__
#define __USRSOCK2LWIP_H__


/*********************************************/
#define USRSOCK_SOCKET_OFFSET LWIP_SOCKET_OFFSET

/*********************************************/
#define usrsocket_accept           accept
#define usrsocket_bind             bind
#define usrsocket_shutdown         shutdown
#define usrsocket_getpeername      getpeername
#define usrsocket_getsockname      getsockname
#define usrsocket_setsockopt       setsockopt
#define usrsocket_getsockopt       getsockopt
#define usrsocket_close            closesocket
#define usrsocket_connect          connect
#define usrsocket_listen           listen
#define usrsocket_recv             recv
#define usrsocket_recvfrom         recvfrom
#define usrsocket_send             send
#define usrsocket_sendmsg          sendmsg
#define usrsocket_sendto           sendto
#define usrsocket_socket           socket
#define usrsocket_ioctlsocket      ioctl
#define usrsocket_getaddrinfo      getaddrinfo
#define usrsocket_freeaddrinfo     freeaddrinfo
#define usrsocket_gethostbyname    gethostbyname
#define usrsocket_select           select

#endif
