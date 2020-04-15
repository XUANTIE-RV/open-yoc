/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include "cis_api.h"
#include "cis_if_net.h"

#include <stdio.h>
#include <cis_list.h>
#include <cis_if_sys.h>
#include <cis_log.h>
#include <cis_if_net.h>

#include <errno.h>
#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <lwip/netdb.h>

#define MY_PORT               0
#define PEER_PORT             5683
#define MAX_PACKET_SIZE       512  // Maximal size of radio packet

struct st_cisnet_context {
    int sock;
    char host[32];
    uint16_t port;

    sa_family_t family;
};

static char g_net_recv_pool[MAX_PACKET_SIZE];
static cisnet_callback_t g_callback;
static sys_sem_t g_sem;

cis_ret_t cisnet_init(void *context, const cisnet_config_t *config, cisnet_callback_t cb)
{
    g_callback.onEvent  = cb.onEvent;
    g_callback.userData = cb.userData;
    return CIS_RET_OK;
}

cis_ret_t cisnet_deinit(void* p)
{
    g_callback.onEvent  = NULL;
    g_callback.userData = NULL;
    return CIS_RET_OK;
}

cis_ret_t cisnet_create(void *context, cisnet_t *ctx, const char *host)
{
    int fd;
    int rc = 0;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, NULL, NULL, NULL};

    if (sys_sem_new(&g_sem, 0) != ERR_OK) {
        return CIS_RET_ERROR;
    }

    (*ctx) = (cisnet_t)cissys_malloc(sizeof(struct st_cisnet_context));
    if ((*ctx) == NULL) {
        sys_sem_free(&g_sem);
        CIS_LOGE("Cannot malloc cisnet_t");
        return CIS_RET_ERROR;
    }

    if ((rc = getaddrinfo(host, NULL, &hints, &result)) == 0) {
        struct addrinfo *res = result;

        /* prefer ip4 addresses */
        while (res) {
            if (res->ai_family == AF_INET) {
                result = res;
                break;
            }

            res = res->ai_next;
        }

        if (result->ai_family == AF_INET) {
            inet_ntop(AF_INET, &((struct sockaddr_in *)(result->ai_addr))->sin_addr,
                    (*ctx)->host, sizeof((*ctx)->host));
            (*ctx)->port   = PEER_PORT;
            (*ctx)->family = AF_INET;
        } else {
            rc = -1;
        }

        freeaddrinfo(result);
    }

    if (rc == 0) {
        fd = socket((*ctx)->family, SOCK_DGRAM, 0);
        (*ctx)->sock = fd;
        if (fd < 0) {
            free(*ctx);
            sys_sem_free(&g_sem);
            return CIS_RET_ERROR;
        }
        return CIS_RET_OK;
    }

    free(*ctx);
    sys_sem_free(&g_sem);
    return CIS_RET_ERROR;
}

void cisnet_destroy(void *context, cisnet_t ctx)
{
    if (NULL == ctx || ctx->sock < 0) {
        return;
    }

    close(ctx->sock);
    ctx->sock = -1;
    sys_sem_free(&g_sem);
    free(ctx);
}

cis_ret_t cisnet_connect(void *context, cisnet_t ctx)
{
    g_callback.onEvent(ctx, cisnet_event_connected, NULL, g_callback.userData);
    return CIS_RET_OK;
}


uint8_t cisnet_attached_state(void *ctx)
{
    return 1;
}

cis_ret_t cisnet_disconnect(void *context, cisnet_t ctx)
{
    return CIS_RET_OK;
}

cis_ret_t cisnet_write(void *context, cisnet_t ctx, const uint8_t *buffer, uint32_t  length)
{
    int fd = -1;
    int bytes = 0;
    int recv;
    struct sockaddr_in remote_addr;

    if (NULL == ctx || ctx->sock < 0) {
        return CIS_RET_INVILID;
    }

    fd = ctx->sock;
    remote_addr.sin_family = ctx->family;

    if (1 != inet_pton(remote_addr.sin_family, ctx->host,
                       &remote_addr.sin_addr.s_addr)) {
        return CIS_RET_ERROR;
    }
    remote_addr.sin_port = htons(ctx->port);

    while (length) {
        recv = sendto(fd, buffer + bytes, (size_t)length, 0,
                (const struct sockaddr *)&remote_addr, sizeof(remote_addr));

        if (-1 == recv) {
            return CIS_RET_TIMEOUT;
        }
        length -= recv;
        bytes  += recv;
    }

    return CIS_RET_OK;
}

cis_ret_t cisnet_read(void *context, cisnet_t ctx, uint8_t **buffer, uint32_t *length, cis_time_t timeout)
{
    int ret = -1;
    int fd = -1;
    int count = -1;
    struct sockaddr from;
    int addrlen = 0;
    struct timeval tv;

    if (NULL == ctx || ctx->sock < 0) {
        return CIS_RET_INVILID;
    }

    fd = ctx->sock;
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec  = timeout ;
    tv.tv_usec = 0;

    ret = select2(fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv, &g_sem);

    /* Zero fds ready means we timed out */
    if (ret == 0) {
        return -2;    // receive timeout
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return -3;    //want read
        }

        return -4; //receive failed
    }

    addrlen = sizeof(struct sockaddr);
    count = recvfrom(fd, g_net_recv_pool, (size_t)MAX_PACKET_SIZE, 0, &from,
                     (socklen_t*)&addrlen);

    if (-1 == count) {
        return CIS_RET_TIMEOUT;
    }

    *buffer = (uint8_t*)g_net_recv_pool;
    *length = count;

    return CIS_RET_OK;
}

void cisnet_break_recv(void *contextP, cisnet_t ctx)
{
    if (sys_sem_valid(&g_sem)) {

        sys_sem_signal(&g_sem);
    }
}

cis_ret_t cisnet_free(void *context, cisnet_t ctx, uint8_t *buffer, uint32_t length)
{
    if (NULL == ctx || ctx->sock < 0) {
        return CIS_RET_INVILID;
    }

    return CIS_RET_OK;
}
