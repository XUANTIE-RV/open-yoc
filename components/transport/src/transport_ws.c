#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include "transport.h"
#include "transport_tcp.h"
#include "transport_ws.h"
#include "transport_utils.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha1.h"

static const char *TAG = "TRANSPORT_WS";

// #define DEFAULT_WS_BUFFER (1024)
#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a
// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127
#define MAX_WEBSOCKET_HEADER_SIZE 10
#define WS_RESPONSE_OK    101

struct transport_ws {
    const char *path;
    const char *protocol;
    char *buffer;
    size_t buffer_size;
    int send_type;
    transport_handle_t parent;
};

typedef struct transport_ws transport_ws_t;

static transport_handle_t ws_get_payload_transport_handle(transport_handle_t t)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return ws->parent;
}

static char *trimwhitespace(const char *str)
{
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) {
        return (char *)str;
    }

    // Trim trailing space
    end = (char *)(str + strlen(str) - 1);
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

    return (char *)str;
}


static char *get_http_header(const char *buffer, const char *key)
{
    char *found = strstr(buffer, key);
    if (found) {
        found += strlen(key);
        char *found_end = strstr(found, "\r\n");
        if (found_end) {
            found_end[0] = 0;//terminal string

            return trimwhitespace(found);
        }
    }
    return NULL;
}

static int ws_connect(transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    if (transport_connect(ws->parent, host, port, timeout_ms) < 0) {
        LOGE(TAG, "Error connect to ther server");
        return -1;
    }

    LOGD(TAG, "connect %s:%d ok", host, port);
    unsigned char random_key[16];
    // getrandom(random_key, sizeof(random_key), 0);
    transport_utils_random(random_key, sizeof(random_key));

    // Size of base64 coded string is equal '((input_size * 4) / 3) + (input_size / 96) + 6' including Z-term
    unsigned char client_key[28] = {0};

    size_t outlen = 0;
    mbedtls_base64_encode(client_key, sizeof(client_key), &outlen, random_key, sizeof(random_key));
    int len = snprintf(ws->buffer, ws->buffer_size,
                         "GET %s HTTP/1.1\r\n"
                         "Connection: Upgrade\r\n"
                         "Host: %s:%d\r\n"
                         "Upgrade: websocket\r\n"
                         "Sec-WebSocket-Version: 13\r\n"
                         "Sec-WebSocket-Protocol: %s\r\n"
                         "Sec-WebSocket-Key: %s\r\n"
                         "User-Agent: CK Websocket Client\r\n\r\n",
                         ws->path,
                         host, port,
                         ws->protocol ? ws->protocol : " ",
                         client_key);
    if (len <= 0 || len >= ws->buffer_size) {
        LOGE(TAG, "Error in request generation, %d", len);
        return -1;
    }
    LOGD(TAG, "Write upgrate request\r\n%s", ws->buffer);
    if (transport_write(ws->parent, ws->buffer, len, timeout_ms) <= 0) {
        LOGE(TAG, "Error write Upgrade header %s", ws->buffer);
        return -1;
    }
    if ((len = transport_read(ws->parent, ws->buffer, ws->buffer_size, timeout_ms)) <= 0) {
        LOGE(TAG, "Error read response for Upgrade header %s", ws->buffer);
        return -1;
    }
    char *server_key = get_http_header(ws->buffer, "Sec-WebSocket-Accept:");
    if (server_key == NULL) {
        LOGE(TAG, "Sec-WebSocket-Accept not found");
        return -1;
    }

    // See mbedtls_sha1_ret() arg size
    unsigned char expected_server_sha1[20];
    // Size of base64 coded string see above
    unsigned char expected_server_key[33] = {0};
    // If you are interested, see https://tools.ietf.org/html/rfc6455
    const char expected_server_magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    unsigned char expected_server_text[sizeof(client_key) + sizeof(expected_server_magic) + 1];
    strcpy((char*)expected_server_text, (char*)client_key);
    strcat((char*)expected_server_text, expected_server_magic);

    size_t key_len = strlen((char*)expected_server_text);
    mbedtls_sha1(expected_server_text, key_len, expected_server_sha1);
    mbedtls_base64_encode(expected_server_key, sizeof(expected_server_key),  &outlen, expected_server_sha1, sizeof(expected_server_sha1));
    expected_server_key[ (outlen < sizeof(expected_server_key)) ? outlen : (sizeof(expected_server_key) - 1) ] = 0;
    LOGD(TAG, "server key=%s, send_key=%s, expected_server_key=%s", (char *)server_key, (char*)client_key, expected_server_key);
    if (strcmp((char*)expected_server_key, (char*)server_key) != 0) {
        LOGE(TAG, "Invalid websocket key");
        return -1;
    }
    return 0;
}

static int ws_write(transport_handle_t t, const char *buff, int len, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    char ws_header[MAX_WEBSOCKET_HEADER_SIZE];
    char *mask;
    int header_len = 0, i;
    char *buffer = (char *)buff;
    int poll_write;
    if ((poll_write = transport_poll_write(ws->parent, timeout_ms)) <= 0) {
        return poll_write;
    }

    /*
    typedef enum {
        WS_SEND_TYPE_BIN = 0,
        WS_SEND_TYPE_TEXT
    } ws_send_type_t;
    */
    if (ws->send_type == 0) {
        ws_header[header_len++] = WS_OPCODE_BINARY | WS_FIN;
    } else {
        ws_header[header_len++] = WS_OPCODE_TEXT | WS_FIN;
    }

    // NOTE: no support for > 16-bit sized messages
    if (len > 125) {
        ws_header[header_len++] = WS_SIZE16 | WS_MASK;
        ws_header[header_len++] = (uint8_t)(len >> 8);
        ws_header[header_len++] = (uint8_t)(len & 0xFF);
    } else {
        ws_header[header_len++] = (uint8_t)(len | WS_MASK);
    }
    mask = &ws_header[header_len];
    // getrandom(ws_header + header_len, 4, 0);
    transport_utils_random((unsigned char *)(ws_header + header_len), 4);
    header_len += 4;

    for (i = 0; i < len; ++i) {
        buffer[i] = (buffer[i] ^ mask[i % 4]);
    }
    if (transport_write(ws->parent, ws_header, header_len, timeout_ms) != header_len) {
        LOGE(TAG, "Error write header");
        return -1;
    }
    return transport_write(ws->parent, buffer, len, timeout_ms);
}

static int ws_read(transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    int payload_len;
    int payload_len_buff = len;
    char *data_ptr = buffer, mask, *mask_key = NULL;
    // char opcode;
    int rlen;
    int poll_read;
    if ((poll_read = transport_poll_read(ws->parent, timeout_ms)) <= 0) {
        return poll_read;
    }

    // Receive and process header first (based on header size)
    int header = 2;
    if ((rlen = transport_read(ws->parent, buffer, header, timeout_ms)) <= 0) {
        LOGE(TAG, "Error read data");
        return rlen;
    }
    // opcode = (*data_ptr & 0x0F);
    data_ptr ++;
    mask = ((*data_ptr >> 7) & 0x01);
    payload_len = (*data_ptr & 0x7F);
    data_ptr++;
    // LOGD(TAG, "Opcode: %d, mask: %d, len: %d\r\n", opcode, mask, payload_len);
    if (payload_len == 126) {
        // headerLen += 2;
        if ((rlen = transport_read(ws->parent, data_ptr, header, timeout_ms)) <= 0) {
            LOGE(TAG, "Error read data");
            return rlen;
        }
        payload_len = data_ptr[0] << 8 | data_ptr[1];
        payload_len_buff = len - 4;
        data_ptr += 2;
    } else if (payload_len == 127) {
        // headerLen += 8;
        header = 8;
        if ((rlen = transport_read(ws->parent, data_ptr, header, timeout_ms)) <= 0) {
            LOGE(TAG, "Error read data");
            return rlen;
        }

        if (data_ptr[0] != 0 || data_ptr[1] != 0 || data_ptr[2] != 0 || data_ptr[3] != 0) {
            // really too big!
            payload_len = 0xFFFFFFFF;
        } else {
            payload_len = data_ptr[4] << 24 | data_ptr[5] << 16 | data_ptr[6] << 8 | data_ptr[7];
        }
        data_ptr += 8;
        payload_len_buff = len - 10;
    }
    // Then receive and process payload
    if ((rlen = transport_read(ws->parent, data_ptr, payload_len, timeout_ms)) <= 0) {
        LOGE(TAG, "Error read data");
        return rlen;
    }
    if (payload_len > payload_len_buff) {
        LOGD(TAG, "Actual data received (%d) are longer than mqtt buffer (%d)", payload_len, payload_len_buff);
        payload_len = payload_len_buff;
    }

    if (mask) {
        mask_key = data_ptr;
        data_ptr += 4;
        for (int i = 0; i < payload_len; i++) {
            buffer[i] = (data_ptr[i] ^ mask_key[i % 4]);
        }
    } else {
        memmove(buffer, data_ptr, payload_len);
    }
    return payload_len;
}

static int ws_poll_read(transport_handle_t t, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return transport_poll_read(ws->parent, timeout_ms);
}

static int ws_poll_write(transport_handle_t t, int timeout_ms)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return transport_poll_write(ws->parent, timeout_ms);;
}

static int ws_close(transport_handle_t t)
{
    transport_ws_t *ws = transport_get_context_data(t);
    return transport_close(ws->parent);
}

static web_err_t ws_destroy(transport_handle_t t)
{
    transport_ws_t *ws = transport_get_context_data(t);
    if (ws && ws->buffer) {
        aos_free(ws->buffer);
    }
    aos_free(ws);
    return 0;
}

void transport_ws_set_path(transport_handle_t t, const char *path)
{
    transport_ws_t *ws = transport_get_context_data(t);
    ws->path = path;
}

// void transport_ws_set_protocol(transport_handle_t t, const char *protocol)
// {
//     transport_ws_t *ws = transport_get_context_data(t);
//     ws->protocol = protocol;
// }

void transport_ws_set_sendtype(transport_handle_t t, int send_type)
{
    transport_ws_t *ws = transport_get_context_data(t);
    ws->send_type = send_type;
}

transport_handle_t transport_ws_init(transport_handle_t parent_handle,
                                        const char *path,
                                        const char *protocol,
                                        int send_type,
                                        int buffer_size)
{
    transport_handle_t t = transport_init();
    transport_ws_t *ws = aos_zalloc(sizeof(transport_ws_t));
    TRANSPORT_MEM_CHECK(TAG, ws, return NULL);
    ws->parent = parent_handle;

    // ws->path = strdup("/");
    // TRANSPORT_MEM_CHECK(TAG, ws->path, return NULL);
    // ws->protocol = aos_zalloc(16);
    // TRANSPORT_MEM_CHECK(TAG, ws->protocol, {
    //     aos_free(ws->path);
    //     return NULL;
    // });
    ws->path = path;
    ws->protocol = protocol;
    ws->send_type = send_type;
    ws->buffer_size = buffer_size;
    ws->buffer = aos_zalloc(ws->buffer_size);
    TRANSPORT_MEM_CHECK(TAG, ws->buffer, {
        // aos_free(ws->path);
        // aos_free(ws->protocol);
        aos_free(ws);
        return NULL;
    });

    transport_set_func(t, ws_connect, ws_read, ws_write, ws_close, ws_poll_read, ws_poll_write, ws_destroy);
    // webocket underlying transfer is the payload transfer handle
    transport_set_parent_transport_func(t, ws_get_payload_transport_handle);

    transport_set_context_data(t, ws);
    return t;
}

