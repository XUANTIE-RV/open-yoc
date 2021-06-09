/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_btsnoop.c
 * @brief:   vendor btsnoop implement
 * @author:  zhigao.dzg@alibaba-inc.com
 * @date:    2020/03/12
 * @version: 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <aos/kernel.h>
#include <aos/cli.h>

// #include "tg_btsnoop.h"
// #include "tg_bluetooth.h"
// #include "tg_bt_mesh.h"
#include "bt_err.h"
// #include "bt_hal_os.h"
#include "hci/btsnoop.h"
#include "common/bt_target.h"
#include "common/bt_trace.h"

#if CONFIG_BT_SNOOP_ENABLED
#define BTSNOOP_TASK_STACK_SIZE          (2048)
#define BTSNOOP_TASK_PRIO                (BT_TASK_DEFAULT_PRIORITIES - 1)
#define BTSNOOP_TASK_NAME                "BTSNOOP"
#define BTSNOOP_QUEUE_LEN                3200

typedef struct _bt_snooop_node {
    void *data;
    int16_t length;
}btsnoop_node;

static int8_t btsnoop_task_enabled = 0;
static int8_t btsnoop_enabled = 0;
static aos_task_t btsnoop_thread;
static aos_queue_t btsnoop_queue;
static int8_t btsnoop_queue_buf[BTSNOOP_QUEUE_LEN * sizeof(btsnoop_node)];

typedef struct header {
    int magic_number;
    int length;
}header_t;

typedef struct _server_t {
    int fd;
}server_t;

typedef struct _client_t {
    int fd;
    uint8_t *pkt_data;
    uint16_t pkt_expect;
    uint16_t pkt_len;
    uint16_t pkt_offset;
}client_t;

static server_t g_btsnoop_server;
static client_t g_btsnoop_client;

extern void btsnoop_net_set_callback(btsnoop_net_callback callback);

static int accept_client(int fd)
{
    struct sockaddr addr;
    socklen_t len;
    int nfd;

    memset(&addr, 0, sizeof(addr));
    len = sizeof(addr);
    if (getsockname(fd, (struct sockaddr *) &addr, &len) < 0) {
        HCI_TRACE_ERROR("Failed to get socket name");
        return -1;
    }

    nfd = accept(fd, (struct sockaddr *) &addr, &len);
    if (nfd < 0) {
        HCI_TRACE_ERROR("Failed to accept client socket");
        return -1;
    }
    //printf("client accepted:%d\n", nfd);

    return nfd;
}

static int server_accept(server_t *server)
{
    client_t *client = &g_btsnoop_client;

    if (!client)
        return -1;
    memset(client, 0, sizeof(*client));

    client->fd = accept_client(server->fd);
    if (client->fd < 0) {
        HCI_TRACE_ERROR("client fd is incorrect\n");
        return -1;
    }

    return 0;
}

static int open_tcp(void)
{
    struct sockaddr_in addr;
    int fd = 0;

    HCI_TRACE_WARNING("%s start", __func__);
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        HCI_TRACE_ERROR("Failed to open server socket");
        return -1;
    }

    //setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(40001);
    HCI_TRACE_WARNING("%s start bind", __func__);
    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        HCI_TRACE_ERROR("Failed to bind server socket");
        close(fd);
        return -1;
    }

    HCI_TRACE_WARNING("%s start listen", __func__);
    if (listen(fd, 100) < 0) {
        HCI_TRACE_ERROR("Failed to listen server socket");
        close(fd);
        return -1;
    }
    HCI_TRACE_WARNING("%s end", __func__);

    return fd;
}

static int server_open_tcp(void)
{
    server_t *server;

    HCI_TRACE_WARNING("%s", __func__);
    server = &g_btsnoop_server;
    memset(server, 0, sizeof(*server));

    server->fd = open_tcp();
    if (server->fd < 0) {
        return -1;
    }

    return 0;
}

static int server_close_tcp(void)
{
    server_t *server;

    server = &g_btsnoop_server;
    close(server->fd);

    return 0;
}

static int create_tcp_server(void)
{
    HCI_TRACE_WARNING("%s", __func__);
    if (server_open_tcp() < 0) {
        HCI_TRACE_ERROR("failed to create server socket");
        return -1;
    }

    return 0;
}

static int destroy_tcp_server(void)
{
    return server_close_tcp();
}

void btsnoop_callback(const void* data, size_t length)
{
    btsnoop_node snoop_node;
    int32_t ret;

    if (btsnoop_enabled == 0)
        return;

    if (btsnoop_task_enabled == 0)
        return;

    snoop_node.data = malloc(length);
    if (snoop_node.data == NULL) {
        HCI_TRACE_ERROR("%s malloc failed, length = %d", __func__, length);
        return;
    }
    memcpy(snoop_node.data, data, length);
    snoop_node.length = length;

//    HCI_TRACE_WARNING("%s send start", __func__);
    ret = aos_queue_send(&btsnoop_queue, &snoop_node, sizeof(btsnoop_node));
    if (ret != 0) {
        HCI_TRACE_ERROR("%s send failed, length = %d, ret = %d", __func__, length, ret);
        free(snoop_node.data);
    }

    return;
}

void btsnoop_thread_handle(void *pArgs)
{
    btsnoop_node snoop_node;
    int32_t len;
    int ret;
    server_t *srv = &g_btsnoop_server;
    client_t *cli = &g_btsnoop_client;

    HCI_TRACE_WARNING("%s", __func__);
    do {
        ret = server_accept(srv);
        if (ret < 0) {
            HCI_TRACE_ERROR("fail to accept client connection");
            close(srv->fd);
            return;
        }
    }while(0);

    HCI_TRACE_WARNING("%s server accepted", __func__);
    btsnoop_task_enabled = 1;
    while (1) {
        ret = aos_queue_recv(&btsnoop_queue, AOS_WAIT_FOREVER, &snoop_node, (unsigned int *)&len);
        if (ret) {
//            HCI_TRACE_ERROR("%s recv err: %d", __func__, ret);
            continue;
        }
        
//        HCI_TRACE_WARNING("%s recvd: %d", __func__, snoop_node.length);
        send(cli->fd, snoop_node.data, snoop_node.length, 0);

        free(snoop_node.data);
    }
    HCI_TRACE_ERROR("%s quit", __func__);
}

static void btsnoop_cmd_handle(bool flag)
{
    int32_t ret;

    HCI_TRACE_WARNING("%s: %d", __func__, flag);
    if (flag) {
        if (btsnoop_task_enabled == 0) {
            ret = create_tcp_server();
            if (ret != 0) {
                return;
            }
            ret = aos_queue_new(&btsnoop_queue, btsnoop_queue_buf, BTSNOOP_QUEUE_LEN * sizeof(btsnoop_node), sizeof(btsnoop_node));
            if (ret != 0) {
                return;
            }
            aos_task_new_ext(&btsnoop_thread, BTSNOOP_TASK_NAME, btsnoop_thread_handle, NULL, BTSNOOP_TASK_STACK_SIZE, BTSNOOP_TASK_PRIO);
        }

        btsnoop_enabled = 1;
    } else {
        ret = destroy_tcp_server();
        if (ret != 0) {
            return;
        }
        btsnoop_enabled = 0;
    }
}

static void btsnoop_start_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int32_t flag = 0;

    HCI_TRACE_WARNING("%s", __func__);
    if (argc < 2)
        return;

    flag = atoi((const char *)argv[1]);
    btsnoop_cmd_handle((bool)flag);
}

static struct cli_command ncmd = {
    .name = "bt_snoop_start",
    .help = "bt_snoop_start",
    .function = btsnoop_start_cmd,
};

static void btsnoop_register_cli(void)
{
    aos_cli_register_command(&ncmd);
}

void btsnoop_init(void)
{
    btsnoop_net_set_callback(btsnoop_callback);
    btsnoop_register_cli();
}

#endif

