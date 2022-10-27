/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef AT_SOCKET_H
#define AT_SOCKET_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <aos/list.h>

#include <lwip/netdb.h>
#include <sys/socket.h>

/* support max 3 connection */
#define MIN_CIP_ID (0)

#define MAX_CIP_ID (2)

typedef enum {
    PT_MODE  = 0, /* passthrough mode: data will be recv by event */
    CMD_MODE = 1  /* cmd mode: data will be recv by AT+CIPRECV */
} recv_mode_t;

/* status */
typedef enum {
    STATS_STOP       = 0, /* not connect */
    STATS_CONNECTING = 1, /* for tcp server, if not tcp client connected, will keep in connecting */
    STATS_CONNECT    = 2
} at_conn_status_t;

/* connection type */
typedef enum {
    TYPE_NULL        = 0, /*init val */
    TYPE_TCP_SERVER  = 1,
    TYPE_TCP_CLIENT  = 2,
    TYPE_UDP_UNICAST = 3
} at_conn_type_t;

typedef struct {
    int id;
    int sockfd;
    /* keep tcp server sockfd */
    int ssockfd;
    /* target ip */
    ip4_addr_t       ip_addr;
    int              rport;
    int              lport;
    at_conn_type_t   type;
    at_conn_status_t status;
    slist_t          next;
    char            *recv_buf;
    int              recv_size;
    int              c_cnt;
} at_conn_t;

#define AT_OK 0                 /*!< No error */
#define AT_ERR (-1)             /*!< General fail code */
#define AT_ERR_NO_KEY (-2)      /*!< Key not set */
#define AT_ERR_INVAL (-3)       /*!< Invalid argument */
#define AT_ERR_STATUS (-4)      /*!< Invalid status */
#define AT_ERR_RECV (-5)        /*!< socket API: Fail to recv data */
#define AT_ERR_SEND (-6)        /*!< socket API: Fail to send data */
#define AT_ERR_START (-7)       /*!< socket API: Fail to start */
#define AT_ERR_CHECK (-8)       /*!< data check error*/
#define AT_ERR_FOTA_NOVER (-9)  /* !< no version */
#define AT_ERR_FOTA_MDVER (-10) /* !< version for module */
#define AT_ERR_PROC (-11)       /* !< process failed*/

const char *at_conn_type_str(at_conn_type_t type);

at_conn_t *at_parse_start_arg(char *data);
void       at_dump_conn_arg(at_conn_t *conn);

at_conn_t *conn_find_by_id(int id);

void add_conn(at_conn_t *at_conn);
void remove_conn(at_conn_t *at_conn);

int   at_connect_start(at_conn_t *at_conn, char *cmd);
void *at_socket_recv(at_conn_t *conn, int *size);
int   at_disconnect(at_conn_t *conn, int keep_server);

int at_disconnect_all();
int at_count_conn();

void subscribe(at_conn_t *conn);
void unsubscribe(at_conn_t *conn);

#endif
