// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <stdlib.h>
#include <string.h>

#include <aos/queue.h>

#include "transport/transport.h"
#include "transport/transport_utils.h"

static const char *TAG = "TRANSPORT";

/**
 * Transport layer structure, which will provide functions, basic properties for transport types
 */
struct transport_item_t {
    int             port;
    int             socket;         /*!< Socket to use in this transport */
    char            *scheme;        /*!< Tag name */
    void            *context;       /*!< Context data */
    void            *data;          /*!< Additional transport data */
    connect_func    _connect;       /*!< Connect function of this transport */
    io_read_func    _read;          /*!< Read */
    io_func         _write;         /*!< Write */
    trans_func      _close;         /*!< Close */
    poll_func       _poll_read;     /*!< Poll and read */
    poll_func       _poll_write;    /*!< Poll and write */
    trans_func      _destroy;       /*!< Destroy and free transport */
    connect_async_func _connect_async;      /*!< non-blocking connect function of this transport */
    payload_transfer_func  _parent_transfer;       /*!< Function returning underlying transport layer */

    STAILQ_ENTRY(transport_item_t) next;
};


/**
 * This list will hold all transport available
 */
STAILQ_HEAD(transport_list_t, transport_item_t);

static transport_handle_t transport_get_default_parent(transport_handle_t t)
{
    /*
    * By default, the underlying transport layer handle is the handle itself
    */
    return t;
}

transport_list_handle_t transport_list_init()
{
    transport_list_handle_t list = calloc(1, sizeof(struct transport_list_t));
    TRANSPORT_MEM_CHECK(TAG, list, return NULL);
    STAILQ_INIT(list);
    return list;
}

web_err_t transport_list_add(transport_list_handle_t list, transport_handle_t t, const char *scheme)
{
    if (list == NULL || t == NULL) {
        return WEB_ERR_INVALID_ARG;
    }
    t->scheme = calloc(1, strlen(scheme) + 1);
    TRANSPORT_MEM_CHECK(TAG, t->scheme, return WEB_ERR_NO_MEM);
    strcpy(t->scheme, scheme);
    STAILQ_INSERT_TAIL(list, t, next);
    return WEB_OK;
}

transport_handle_t transport_list_get_transport(transport_list_handle_t list, const char *scheme)
{
    if (!list) {
        return NULL;
    }
    if (scheme == NULL) {
        return STAILQ_FIRST(list);
    }
    transport_handle_t item;
    STAILQ_FOREACH(item, list, next) {
        if (strcasecmp(item->scheme, scheme) == 0) {
            return item;
        }
    }
    return NULL;
}

web_err_t transport_list_destroy(transport_list_handle_t list)
{
    transport_list_clean(list);
    free(list);
    return WEB_OK;
}

web_err_t transport_list_clean(transport_list_handle_t list)
{
    transport_handle_t item = STAILQ_FIRST(list);
    transport_handle_t tmp;
    while (item != NULL) {
        tmp = STAILQ_NEXT(item, next);
        if (item->_destroy) {
            item->_destroy(item);
        }
        transport_destroy(item);
        item = tmp;
    }
    STAILQ_INIT(list);
    return WEB_OK;
}

transport_handle_t transport_init()
{
    transport_handle_t t = calloc(1, sizeof(struct transport_item_t));
    TRANSPORT_MEM_CHECK(TAG, t, return NULL);
    return t;
}

transport_handle_t transport_get_payload_transport_handle(transport_handle_t t)
{
    if (t && t->_read) {
        return t->_parent_transfer(t);
    }
    return NULL;
}

web_err_t transport_destroy(transport_handle_t t)
{
    if (t->scheme) {
        free(t->scheme);
    }
    free(t);
    return WEB_OK;
}

int transport_connect(transport_handle_t t, const char *host, int port, int timeout_ms)
{
    int ret = -1;
    if (t && t->_connect) {
        return t->_connect(t, host, port, timeout_ms);
    }
    return ret;
}

int transport_connect_async(transport_handle_t t, const char *host, int port, int timeout_ms)
{
    int ret = -1;
    if (t && t->_connect_async) {
        return t->_connect_async(t, host, port, timeout_ms);
    }
    return ret;
}

int transport_read(transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    if (t && t->_read) {
        return t->_read(t, buffer, len, timeout_ms);
    }
    return -1;
}

int transport_write(transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    if (t && t->_write) {
        return t->_write(t, buffer, len, timeout_ms);
    }
    return -1;
}

int transport_poll_read(transport_handle_t t, int timeout_ms)
{
    if (t && t->_poll_read) {
        return t->_poll_read(t, timeout_ms);
    }
    return -1;
}

int transport_poll_write(transport_handle_t t, int timeout_ms)
{
    if (t && t->_poll_write) {
        return t->_poll_write(t, timeout_ms);
    }
    return -1;
}

int transport_close(transport_handle_t t)
{
    if (t && t->_close) {
        return t->_close(t);
    }
    return 0;
}

void *transport_get_context_data(transport_handle_t t)
{
    if (t) {
        return t->data;
    }
    return NULL;
}

// void *transport_get_n(transport_handle_t t)
// {
//     if (t) {
//         return &t->n;
//     }
//     return NULL;
// }

web_err_t transport_set_context_data(transport_handle_t t, void *data)
{
    if (t) {
        t->data = data;
        return WEB_OK;
    }
    return WEB_FAIL;
}

web_err_t transport_set_func(transport_handle_t t,
                             connect_func _connect,
                             io_read_func _read,
                             io_func _write,
                             trans_func _close,
                             poll_func _poll_read,
                             poll_func _poll_write,
                             trans_func _destroy)
{
    if (t == NULL) {
        return WEB_FAIL;
    }
    t->_connect = _connect;
    t->_read = _read;
    t->_write = _write;
    t->_close = _close;
    t->_poll_read = _poll_read;
    t->_poll_write = _poll_write;
    t->_destroy = _destroy;
    t->_connect_async = NULL;
    t->_parent_transfer = transport_get_default_parent;
    return WEB_OK;
}

int transport_get_default_port(transport_handle_t t)
{
    if (t == NULL) {
        return -1;
    }
    return t->port;
}

web_err_t transport_set_default_port(transport_handle_t t, int port)
{
    if (t == NULL) {
        return WEB_FAIL;
    }
    t->port = port;
    return WEB_OK;
}

web_err_t transport_set_async_connect_func(transport_handle_t t, connect_async_func _connect_async_func)
{
    if (t == NULL) {
        return WEB_FAIL;
    }
    t->_connect_async = _connect_async_func;
    return WEB_OK;
}

web_err_t transport_set_parent_transport_func(transport_handle_t t, payload_transfer_func _parent_transport)
{
    if (t == NULL) {
        return WEB_FAIL;
    }
    t->_parent_transfer = _parent_transport;
    return WEB_OK;
}
