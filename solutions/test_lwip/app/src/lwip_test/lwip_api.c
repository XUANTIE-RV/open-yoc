/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include "../yunit.h"
#include <ulog/ulog.h>
#include <aos/aos.h>
#include <lwip/sockets.h>

#define TAG "lwip_func"
#define DEFAULTFLAG 0
#define OTHERFLAG 1

ip_addr_t     ipaddr;
ip_addr_t     netmask;
ip_addr_t     gw;
ip_addr_t     ipaddr_pool_start;
ip_addr_t     ipaddr_pool_end;
static struct iperf_param param;

typedef unsigned int u32;

static int iSockFD;
static char server_ip[16] = "0.0.0.1";

struct iperf_param {
    u32 buf_size;
    bool server; /** server/client*/
    bool udp;
    char server_ip[16];
    u32 port;
    u32 second;
    u32 report_interval;
    u32 socket_timeout;
};

static void test_lwip_socket(){
    int ret;
    //test valid parameter
    iSockFD = lwip_socket(AF_INET, SOCK_STREAM, 0);
    YUNIT_ASSERT_MSG_QA(iSockFD==LWIP_SOCKET_OFFSET, "iSockFD=%d", iSockFD, "YOC_lwip_01");

    //test invalid parameter
    ret = lwip_socket(-1, 4, 0);
    YUNIT_ASSERT_MSG_QA(ret==-1, "ret=%d", ret, "YOC_lwip_02");

    //open one more socket
    ret = lwip_socket(AF_INET, SOCK_STREAM, 0);
    YUNIT_ASSERT_MSG_QA(ret==LWIP_SOCKET_OFFSET+1, "ret=%d", ret, "YOC_lwip_03");
}

static void test_lwip_connect(){
    struct sockaddr_in  sAddr;
    int iStatus;
    param.buf_size = 10 * 1024; //default 10K buffer size for tcp
    param.port = 5001;
    param.report_interval = 1;
    param.second = 10;
    param.server = 0;
    param.udp = 0;
    param.socket_timeout = 0;
    strcpy(param.server_ip,server_ip);
    int iAddrSize;
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = lwip_htons(param.port);
    sAddr.sin_addr.s_addr = inet_addr(param.server_ip);
    iAddrSize = sizeof(struct sockaddr_in);
    //socket does not exist 
    iStatus = lwip_connect(-1, (struct sockaddr *)&sAddr, iAddrSize);
    YUNIT_ASSERT_MSG_QA(iStatus==-1, "iStatus=%d", iStatus, "YOC_lwip_04");
}

static void test_lwip_setsockopt(){
    int n = 1;
    int ret;
    param.server = 1;
    iSockFD = lwip_socket(AF_INET, SOCK_STREAM, 0);
    //test valid parameter
    ret = lwip_setsockopt(iSockFD, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(n));
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_lwip_05");

    //socket does not exist
    ret = lwip_setsockopt(-1, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(n));
    YUNIT_ASSERT_MSG_QA(ret==-1, "ret=%d", ret, "YOC_lwip_06");

    //option name is NULL
    ret = lwip_setsockopt(iSockFD, SOL_SOCKET, SO_REUSEADDR, NULL, 0);
    YUNIT_ASSERT_MSG_QA(ret==-1, "ret=%d", ret, "YOC_lwip_07");
}

static void test_lwip_send(){
    char *cBsdBuf = NULL;
    int iStatus;
    cBsdBuf = aos_malloc(param.buf_size);
    int iCounter;

    for (iCounter = 0; iCounter < param.buf_size; iCounter++) {
        cBsdBuf[iCounter] = (char)(iCounter % 10);
    }
    //socket does not exist
    iStatus = lwip_send(-1, cBsdBuf, param.buf_size, DEFAULTFLAG);
    YUNIT_ASSERT_MSG_QA(iStatus==-1, "iStatus=%d", iStatus, "YOC_lwip_08");

    //flag is 0
    iStatus = lwip_send(iSockFD, 0, 0, DEFAULTFLAG);
    YUNIT_ASSERT_MSG_QA(iStatus==0, "iStatus=%d", iStatus, "YOC_lwip_09");

    //flag is 1
    iStatus = lwip_send(iSockFD, 0, 0, OTHERFLAG);
    YUNIT_ASSERT_MSG_QA(iStatus==0, "iStatus=%d", iStatus, "YOC_lwip_10");
}

static void test_lwip_bind(){
    int iStatus;
    int iAddrSize;
    struct sockaddr_in  sLocalAddr;
    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family      = AF_INET;
    sLocalAddr.sin_len         = sizeof(sLocalAddr);
    sLocalAddr.sin_port        = lwip_htons(param.port);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    iAddrSize = sizeof(sLocalAddr);

    //test valid parameter
    iStatus = lwip_bind(iSockFD, (struct sockaddr *)&sLocalAddr, iAddrSize);
    YUNIT_ASSERT_MSG_QA(iStatus==0, "iStatus=%d", iStatus, "YOC_lwip_11");

    //iAddrSize < 0
    iStatus = lwip_bind(iSockFD, (struct sockaddr *)&sLocalAddr, -1);
    YUNIT_ASSERT_MSG_QA(iStatus==-1, "iStatus=%d", iStatus, "YOC_lwip_12");

    //socket does not exist
    iStatus = lwip_bind(-1, (struct sockaddr *)&sLocalAddr, iAddrSize);
    YUNIT_ASSERT_MSG_QA(iStatus==-1, "iStatus=%d", iStatus, "YOC_lwip_13");
}

static void test_lwip_listen(){
    int iStatus;
    //test valid parameter
    iStatus = lwip_listen(iSockFD, 1);
    YUNIT_ASSERT_MSG_QA(iStatus==0, "iStatus=%d", iStatus, "YOC_lwip_14");

    //socket does not exist
    iStatus = lwip_listen(-1, 1);
    YUNIT_ASSERT_MSG_QA(iStatus==-1, "iStatus=%d", iStatus, "YOC_lwip_15");

    //backlog is 0
    iStatus = lwip_listen(iSockFD, 0);
    YUNIT_ASSERT_MSG_QA(iStatus==0, "iStatus=%d", iStatus, "YOC_lwip_16");

    //backlog is max
    iStatus = lwip_listen(iSockFD, 255);
    YUNIT_ASSERT_MSG_QA(iStatus==0, "iStatus=%d", iStatus, "YOC_lwip_17");

    //backlog is max+1
    iStatus = lwip_listen(iSockFD, 256);
    YUNIT_ASSERT_MSG_QA(iStatus==0, "iStatus=%d", iStatus, "YOC_lwip_18");
}

static void test_lwip_sendto(){
    int cli_sockfd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    char *buffer;
    struct sockaddr_in cli_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    buffer = (char *)aos_malloc(param.buf_size);
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr(param.server_ip);
    cli_addr.sin_port = lwip_htons(param.port);
    //socket does not exist
    int ret = lwip_sendto(-1, buffer, param.buf_size, DEFAULTFLAG, (struct sockaddr *)&cli_addr, addrlen);
    YUNIT_ASSERT_MSG_QA(ret==-1, "ret=%d", ret, "YOC_lwip_19");

    //test valid parameter
    ret = lwip_sendto(cli_sockfd, buffer, param.buf_size, DEFAULTFLAG, (struct sockaddr *)&cli_addr, addrlen);
    YUNIT_ASSERT_MSG_QA(ret!=-1, "ret=%d", ret, "YOC_lwip_20");

    //data is NULL
    ret = lwip_sendto(cli_sockfd, 0, 0, DEFAULTFLAG, (struct sockaddr *)&cli_addr, addrlen);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_lwip_21");

    //flag is 1
    ret = lwip_sendto(cli_sockfd, 0, 0, OTHERFLAG, (struct sockaddr *)&cli_addr, addrlen);
    YUNIT_ASSERT_MSG_QA(ret==0, "ret=%d", ret, "YOC_lwip_22");
}

static void test_lwip_all(void){
    test_lwip_socket();
    test_lwip_connect();
    test_lwip_send();
    test_lwip_setsockopt();
    test_lwip_bind();
    test_lwip_listen();
    test_lwip_sendto();
}

void lwipapi_test_entry(yunit_test_suite_t *suite){
    yunit_add_test_case(suite, "api_lwip", test_lwip_all);
}