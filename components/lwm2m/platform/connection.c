/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Pascal Rieux - Please refer to git log
 *
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "connection.h"

#include <std_object.h>
#include <internals.h>
#include <sys/socket.h>

#include <errno.h>

typedef struct {
    lwm2m_object_t *securityObjP;
    lwm2m_object_t *serverObject;
    int sock;
    lwm2m_context_t *lwm2mH;
#ifdef WITH_TINYDTLS
    dtls_connection_t *connList;
#else
    connection_t *connList;
#endif
    int addressFamily;
    sys_sem_t sem;
} client_data_t;

int create_socket(const char *portStr, int addressFamily)
{
    int s = -1;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (0 != getaddrinfo(NULL, portStr, &hints, &res)) {
        return -1;
    }

    for (p = res ; p != NULL && s == -1 ; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        //if (s >= 0) {
        //    if (-1 == bind(s, p->ai_addr, p->ai_addrlen)) {
        //        close(s);
        //        s = -1;
        //    }
        //}
    }

    freeaddrinfo(res);

    if (s > 0) {
        struct timeval tv;
        /*set socket*/
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));

        tv.tv_sec = 10;
        tv.tv_usec = 0;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    }

    return s;
}

connection_t *connection_find(connection_t *connList,
                              struct sockaddr_storage *addr,
                              size_t addrLen)
{
    connection_t *connP;

    connP = connList;

    while (connP != NULL) {
        if ((connP->addrLen == addrLen)
            && (memcmp(&(connP->addr), addr, addrLen) == 0)) {
            return connP;
        }

        connP = connP->next;
    }

    return connP;
}

connection_t *connection_new_incoming(connection_t *connList,
                                      int sock,
                                      struct sockaddr *addr,
                                      size_t addrLen)
{
    connection_t *connP;

    connP = (connection_t *)lwm2m_malloc(sizeof(connection_t));

    if (connP != NULL) {
        connP->sock = sock;
        memcpy(&(connP->addr), addr, addrLen);
        connP->addrLen = addrLen;
        connP->next = connList;
    }

    return connP;
}

connection_t *connection_create(connection_t *connList,
                                int sock,
                                char *host,
                                char *port,
                                int addressFamily)
{
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *p;
    int s;
    struct sockaddr *sa = NULL;
    socklen_t sl = 0;
    connection_t *connP = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    if (0 != getaddrinfo(host, port, &hints, &servinfo) || servinfo == NULL) {
        return NULL;
    }

    // we test the various addresses
    s = -1;

    for (p = servinfo ; p != NULL && s == -1 ; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (s >= 0) {
            sa = p->ai_addr;
            sl = p->ai_addrlen;

            if (-1 == connect(s, p->ai_addr, p->ai_addrlen)) {
                close(s);
                s = -1;
            }
        }
    }

    if (s >= 0) {
        connP = connection_new_incoming(connList, sock, sa, sl);
        close(s);
    }

    if (NULL != servinfo) {
        freeaddrinfo(servinfo);;
    }

    return connP;
}

void connection_free(connection_t *connList)
{
    while (connList != NULL) {
        connection_t *nextP;

        nextP = connList->next;
        free(connList);

        connList = nextP;
    }
}

int connection_send(connection_t *connP,
                    uint8_t *buffer,
                    size_t length)
{
    int nbSent;
    size_t offset;

    offset = 0;

    LOG_ARG("send %d", length);

    while (offset != length) {
        nbSent = sendto(connP->sock, buffer + offset, length - offset, 0, (struct sockaddr *) & (connP->addr), connP->addrLen);

        if (nbSent == -1) {
            return -1;
        }

        offset += nbSent;
    }

    return 0;
}

uint8_t lwm2m_buffer_send(void *sessionH,
                          uint8_t *buffer,
                          size_t length,
                          void *userdata)
{
    connection_t *connP = (connection_t *) sessionH;

    if (connP == NULL) {
        LOG_ARG("#> failed sending %lu bytes, missing connection\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    if (-1 == connection_send(connP, buffer, length)) {
        //LOG_ARG("#> failed sending %lu bytes\r\n", length);
        LOGE("lwm2m", "failed sending %lu bytes", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void *session1,
                            void *session2,
                            void *userData)
{
    return (session1 == session2);
}

void lwm2m_break_recv(lwm2m_context_t *contextP)
{
    client_data_t *dataP;

    dataP = (client_data_t *)contextP->userData;

    if (sys_sem_valid(&dataP->sem)) {

        sys_sem_signal(&dataP->sem);
    }
}

int lwm2m_recv(lwm2m_context_t *contextP, void *buffer, size_t len, time_t timeout)
{
    int result;
    fd_set readfds;
    struct timeval tv = {timeout, 0};
    int fd;
    client_data_t *dataP;

    dataP = (client_data_t *)contextP->userData;

    fd = dataP->sock;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    /*
     * This part will set up an interruption until an event happen on SDTIN or the socket until "tv"timed out (set
     * with the precedent function)
     */

    result = select2(fd + 1, &readfds, NULL, NULL, &tv, &dataP->sem);

    if (result < 0) {
        if (errno != EINTR) {
            LOG_ARG("Error in select(): %d %s\r\n", errno, strerror(errno));
        }
    } else if (result > 0) {
        int numBytes;

        /*
         * If an event happens on the socket
         */
        if (FD_ISSET(fd, &readfds)) {
            struct sockaddr_storage addr;
            socklen_t addrLen;

            addrLen = sizeof(addr);

            /*
             * We retrieve the data received
             */
            numBytes = recvfrom(fd, buffer, len, 0, (struct sockaddr *)&addr, &addrLen);

            if (0 > numBytes) {
                LOG_ARG("Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
            } else if (0 < numBytes) {
                char s[INET_ADDRSTRLEN];
                //in_port_t port = 0;

#ifdef WITH_TINYDTLS
                dtls_connection_t *connP;
#else
                connection_t *connP;
#endif

                if (AF_INET == addr.ss_family) {
                    struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                    inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                    //port = saddr->sin_port;
                }

                LOG_ARG("%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));

                /*
                 * Display it in the STDERR
                 */
                //output_buffer(stderr, buffer, numBytes, 0);

                connP = connection_find(dataP->connList, &addr, addrLen);

                if (connP != NULL) {
                    /*
                        * Let liblwm2m respond to the query depending on the context
                        */
#ifdef WITH_TINYDTLS
                    int result = connection_handle_packet(connP, buffer, numBytes);

                    if (0 != result) {
                        LOG_ARG("error handling message %d\n", result);
                    }

#else
                    lwm2m_handle_packet(contextP, buffer, numBytes, connP);
#endif
                    //conn_s_updateRxStatistic(objArray[7], numBytes, false);
                } else {
                    LOG("received bytes ignored!\r\n");
                }
            }
        }
    }

    return 0;
}

void *lwm2m_connect_init(lwm2m_context_t *ctx)
{
    client_data_t *data = lwm2m_malloc(sizeof(client_data_t));

    if (data) {
        memset(data, 0, sizeof(client_data_t));
        data->addressFamily = AF_INET;
        data->lwm2mH = ctx;

        
        if (sys_sem_new(&data->sem, 0) != ERR_OK) {
            /* failed to create semaphore */
            lwm2m_free(data);
            return NULL;
        }
    }

    return data;
}

void lwm2m_connect_deinit(lwm2m_context_t *ctx)
{
    client_data_t *data = (client_data_t *)ctx->userData;
    sys_sem_free(&data->sem);
    free(data);
}

#ifdef WITH_TINYDTLS
void *lwm2m_connect_server(uint16_t secObjInstID,
                           void *userData)
{
    client_data_t *dataP;
    lwm2m_list_t *instance;
    dtls_connection_t *newConnP = NULL;
    dataP = (client_data_t *)userData;
    lwm2m_object_t   *securityObj = dataP->securityObjP;

    instance = LWM2M_LIST_FIND(dataP->securityObjP->instanceList, secObjInstID);

    if (instance == NULL) {
        return NULL;
    }


    newConnP = connection_create(dataP->connList, dataP->sock, securityObj, instance->id, dataP->lwm2mH, dataP->addressFamily);

    if (newConnP == NULL) {
        LOG_ARG("Connection creation failed.\n");
        return NULL;
    }

    dataP->connList = newConnP;
    return (void *)newConnP;
}
#else

void *lwm2m_connect_server(uint16_t secObjInstID,
                           void *userData)
{
    client_data_t *dataP;
    char *uri;
    char *host;
    char *port;
    connection_t *newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = get_server_uri(dataP->lwm2mH->objectList, secObjInstID);

    if (uri == NULL) {
        return NULL;
    }

    // parse uri in the form "coaps://[host]:[port]"
    if (0 == strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri + strlen("coaps://");
    } else if (0 == strncmp(uri, "coap://",  strlen("coap://"))) {
        host = uri + strlen("coap://");
    } else {
        goto exit;
    }

    port = strrchr(host, ':');

    if (port == NULL) {
        goto exit;
    }

    // remove brackets
    if (host[0] == '[') {
        host++;

        if (*(port - 1) == ']') {
            *(port - 1) = 0;
        } else {
            goto exit;
        }
    }

    // split strings
    *port = 0;
    port++;

    dataP->sock = create_socket(port, dataP->addressFamily);

    if (dataP->sock < 0) {
        goto exit;
    }

    LOG_ARG("Opening connection to server at %s:%s\r\n", host, port);
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);

    if (newConnP == NULL) {
        LOG("Connection creation failed.\r\n");
    } else {
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}
#endif

void lwm2m_close_connection(void *sessionH,
                            void *userData)
{
    client_data_t *app_data;
#ifdef WITH_TINYDTLS
    dtls_connection_t *targetP;
#else
    connection_t *targetP;
#endif

    app_data = (client_data_t *)userData;
#ifdef WITH_TINYDTLS
    targetP = (dtls_connection_t *)sessionH;
#else
    targetP = (connection_t *)sessionH;
#endif

    if (app_data->sock >= 0) {
        close(app_data->sock);
    }

    if (targetP == app_data->connList) {
        app_data->connList = targetP->next;
        lwm2m_free(targetP);
    } else {
#ifdef WITH_TINYDTLS
        dtls_connection_t *parentP;
#else
        connection_t *parentP;
#endif

        parentP = app_data->connList;

        while (parentP != NULL && parentP->next != targetP) {
            parentP = parentP->next;
        }

        if (parentP != NULL) {
            parentP->next = targetP->next;
            lwm2m_free(targetP);
        }
    }

    //free(app_data);
}
