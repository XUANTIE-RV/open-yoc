/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <unistd.h>
#include <errno.h>
#include <lwip/netdb.h>
#include <sys/select.h>
#include <yoc/aui_cloud.h>
#include <aos/kv.h>
#include <webclient.h>
#include "mbedtls/base64.h"
#include "mbedtls/md5.h"
#include "cJSON.h"
#include "pcm_buffer.h"

#include "aui_cloud_imp.h"

#define TAG "BAIDU_ASR"

enum baidu_status_t {
    BAIDU_BUFFER_INIT = 0,
    BAIDU_BUFFER_ONGOING,
    BAIDU_BUFFER_SOCK_ERR,
    BAIDU_BUFFER_END,
};

#define BAIDU_ASR_JSON_RESULT_LEN (2 * 1024)
#define SOCKET_READ_TIMEOUT (3 * 1000)
#define SOCKET_WRITE_TIMEOUT (500)
#define SOCKET_WRITE_EVERY (1 * 1024)
#define BAIDU_ZERO_AUDIO_SIZE (1)

typedef struct baidu_context {
    struct pcm_buffer_list buf_list;
    int                    status;
    int                    finish;
    int                    network_err;
} baidu_context_t;

/**
    For Test debug for invalid response
    from Proxy server
*/
static void invalid_buffer_dump(unsigned char *buffer, int len)
{
    int j = 0;
    for (j = 0; j < len; j++) {
        if (0 == (j & (16 - 1))) //print 16 hex every line
            printf("\n%06x ", j);
        printf(" %02x", buffer[j] & 0xff);
    }
}

static int baidu_result_extract(aui_t *aui, char *buffer, int buffer_len)
{
    int content_length = 0;
    int ret            = AUI_ERR_FAIL;

    sscanf(buffer, "%*[^0-9]%i", &content_length);
    if (content_length > 0) {
        char *content = strstr(buffer, "\r\n");

        if (content == NULL) {
            LOGE(TAG, "Invalid buffer format from server, content length=%d", content_length);
            invalid_buffer_dump((unsigned char *)buffer, buffer_len);
            ret = AUI_ERR_FAIL;
            goto end;
        }

        content += strlen("\r\n\r\n");
        if (strlen(content) == content_length) {
            if (aui->config.nlp_cb)
                aui->config.nlp_cb(content);
            ret = AUI_ERR_OK;
        } else {
            LOGE(TAG, "content length not match %d %d", strlen(content), content_length);
            ret = AUI_ERR_FAIL;
        }
    } else {
        LOGE(TAG, "content length not ok, buffer=%s", buffer);
        ret = AUI_ERR_FAIL;
    }
end:
    return ret;
}

static int baidu_socket_read(aui_t *aui, int socket_fd, int timeout_ms)
{

    int ret = AUI_ERR_FAIL;

    /** set socket options */
    struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 100)) {
        interval.tv_sec  = 0;
        interval.tv_usec = 10000;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval)))
        return -1;

    int   len    = BAIDU_ASR_JSON_RESULT_LEN;
    char *buffer = aos_zalloc(len);

    int bytes = 0;
    while (bytes < len) {
        int rc = recv(socket_fd, &buffer[bytes], (size_t)(len - bytes), 0);
        if (rc == -1) {
            LOGD(TAG, "baidu recv result fail=%d,%d ", rc, errno);
            if (errno != ENOTCONN && errno != ECONNRESET) {
                //bytes = -1;
            }
            break;
        } else if (rc == 0) {
            bytes = 0;
            break;
        } else {
            bytes += rc;
            if (bytes >
                30) { /** 30 is margin, prevent call 'baidu_result_extract' too many times */
                if (AUI_ERR_OK == baidu_result_extract(aui, buffer, bytes)) {
                    ret = AUI_ERR_OK;
                    break;
                }
            }
        }
    }

    aos_free(buffer);

    /** fail callback */
    if (AUI_ERR_FAIL == ret) {
        if (aui->config.nlp_cb)
            aui->config.nlp_cb("{\"error\":\"baidu asr failed\"}");
    }

    return ret;
}

static int baidu_socket_write(int socket_fd, unsigned char *buffer, int len, int timeout_ms)
{
    int rc = 0;

    struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 100)) {
        interval.tv_sec  = 1;
        interval.tv_usec = 10000;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&interval, sizeof(struct timeval)))
        return -1;

    int length_every = SOCKET_WRITE_EVERY;
    int length_left  = len;

    while (length_left > 0) {
        int length_cur = length_left > length_every ? length_every : length_left;
        rc             = send(socket_fd, buffer + (len - length_left), length_cur, 0);

        if (rc < 0) {
            if (errno == EPIPE || errno == ECONNRESET) {
                LOGE(TAG, "socket err connection reset");
                break;
            } else if ((errno == EINTR) || (errno == EAGAIN)) {
                LOGE(TAG, "socket wants write more");
                continue;
            } else {
                LOGE(TAG, "errno=%d", errno);
            }
            break;
        }

        if (rc != length_cur) {
            LOGD(TAG, "Wanted=%d, Real=%d", length_cur, rc);
        }

        length_left -= rc;
    }

    return rc;
}

/**
    Connect to baidu socket
    OK  : AUI_ERR_OK
    FAIL: AUI_ERR_FAIL
*/
static int baidu_socket_connect(int *out_socket_fd, char *addr, int port, int timeout_second)
{
    int                type = SOCK_STREAM;
    struct sockaddr_in address;
    int                rc        = AUI_ERR_FAIL;
    int                socket_fd = -1;
    sa_family_t        family    = AF_INET;
    struct addrinfo *  result    = NULL;
    struct addrinfo    hints     = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0) {
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
            address.sin_port   = htons(port);
            address.sin_family = family = AF_INET;
            address.sin_addr            = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
        } else {
            goto end;
        }

        freeaddrinfo(result);
    }

    if (rc == 0) {
        socket_fd = socket(family, type, IPPROTO_TCP);
        if (socket_fd != -1) { //when socket_fd poll is used up, it will return -1
            /* set socket to non-blocking */
            int flags = lwip_fcntl(socket_fd, F_GETFL, 0);
            rc        = lwip_fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

            struct timeval tv;
            tv.tv_sec  = timeout_second;
            tv.tv_usec = 0;

            rc = connect(socket_fd, (struct sockaddr *)&address, sizeof(address));
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(socket_fd, &fds);
            rc = select(socket_fd + 1, NULL, &fds, NULL, &tv);
            LOGD(TAG, "socket connect select");

            if (rc == -1) {
                LOGE(TAG, "socket connect select error. errno = %d", errno);
            } else if (rc == 1) {
                ; //OK case
            } else if (rc == 0) {
                LOGE(TAG, "socket connect select timeout. errno = %d", errno);
            } else {
                LOGE(TAG, "socket connect select unknowd error. errno = %d", errno);
            }

            /* restore socket to blocking */
            lwip_fcntl(socket_fd, F_SETFL, flags);

            //rc = connect(socket_fd, (struct sockaddr *)&address, sizeof(address));
            *out_socket_fd = socket_fd;
            if (rc == 1) {
                rc = AUI_ERR_OK;
            } else {
                //LOGE(TAG, "Socket connect failed");
                rc = AUI_ERR_FAIL;
                close(socket_fd);
            }
        } else {
            LOGE(TAG, "Socket used up");
        }
    } else {
        rc = AUI_ERR_FAIL;
    }
end:
    return rc;
}

static void baidu_socket_close(int socket_fd)
{
    close(socket_fd);
}

static void baidu_cloud_result_get(aui_t *aui, int socketfd)
{
    /** notify server this is the last packet */
    baidu_socket_write(socketfd, (unsigned char *)"END", 3, SOCKET_WRITE_TIMEOUT);
    baidu_socket_read(aui, socketfd, SOCKET_READ_TIMEOUT);
    /** Read all and close the socket, 
       server will not close socket for me */
    baidu_socket_close(socketfd);

    return;
}

static void baidu_pcm_send(int socketfd, struct pcm_buffer_list *buffer_list)
{
    struct pcm_buffer_list pcm_buf_list_tmp;
    pcm_buffer_init(&pcm_buf_list_tmp);

    /** loop the buffer and send */
    pcm_buffer_move(&pcm_buf_list_tmp, buffer_list);
    /* PCM -> BAIDU %d Bytes: pcm_buffer_total_size(&pcm_buf_list_cloud)); */

    int                     i    = 0;
    struct pcm_buffer_list *list = &pcm_buf_list_tmp;
    for (i = 0; i < sizeof(list->buffer) / sizeof(list->buffer[0]); i++) {
        struct pcm_buffer *buffer = &list->buffer[i];
        if (buffer->valid) {
            int rc = baidu_socket_write(socketfd, (unsigned char *)buffer->buffer, buffer->used,
                                        SOCKET_WRITE_TIMEOUT);
            if (rc < 0)
                break;
        }
    }
    pcm_buffer_free(&pcm_buf_list_tmp);
    aos_mutex_free(&pcm_buf_list_tmp.mutex);
}

/**
    Main task to drain the buffer and send to cloud
*/
static void baidu_cloud_task(void *arg)
{

    int              sockfd  = -1;
    int              retry   = 1;
    aui_t *          aui     = (aui_t *)arg;
    baidu_context_t *context = (baidu_context_t *)aui->context;

    char ars_ip_port[32];
    int asr_port = 0;
    int ret = aos_kv_getstring("asr_server", ars_ip_port, sizeof(ars_ip_port));
    if (ret > 0) {
        char *str_port = strchr(ars_ip_port, ':');
        if (str_port) {
            *str_port = '\0';
            str_port ++;
        }
        asr_port = atoi(str_port);
    } else {
        asr_port = BAIDU_ASR_PORT;
        strcpy(ars_ip_port, BAIDU_ASR_IP);
    }

    /** init status */
    while (AUI_ERR_OK != baidu_socket_connect(&sockfd, ars_ip_port, asr_port, 5)) {
        LOGE(TAG, "Baidu Cloud connect fail(%s:%d), retry=%d", ars_ip_port, asr_port, retry);
        if (--retry == 0) {
            context->network_err = 1;
            context->status      = BAIDU_BUFFER_END;
            if (aui->config.nlp_cb) {
                aui->config.nlp_cb("{\"aui_result\":-101,\"msg\":\"baidu cloud connect fail(ENETUNREACH)\"}");
            }
            aos_task_exit(0);
        }
    }

    while (context->finish == 0) {
        if (pcm_buffer_total_size(&context->buf_list) != 0)
            baidu_pcm_send(sockfd, &context->buf_list);
        else
            aos_msleep(50);
    }

    LOGD(TAG, "Baidu Cloud Read result");
    baidu_cloud_result_get(aui, sockfd);
    context->status = BAIDU_BUFFER_END;

    aos_task_exit(0);
}

/** init only once with multiple talks */
void baidu_asr_init(aui_t *aui)
{
    baidu_context_t *context = aos_malloc(sizeof(baidu_context_t));
    pcm_buffer_init(&context->buf_list);
    context->status      = BAIDU_BUFFER_END;
    context->network_err = 0;
    context->finish      = 0;
    aui->context         = context;
}

int baidu_asr_source_pcm_finish(aui_t *aui)
{
    baidu_context_t *context = (baidu_context_t *)aui->context;

    context->finish = 1;
    while (context->status != BAIDU_BUFFER_END) {
        LOGD(TAG, "Wait thread exit");
        aos_msleep(1000);
    }
    context->network_err = 0;
    context->finish      = 0;
    pcm_buffer_free(&context->buf_list);

    return 0;
}

int baidu_asr_source_pcm(aui_t *aui, char *data, int size)
{
    int ret = -1;
    baidu_context_t *context = (baidu_context_t *)aui->context;
    if (context->status == BAIDU_BUFFER_END) {
        context->status = BAIDU_BUFFER_ONGOING;
        aos_task_t task_handle; /** create&run task using 30ms*/
        aos_task_new_ext(&task_handle, "baidu_cloud_task", baidu_cloud_task, aui, 4 * 1024,
                         AOS_DEFAULT_APP_PRI); /** todo shrink the stack size used */
    }

    if (context->network_err) {
        ret = -1;
        goto end;
    }

    ret = pcm_buffer_copy(&context->buf_list, data, size);
end:
    if (ret < 0)
        LOGE(TAG, "Source PCM Error\n");
    return ret;
}
