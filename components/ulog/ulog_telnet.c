/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#include "ulog_telnet.h"
#include "ulog.h"
#include <aos/aos.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <lwip/netdb.h>
#include <arpa/inet.h>

#define TAG "ulog_telnet"

#define TELNET_WILL 251
#define TELNET_WONT 252
#define TELNET_DO 253
#define TELNET_DONT 254
#define TELNET_IAC 255

typedef struct _ulog_telnet {
    int fd;
    int telnet_run;
} ulog_telnet_t;

static ulog_telnet_t s_ulog_telnet;
static struct ulog_backend telnet;
static ulog_telnet_event u_cb;

static void ulog_telnet_backend_output(struct ulog_backend *backend, const char *log, int len)
{
    if (backend && log && len > 0 && s_ulog_telnet.fd > 0) {
        send(s_ulog_telnet.fd, log, len, 0);      
    }
}

static void _telnet_option(uint8_t option, uint8_t value) //telnet_option(TELNET_DONT,1)
{
    char buffer[16] = {0};

    char *buf           = buffer;
    *buf                = (char)TELNET_IAC;
    *++buf              = (char)option;
    *++buf              = (char)value;
    if (s_ulog_telnet.fd > 0)
        send(s_ulog_telnet.fd, buf, 3, 0);
}

/*telnet 主服务任务*/
static void _telnet_server_listen(void *arg)
{
    int server_sockfd = -1;//服务器端套接字
    int client_sockfd = -1;//客户端套接字
    struct sockaddr_in server_addr;   //服务器网络地址结构体
    struct sockaddr_in remote_addr; //客户端网络地址结构体
    int sin_size;

    memset(&server_addr, 0, sizeof(server_addr)); //数据初始化--清零
    server_addr.sin_family = AF_INET; //设置为IP通信
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//服务器IP地址--允许连接到所有本地地址上
    server_addr.sin_port = htons(23); //服务器telnet端口号

    /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
    if((server_sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        LOGE(TAG, "create error");
        return;
    }
    int n = 1;
    setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(n));

    /*将套接字绑定到服务器的网络地址上*/
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        LOGE(TAG, "bind error");
        return;
    }

    /*监听连接请求*/
    listen(server_sockfd, 1);

    LOGD(TAG, "Telnet: listening for telnet requests....\n");

    sin_size = sizeof(struct sockaddr_in);
    while(s_ulog_telnet.telnet_run) {
        /*等待客户端连接请求到达*/
        if((client_sockfd = accept(server_sockfd, (struct sockaddr *)&remote_addr, (socklen_t *)&sin_size)) < 0)
        {
            LOGE(TAG, "accept error, %d", client_sockfd);
            aos_msleep(3000);
            continue;
        }
        LOGD(TAG, "accept client %s, 0x%x",inet_ntoa(remote_addr.sin_addr), client_sockfd);
        s_ulog_telnet.fd = client_sockfd;
        // /** set socket timeout to 1 s */
        // int timeout_ms = 1 * 1000;
        // struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};

        // if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 100)) {
        //     interval.tv_sec = 0;
        //     interval.tv_usec = 10000;
        // }

        // if (setsockopt(client_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval))) {
        //     LOGE(TAG, "SetSocket SO_RCVTIMEO failed");
        // }

        _telnet_option(TELNET_DO, 34);//客户端关闭行模式
        u_cb(1);
    }
    LOGD(TAG, "exit..........");
    if (client_sockfd > 0)
        close(client_sockfd);
    if (server_sockfd > 0) {
        shutdown(server_sockfd, SHUT_RDWR);
        close(server_sockfd);        
    }
    LOGD(TAG, "connection exit..........");
    aos_task_exit(0);
}

static void _telnet_server_init(struct ulog_backend *backend)
{
    aos_task_t task;

    s_ulog_telnet.telnet_run = 1;
    aos_task_new_ext(&task, "telnetd", _telnet_server_listen, NULL, 4 * 1024, AOS_DEFAULT_APP_PRI);
}

static void _telnet_server_deinit(struct ulog_backend *backend)
{
   s_ulog_telnet.fd = -1;
   s_ulog_telnet.telnet_run = 0; 
}

int ulog_telnet_init(ulog_telnet_event event)
{
    ulog_init();
    telnet.init = _telnet_server_init;
    telnet.output = ulog_telnet_backend_output;
    telnet.deinit = _telnet_server_deinit;
    u_cb = event;
    ulog_backend_register(&telnet, "telnet");

    return 0;
}

int ulog_telnet_deinit(void)
{
    ulog_backend_unregister(&telnet);
    return 0;
}